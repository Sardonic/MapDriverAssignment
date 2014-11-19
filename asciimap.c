#include "asciimap.h"

static char* initials = "SBDJJS";
static int num_initials = 6;

static driver_status_t status =
{
	false, /* Busy-ness */
	{0}, /* Buffer */
	{0}, /* Static string */
	NULL, /* Buffer's pointer */
	-1, /* major */
	-1 /* minor */
};

static int device_open(inode, file)
	struct inode* inode;
	struct file* file;
{
	/* Uhh... Not sure what I need to do here, yet... */

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

	/* printf("Hey, you're totally reading from me! Good job!\n"); */

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
	int string_index = 0;

	while (length > 0)
	{
		/* Reading from the static buffer for now for testing */
		if (string_index >= STATIC_BSIZE)
		{
			/* Maybe loop back instead of bailing out? I dunno. */
			break;
		}

		put_user(status.string[string_index++], buffer++);
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
	return 0;
}


/* Initialize the module - Register the character device */
int
init_module(void)
{
	int i,
	    j;
	/* Register the character device (atleast try) */
	status.major = register_chrdev
	(
		0,
		DEVICE_NAME,
		&Fops
	);

	/* This is kinda unsafe... Have some seriously strange names
	 * and arithmetic going on here. */
	for (i = 0; i < STATIC_ROWSIZE; i++)
	{
		for (j = 0; j < STATIC_COLSIZE; j++)
		{
			char ch = initials[(STATIC_ROWSIZE * i + j) % num_initials];
			status.string[STATIC_ROWSIZE * i + j] = ch;
		}
		status.string[STATIC_ROWSIZE * i + STATIC_COLSIZE - 1] = '\n';
	}

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
		"mknod %s c %d <minor>\n\n" \
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
