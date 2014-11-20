#include "asciimap.h"

extern int errno;

static char* initials = "SBDJJS";
static int num_initials = 6;

static driver_status_t status =
{
	false, /* Busy-ness */
	{0}, /* Buffer */
	{0}, /* Static string */
	0, /* width */
	0, /* height */
	NULL, /* Buffer's pointer */
	-1, /* major */
	-1 /* minor */
};

static int device_open(inode, file)
	struct inode* inode;
	struct file* file;
{
	status.minor = inode->i_rdev >> 8;
	status.minor = inode->i_rdev & 0xFF;

	printk
	(
		"Device: %d.%d, busy: %d\n",
		status.major,
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

	while (length > 0 && *status.buf_ptr)
	{
		put_user(*status.buf_ptr++, buffer++);
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
	/* TODO: Test for width and height to see if we're adding to the map
	 * or just overwritting existing data. */
	int bytes_written = 0;

	while (length > 0 && status.buf_ptr - status.buf < BSIZE - 1) /* saving room for \0 */
	{
		get_user(*status.buf_ptr, buffer);
		status.buf_ptr++;
		buffer++;
		bytes_written++;
		length--;
	}

	status.buf[BSIZE - 1] = '\0';

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

/* Initialize the module - Register the character device */
int
init_module(void)
{
	/* Register the character device (atleast try) */
	status.major = register_chrdev
	(
		0,
		DEVICE_NAME,
		&Fops
	);

	/* Fill our array with initials, sequentially. */
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

	/* This line assumes the string is filled completely with
	 * useful information, except for the last character. */
	status.string[STATIC_BSIZE - 1] = '\0';

	/* Have to use our own memcpy. Ho-hum. */
	{
		const char* src = status.string;
		char* dst = status.buf;
		while ((*dst++ = *src++)) {}

		status.width = STATIC_COLSIZE;
		status.height = STATIC_ROWSIZE;
	}

	status.buf_ptr = status.buf;
	
	/* Negative values signify an error */
	if(status.major < 0)
	{
		printk
		(
			"Sorry, registering the ASCII device failed with %d\n",
			status.major
		);

		return status.major;
	}

	printk
	(
		"\n********** MKNOD MSG BEGIN **********\n" \
		"Registeration of asciimap.ko is a success. The major device number is %d.\n",
		status.major
	);

	printk
	(
		"If you want to talk to the device driver,\n" \
		"you'll have to create a device file. \n" \
		"We suggest you use:\n\n" \
		"mknod %s c %d 0\n\n" \
		"You can try different minor numbers and see what happens.\n",
		DEVICE_NAME,
		status.major
	);

	return SUCCESS;
}

/* Cleanup - unregister the appropriate file from /proc */
void
cleanup_module(void)
{
	unregister_chrdev(status.major, DEVICE_NAME);
}
