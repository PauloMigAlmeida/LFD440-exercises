/*
 * module.c
 *
 *  Created on: 14/02/2020
 *      Author: paulo
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>


//prototypes
ssize_t debugfs_read (struct file *file, char __user *buf, size_t count, loff_t *ppos);
ssize_t debugfs_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos);

//definitions
#define MAX_STRING_SIZE 50
#define DIR_ENTRY_NAME "my_debugfs_dir_entry"
#define FILE_ENTRY_NAME "my_debugfs_file_entry"

//flow variables
static char kstring[MAX_STRING_SIZE];
static int val = 0;
static struct dentry *dir_entry;
static struct dentry *file_entry;
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = debugfs_read,
	.write = debugfs_write,
};

//debugfs methods
ssize_t debugfs_read (struct file *file, char __user *buf, size_t count, loff_t *ppos)
{

	int nbytes;
	nbytes = sprintf(kstring, "%d\n", val);
	return simple_read_from_buffer(buf, count, ppos, kstring, nbytes);

}
ssize_t debugfs_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int rc;
	int nbytes = count;

	rc = copy_from_user(&kstring, buf, nbytes);
	sscanf(kstring,"%d", &val);

	return nbytes;
}

//helper methods
static inline __always_inline bool validate_debugfs_helper(struct dentry *entry, char* name)
{
	bool ret = true;
	if (unlikely(!entry && IS_ERR(entry))){
		pr_info("%s: had troubled creating %s entry\n", KBUILD_MODNAME, name);
		ret = false;
	}
	else
		pr_info("%s: created dentry '%s' successfully\n", KBUILD_MODNAME, name);
	return ret;
}

static inline __always_inline void remove_debugfs_helper(struct dentry *entry, char* name)
{
	if (likely(entry && !IS_ERR(entry))){
		debugfs_remove(entry);
		pr_info("%s: removed dentry '%s' successfully\n", KBUILD_MODNAME, name);
	}
}

//module methods
static int __init m_init(void)
{
	dir_entry = debugfs_create_dir(DIR_ENTRY_NAME, NULL);
	if (!validate_debugfs_helper(dir_entry, DIR_ENTRY_NAME))
		goto err_0;

	file_entry = debugfs_create_file(FILE_ENTRY_NAME, S_IRUGO | S_IRUSR, dir_entry, NULL, &fops);
	if (!validate_debugfs_helper(file_entry, DIR_ENTRY_NAME))
		goto file_err;

	return 0;

file_err:
	debugfs_remove(dir_entry);
err_0:
	return -1;
}

static void __exit m_exit(void)
{
	remove_debugfs_helper(file_entry, FILE_ENTRY_NAME);
	remove_debugfs_helper(dir_entry, DIR_ENTRY_NAME);
}

module_init(m_init);
module_exit(m_exit);
MODULE_AUTHOR("Paulo Miguel Almeida");
MODULE_LICENSE("GPL v2");
