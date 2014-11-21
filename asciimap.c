/* The necessary header files */

/* Standard in kernel modules */
#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */

/* For character devices */
#include <linux/fs.h>       /* The character device
                             * definitions are here
                             */

#include <asm/uaccess.h>  /* for put/get_user */

#include "asciimap.h"

/* Device Declarations **************************** */

/* The maximum length of the message from the device */
/* #define DRV_BUF_SIZE 80 */
/* Storing this by hand. Inelegant, I know... */
#define BSIZE 6400

#define STATIC_ROWSIZE 50
#define STATIC_COLSIZE 51 /* save space for \n at the end */
#define STATIC_BSIZE ((STATIC_COLSIZE * STATIC_ROWSIZE) + 1) /* save space for \0 at the end */

/* Return codes */

#define SUCCESS      0


/*
 * Driver status structure
 */
typedef struct _driver_status
{
	/* Is the device open right now? Used to prevent
	 * concurent access into the same device
	 */
	bool  busy;

	/* The actual map data */
	char  buf[BSIZE];

	/* Original map data */
	char string[STATIC_BSIZE];

	/* Map size */
	int map_byte_length;

	/* How far did the process reading the message
	 * get? Useful if the message is larger than the size
	 * of the buffer we get to fill in device_read.
	 */
	char* buf_ptr;

	/* The minor device number for the device.
	 */
	int   minor;
} driver_status_t;


static driver_status_t status =
{
	.busy = false, /* Busy-ness */
	.buf = {0}, /* Buffer */
	.string ={0}, /* Static string */
	.map_byte_length = 0, /* width */
	.buf_ptr = NULL, /* total length */
	.minor = -1 /* minor */
};

/*
 * Driver funcitons' prototypes
 */
static int device_open(struct inode*, struct file*);
static int  device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);
static loff_t device_seek(struct file *, loff_t, int);
static int device_ioctl(struct inode*, struct file*, unsigned int, unsigned int);
/* Kernel module-related */

/* Module Declarations ***************************** */

/* This structure will hold the functions to be
 * called when a process does something to the device
 * we created. Since a pointer to this structure is
 * kept in the devices table, it can't be local to
 * init_module. NULL is for unimplemented functions.
 */
struct file_operations Fops =
{
	.read = device_read,
	.write = device_write,
	.llseek = device_seek,
	.ioctl = device_ioctl,
	.open = device_open,
	.release = device_release
};



int init_module(void);
void cleanup_module(void);



extern int errno;

static char* initials = "SBDJJS";
static int num_initials = 6;

static int mem_copy(char* dst, const char* src)
{
	int count = 0;
	/* Have to use our own mem_copy. Ho-hum. */
	{
		while ((*dst++ = *src++))
		{
			count++;
		}

		/* We copied a '\0' too, but the loop body didn't run */
		count++;
	}
	return count;

}

static void init_static_map()
{
	int i,
	    j;

	for (i = 0; i < STATIC_ROWSIZE; i++)
	{
		for (j = 0; j < STATIC_COLSIZE - 1; j++)
		{
			char ch = initials[(STATIC_COLSIZE * i + j) % num_initials];
			status.string[STATIC_COLSIZE * i + j] = ch;
		}
		status.string[STATIC_COLSIZE * i + j] = '\n';
	}
}

static int device_open(inode, file)
	struct inode* inode;
	struct file* file;
{
	status.minor = inode->i_rdev >> 8;
	status.minor = inode->i_rdev & 0xFF;

	printk
	(
		KERN_INFO "Device: %d.%d, busy: %d\n",
		MAJOR_NUM,
		status.minor,
		status.busy
	);

	/* Sorry, I'm a one-process kind of driver... */
	if (status.busy)
		return -EBUSY;

	status.busy = true;

	return SUCCESS;
}

static int device_release(inode, file)
	struct inode* inode;
	struct file*  file;
{
#ifdef _DEBUG
	printk ("device_release(%p,%p)\n", inode, file);
#endif

	/* We're now ready for our next caller */
	status.busy = false;

	return SUCCESS;
}


/* This function is called whenever a process which
 * have already opened the device file attempts to
 * read from it.
 */
static ssize_t device_read(file, buffer, length, offset)
	struct file* file;
    char*        buffer;  /* The buffer to fill with data */
    size_t       length;  /* The length of the buffer */
    loff_t*      offset;  /* Our offset in the file */
{
	int bytes_read = 0;
	int err = 0;

	while (length > 0 && *status.buf_ptr)
	{
		err = put_user(*status.buf_ptr++, buffer++);

		if (err == -EFAULT)
		{
#ifdef _DEBUG
			printk("asciimap::device_read() - Hit error %d\n");
#endif
			return err;
		}

		bytes_read++;
		length--;
	}

#ifdef _DEBUG
	printk
	(
		 "asciimap::device_read() - Read %d bytes, %d left\n",
		 bytes_read,
		 length
	);
#endif

	return bytes_read;
}

/* This function is called when somebody tries to write
 * into our device file.
 */
static ssize_t device_write(file, buffer, length, offset)
	struct file* file;
	const char*  buffer;  /* The buffer */
	size_t       length;  /* The length of the buffer */
	loff_t*      offset;  /* Our offset in the file */
{
	int bytes_written = 0;
	int err = 0;

	while (length > 0 && status.buf_ptr - status.buf < BSIZE - 1) /* saving room for \0 */
	{
		/* get_user is the weirdest macro ever. */
		err = get_user(*status.buf_ptr, buffer);

		if (err == -EFAULT)
		{
#ifdef _DEBUG
			printk("asciimap::device_write() - Hit error %d\n", err);
#endif
			return err;
		}

		status.buf_ptr++;
		buffer++;
		bytes_written++;
		length--;

		if (status.buf_ptr - status.buf > status.map_byte_length)
		{
			status.map_byte_length++;
		}
	}

	if (status.buf_ptr - status.buf == status.map_byte_length)
	{
		*(status.buf_ptr + 1) = '\0';
	}

#ifdef _DEBUG
	printk
	(
		"asciimap::device_write() - Read %d bytes.\n" \
		"Map is now %d bytes long\n",
		bytes_written,
		status.map_byte_length
	);
	/* printk("The length of the map is now %d bytes\n", status.map_byte_length); */
#endif

	return bytes_written;
}

static loff_t device_seek(struct file* file, loff_t offset, int whence)
{
	int b_err = 1;
	int new_ptr_pos;
	switch(whence)
	{
		case SEEK_SET:
			if(offset < BSIZE && offset >= 0)
			{
				status.buf_ptr = (status.buf + offset);
				b_err = 0;
			}
			break;
		case SEEK_CUR:
			new_ptr_pos = offset + (status.buf_ptr - status.buf);
			if(new_ptr_pos < BSIZE && new_ptr_pos > 0)
			{
				status.buf_ptr = (status.buf + new_ptr_pos);
				b_err = 0;
			}
			break;
		case SEEK_END:
			if(offset <= 0 && offset > -BSIZE)
			{
				status.buf_ptr = ((status.buf + BSIZE - 1) + offset);
				b_err = 0;
			}
			break;
		default:
			break;
	}

	if(b_err == 1)
	{
		return -ESPIPE;
	}

	return SUCCESS;
}

/* this function is called when a process tries to do an 
 * ioctl on our devive */
static int device_ioctl(inode, file, ioctl_num, ioctl_param)
	struct inode* inode;
	struct file* file;
	unsigned int ioctl_num; /* number and param for ioctl  */
	unsigned int ioctl_param;
{
	char *temp;

	printk(KERN_INFO "Received ioctl request\n");

	switch	(ioctl_num)
	{
	case IOCTL_RESET_MAP:
		/* So, for some reason, our status.string is getting zeroed out
		 * between the call to init_device and here. Maybe it's in
		 * write or something? I don't really know what the damage is.
		 * 
		 * Anyway, here we have to do the exact steps we did back in
		 * init_module. So here they are, reproduced in full.
		 *
		 * I'm hestitant to make a whole function to do this stuff,
		 * since it shouldn't really be happening anyway. 
		 *
		 * -Scott						 */

		/* Fill our array with initials, sequentially. */
		init_static_map();

		/* This line assumes the string is filled completely with
		 * useful information, except for the last character. */
		status.string[STATIC_BSIZE - 1] = '\0';

		temp = status.buf;
		while(*temp)
		{
			*temp = "\0";
			temp++;	
		}	

		status.map_byte_length = mem_copy(status.buf, status.string) - 1;
		printk(KERN_INFO "New map size: %d\n", status.map_byte_length);
		printk(KERN_INFO "First char of status.string: %c\n", status.string[0]);
		status.buf_ptr = status.buf;
		break;

	case IOCTL_ZERO_OUT:

		temp = status.buf;
		while(*temp)
		{
			*temp = "\0";
			temp++;
		}
		status.buf_ptr =  status.buf;
		break;

	case IOCTL_CHECK_CONSISTENCY:
		printk(KERN_INFO "First char of status.string: %c\n", status.string[0]);
		init_static_map();

		{	
			int width = 0;
			int count = 0;		
			temp = status.buf;
			while(*temp && *temp != '\n') /* Scan the first line of the buffer to get the potential width  */
			{
				width++;
				temp++;	
			}
			printk(KERN_INFO "we have detected a potential length: %d", width);
			temp = status.buf;

			
			while(*temp)
			{
				count++; /*update the index of the line  */
				if(*temp == '\n')
				{
					count--; /*y'a no wat im saian*/
					if(count != width) /*Houston we have a problem*/
					{
					
						printk(KERN_INFO "THE COUNT IS SHORTER THAN THE WIDTH %d - %d", count, width);
						return -1;
					}
					else
					{
						count = 0; /*reset the counter because we are at the end of the line*/
					}
				}
				else if (*temp < 32) 
				{
					printk(KERN_INFO "We have an escape character!: %d", *temp);
					return -1;	/* abort abort abort */
				}

				temp++;
			}
		}
		break;
	default:
		break;
	}	
	
	return SUCCESS;
}
/* Initialize the module - Register the character device */
int
init_module(void)
{
	int err = 0;
	/* Register the character device (atleast try) */
	err = register_chrdev
	(
		MAJOR_NUM,
		DEVICE_NAME,
		&Fops
	);
	
	/* Negative values signify an error */
	if(err < 0)
	{
		printk
		(
			"Sorry, registering the ASCII device failed with %d\n",
			err
		);
return err;
	}

	/* Fill our array with initials, sequentially. */
	init_static_map();

	/* This line assumes the string is filled completely with
	 * useful information, except for the last character. */
	status.string[STATIC_BSIZE - 1] = '\0';

	/* Have to use our own mem_copy. Ho-hum. */
	status.map_byte_length = mem_copy(status.buf, status.string) - 1;

	status.buf_ptr = status.buf;

	printk
	(
		"\n********** MKNOD MSG BEGIN **********\n" \
		"Registeration of asciimap.ko is a success. The major device number is %d.\n",
		MAJOR_NUM
	);

	printk
	(
		"If you want to talk to the device driver,\n" \
		"you'll have to create a device file. \n" \
		"We suggest you use:\n\n" \
		"mknod %s c %d 0\n\n" \
		"You can try different minor numbers and see what happens.\n",
		DEVICE_NAME,
		MAJOR_NUM
	);

	return SUCCESS;
}

/* Cleanup - unregister the appropriate file from /proc */
void
cleanup_module(void)
{
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

