#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

int param_var = 0;	/* Name of the module's param variable */ 

/* module_pram(var_name, var_type, permissions);*/
module_param(param_var, int, S_IRUSR | S_IWUSR);

static int module_param_init(void) {
	printk(KERN_ALERT "\nPassed param = %d", param_var);
	return 0;
}

static void module_param_exit(void) {
	printk(KERN_ALERT "\nBye");
}

module_init(module_param_init);
module_exit(module_param_exit);


MODULE_AUTHOR("Primoz Fiser");
MODULE_DESCRIPTION("module_param module");
MODULE_LICENSE("GPL");
