/*
 * module.c
 *
 *  Created on: 16/02/2020
 *      Author: paulo
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kprobes.h>

#define KP_SYMBOL_NAME "_do_fork"

//prototypes
void kprobe_post_handler(struct kprobe *kprobe, struct pt_regs *regs,
		unsigned long flags);

//flow variables
static struct kprobe do_fork_probe = {
		.post_handler = kprobe_post_handler,
		.symbol_name = KP_SYMBOL_NAME
};

void kprobe_post_handler(struct kprobe *kprobe, struct pt_regs *regs,
		unsigned long flags)
{
	pr_info("%s: kprobe [%s] just triggered\n", KBUILD_MODNAME, KP_SYMBOL_NAME);
}

//module handles
static int __init m_init(void)
{
	int ret;
	ret = register_kprobe(&do_fork_probe);
	if (ret < 0) {
		pr_err("register_kprobe failed, returned %d\n", ret);
		return ret;
	}

	return 0;
}

static void __exit m_exit(void)
{
	unregister_kprobe(&do_fork_probe);
}

module_init(m_init);
module_exit(m_exit);
MODULE_AUTHOR("Paulo Miguel Almeida");
MODULE_LICENSE("GPL v2");
