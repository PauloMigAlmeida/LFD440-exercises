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
int kretprobe_handler(struct kretprobe_instance *kprobe_inst,
		struct pt_regs *regs);
int kretprobe_entry_handler(struct kretprobe_instance *kprobe_inst,
		struct pt_regs *regs);

/* per-instance private data */
struct my_data {
	ktime_t entry_stamp;
};

//flow variables
static const struct kprobe do_fork_probe = {
		.symbol_name = KP_SYMBOL_NAME
};

static struct kretprobe do_fork_kretprobe = {
		.kp = do_fork_probe,
		.handler = kretprobe_handler,
		.data_size = sizeof(struct my_data),
		.maxactive = 10,
};

// entry and handler functions where gotten from kretprobe_example.c
int kretprobe_entry_handler(struct kretprobe_instance *kprobe_inst,
		struct pt_regs *regs)
{
	struct my_data *data;

	if (!current->mm)
		return 1; /* Skip kernel threads */

	data = (struct my_data*)kprobe_inst->data;
	data->entry_stamp = ktime_get();
	return 0;
}

int kretprobe_handler(struct kretprobe_instance *kprobe_inst,
		struct pt_regs *regs)
{
	unsigned long retval = regs_return_value(regs);
	struct my_data *data = (struct my_data*)kprobe_inst->data;
	s64 delta;
	ktime_t now;

	now = ktime_get();
	delta = ktime_to_ns(ktime_sub(now, data->entry_stamp));
	pr_info("%s: returned %lu and took %lld ns to execute\n",
			KP_SYMBOL_NAME, retval, (long long )delta);
	return 0;
}

//module handles
static int __init m_init(void)
{
	int ret;
	ret = register_kretprobe(&do_fork_kretprobe);
	if (ret < 0) {
		pr_err("register_kretprobe failed, returned %d\n", ret);
		return ret;
	}

	return 0;
}

static void __exit m_exit(void)
{
	unregister_kretprobe(&do_fork_kretprobe);

	/* nmissed > 0 suggests that maxactive was set too low. */
	pr_info("Missed probing %d instances of %s\n",
			do_fork_kretprobe.nmissed,
			do_fork_kretprobe.kp.symbol_name);
}

module_init(m_init);
module_exit(m_exit);
MODULE_AUTHOR("Paulo Miguel Almeida");
MODULE_LICENSE("GPL v2");
