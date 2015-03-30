#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>			// File operations
#include <linux/cdev.h>			// Char device
#include <linux/semaphore.h>		// Semaphores usage
#include <asm/uaccess.h>		// copy_to_user; copy_from_user
#include <linux/kdev_t.h>

MODULE_AUTHOR("Primoz Fiser");
MODULE_DESCRIPTION("Char Device Driver Module");
MODULE_LICENSE("Dual BSD/GPL");

/*
	First create a structure for our
	fake char device.
*/
struct fake_device {
	char data[100];
	struct semaphore sem;
}virtual_device;


/*
	Necessary variables and defines
	for our fake char device.
*/
struct cdev *mcdev;	// m stands for "my"
int major_number;	// Our device major number extracted from
				// dev_t using macro - mknod dev c major minor
//int ret;			// Used to store return values - Small stack!
ssize_t ret;
dev_t dev_num;		// Will hold major number that kernel gives us
				// name --> appears in /proc/devices
char buff1;
char *buffer = &buff1;

#define DEVICE_NAME	"fakedevice"

int device_open(struct inode *inode, struct file *filp) {

	// allow only one process to open this device by using semaphore
	if (down_trylock(&virtual_device.sem))
	{
		printk(KERN_ALERT "fakedevice: could not lock device");
		return -1;
	}

	printk(KERN_INFO "fakedevice: opened device");
	return 0;
}

ssize_t device_read(	struct file* filp,
			char* bufStoreData,
			size_t bufCount,
			loff_t* curOffset )
{
	printk(KERN_INFO "fakedevice: Reading from device");
	ret = copy_to_user(bufStoreData, virtual_device.data, bufCount);
	return ret;
}

ssize_t device_write(struct file* filp, const char* bufSourceData, size_t bufCount, loff_t* curOffset ) {

	printk(KERN_INFO "fakedevice: Writing to device");
	ret = copy_from_user(virtual_device.data, bufSourceData, bufCount);
	//printk(KERN_ALERT "fakedevice: %s", bufSourceData);	/* Could be used to Debug */
	//printk(KERN_ALERT "\nfakedevice: bytes transfered %zd", ret);
	//printk(KERN_ALERT "\n%s", format_dev_t(buffer, dev_num));
	return ret;
}

int device_close(struct inode *inode, struct file *filp) {
	up(&virtual_device.sem);
	printk(KERN_INFO "fakedevice: closed device");
	return 0;
}



struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.write = device_write,
	.read = device_read
};


/*
 *
 * name: driver_entry
 * @param void
 * @return 0
 *
 */
static int driver_entry(void) {

	ret = alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);

	if (ret < 0)	/* Check for Errors */
	{
		printk(KERN_ALERT "fakedevice: failed to allocate a major number");
		return ret; 	// propagate error
	}

	major_number = MAJOR(dev_num);	// extract a major number
	printk(KERN_INFO "fakedevice: major number is %d", major_number);
	printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file", DEVICE_NAME, major_number);

	mcdev = cdev_alloc();	// create our cdev structure, initialize our cdev
	mcdev->ops = &fops;		// struct file_operations
	mcdev->owner = THIS_MODULE;

	// Now that we have created our cdev, we have to add it to the kernel
	// int cdev_add(struct cdev* dev, dev_t num, unsigned int count)
	ret = cdev_add(mcdev, dev_num, 1);

	if (ret < 0) 	// Check for Errors //
	{
		printk(KERN_ALERT "fakedevice: unable to load cdev to kernel");
		return ret;
	}

	// Initialize our semaphore
	sema_init(&virtual_device.sem,1);	// initial value of one

	return 0;
}

/*
 *
 * name: driver_exit
 * @param void
 * @return void
 *
 */
static void driver_exit(void) {
	// unregister everything in reverse order
	cdev_del(mcdev);

	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "fakedevice: unloaded module");
}


module_init(driver_entry);
module_exit(driver_exit);
