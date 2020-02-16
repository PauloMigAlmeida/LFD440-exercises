/*
 * module.c
 *
 *  Created on: 16/02/2020
 *      Author: paulo
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>

//definitions
#define PROCFS_FILENAME "my_proc_file"
#define MAX_CHAR_LENGTH 50

//prototypes
ssize_t proc_read(struct file *file, char __user *buf, size_t lbuf,
		loff_t *ppos);
ssize_t proc_write(struct file *file, const char __user *buf, size_t lbuf,
		loff_t *ppos);

//flow variables
static int val = 0;
module_param(val, int, S_IRUSR | S_IRGRP);
static char k_string[MAX_CHAR_LENGTH];
static struct proc_dir_entry *proc_entry;
static const struct file_operations fops = {
		.owner = THIS_MODULE,
		.read = proc_read,
		.write = proc_write,
};

//proc_fs operations
ssize_t proc_read(struct file *file, char __user *buf, size_t lbuf,
		loff_t *ppos)
{
	int nbytes;
	nbytes = sprintf(k_string, "%d\n", val);
	return simple_read_from_buffer(buf, lbuf, ppos, k_string, nbytes);
}

ssize_t proc_write(struct file *file, const char __user *buf, size_t lbuf,
		loff_t *ppos)
{
	ssize_t rc;
	rc = simple_write_to_buffer(k_string, lbuf, ppos, buf, lbuf);
	sscanf(k_string, "%d", &val);
	return rc;
}

//module handlers
static int __init m_init(void)
{
	int ret = 0;
	proc_entry = proc_create(PROCFS_FILENAME, S_IRUSR | S_IRGRP, NULL,
			&fops);

	if (unlikely(!proc_entry && IS_ERR(proc_entry))) {
		pr_err(
				"%s: something went wrong while creating the %s proc file\n",
				KBUILD_MODNAME, PROCFS_FILENAME);
		ret = -1;
	} else {
		pr_info("%s: proc file %s created successfully\n", KBUILD_MODNAME, PROCFS_FILENAME);
	}

	return ret;
}

static void __exit m_exit(void)
{
	if (likely(proc_entry && !IS_ERR(proc_entry))) {
		proc_remove(proc_entry);
		pr_info("%s: proc file %s removed successfully\n", KBUILD_MODNAME, PROCFS_FILENAME);
	}
}

module_init(m_init);
module_exit(m_exit);
MODULE_AUTHOR("Paulo Miguel Almeida");
MODULE_LICENSE("GPL v2");
