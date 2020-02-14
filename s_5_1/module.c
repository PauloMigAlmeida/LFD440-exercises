/*
 * module.c
 *
 *  Created on: 14/02/2020
 *      Author: paulo
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sysrq.h>

//macros & pre-headers
#define SYSRQ_NEW_KEY 'x'

//prototypes
static void sysrq_new_key_handler(int key);

//flow variables
static bool sysrq_registered = false;
static struct sysrq_key_op my_ops = {
	.handler = &sysrq_new_key_handler,
	.help_msg = "my_ops::help_msg(x)",
	.action_msg = "sysrq(x) was triggered - any custom msg you want :)",
	.enable_mask = SYSRQ_ENABLE_DUMP
};

//sysrq handlers
static void sysrq_new_key_handler(int key)
{
	if(likely(key == SYSRQ_NEW_KEY)){
		pr_info("%s: sysrq_new_key_handler was triggered!\n",KBUILD_MODNAME);
	}
}

//module methods
static int __init my_init(void)
{
	int ret = -1;
	// X isn't used in the x86_64, hence for educational purposes I will use it here too
	if(likely(!register_sysrq_key(SYSRQ_NEW_KEY,&my_ops)))
	{
		ret = 0;
		sysrq_registered = true;
		pr_info("%s: sysrq_new_key_handler was successfully registered\n", KBUILD_MODNAME);
	}
	else
	{
		pr_info("%s: sysrq_new_key_handler registration failed\n", KBUILD_MODNAME);
	}

	return ret;
}

static void __exit my_exit(void)
{
	if(likely(sysrq_registered && !unregister_sysrq_key(SYSRQ_NEW_KEY, &my_ops)))
	{
		pr_info("%s: sysrq_new_key_handler was successfully unregistered\n", KBUILD_MODNAME);
	}
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Paulo Miguel Almeida");
MODULE_LICENSE("GPL v2");
