#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <asm/current.h>
#include <linux/sched.h>

/* Name of the module's parameters */
static int param_int = 0;	 
static char *param_char = "Hello World";


/* module_pram(var_name, var_type, permissions); */
/* parameters can be read at /sys/module* example: cat /sys/module/module_param/parameters/param_char */
module_param(param_int, int, S_IRUSR | S_IWUSR);
module_param(param_char, charp, S_IRUSR | S_IWUSR);

static int __init module_param_init(void) {
	printk(KERN_ALERT "\nPassing parameters: insmod module_param.ko param_char='\"Pozdravljen svet\"' param_int=5 ");
	printk(KERN_ALERT "\nPassed string: %s Passed integer: %d", param_char, param_int);
	
	printk(KERN_ALERT "Current proces is \"%s\" (PID %i)\n", current->comm, current->pid);
	return 0;
}

static void __exit module_param_exit(void) {
	printk(KERN_ALERT "\nBye");
}

module_init(module_param_init);
module_exit(module_param_exit);


MODULE_AUTHOR("Primoz Fiser");
MODULE_DESCRIPTION("module_param module");
MODULE_LICENSE("GPL");

