/*  Random number virtual device. */
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DRIVER_AUTHOR "Primoz Fiser <primoz.fiser@norik.com>"
#define DRIVER_DESC   "random number generator device"

/* Declare license and provide authorship and description. */
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

/* Declare all functions. */
static int init_myrandom(void);
static void cleanup_myrandom(void);
static int open_myrandom(struct inode *, struct file *);
static int close_myrandom(struct inode *, struct file *);
static ssize_t read_myrandom(struct file *, char *, size_t, loff_t *);
static ssize_t write_myrandom(struct file *, const char *, size_t, loff_t *);
static unsigned char myrandom(void);

/* Code */
static int Major;
static struct file_operations fops = {
  .read = read_myrandom,
  .release = close_myrandom,
  .open = open_myrandom,
  .write = write_myrandom
};

static int init_myrandom(void)
{
  printk(KERN_ALERT "myrandom init\n");


  Major = register_chrdev(0, "myrandom", &fops);
  if (Major < 0) {
    printk(KERN_ALERT "Registering myrandom device failed: %d\n", Major);
    return Major;
  }

  printk("Myrandom assigned major number %d\n", Major);

  return 0;
}

static void cleanup_myrandom(void)
{
  /* Unregister myrandom device. */
  unregister_chrdev(Major, "myrandom");
  printk(KERN_ALERT "myrandom stopped\n");
}

static int myrandom_in_use = 0;
static int bytes_read;

static int open_myrandom(struct inode *inode, struct file *file)
{
  if (myrandom_in_use)
    return -EBUSY;
  myrandom_in_use++;
  bytes_read = 0;
  return 0;
}

static int close_myrandom(struct inode *inode, struct file *file)
{
  if (myrandom_in_use)
    myrandom_in_use--;
  return 0;
}


/* Generate random letters and numbers.  Algorithm from Wikipedia. */
static unsigned char myrandom(void)
{
  static char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  static unsigned int m_w = 0x12345678;
  static unsigned int m_z = 0x87654321;
  int myrand;

  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  
  myrand = (m_z << 16) + m_w;
  myrand = (myrand >> 16) % sizeof(letters);
  
  return letters[myrand];
}

static ssize_t read_myrandom(struct file *filp, char *buf, size_t len, loff_t *ofs)
{
  unsigned char rand_val;
  int count = 0;

  /* Return EOF when all bytes read. */
  if (bytes_read == 100) 
    return 0;

  while (len-- > 0 && bytes_read++ < 100) {
    rand_val = myrandom();
    put_user(rand_val, buf++);
    count++;
  }
  /* Return number of bytes transferred. */
  return count;
}

static ssize_t write_myrandom(struct file *filp, const char *buf, size_t len, loff_t *ofs)
{
  return 0;
}

module_init(init_myrandom);
module_exit(cleanup_myrandom);
