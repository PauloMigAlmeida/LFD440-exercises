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

//definitions
#define ENTRY_NAME "my_debugfs_entry"

//flow variables
static struct dentry *f_entry;
static u32 value = 0;


//module methods
static int __init m_init(void)
{
	int ret = 0;
	f_entry = debugfs_create_u32(ENTRY_NAME, S_IRUGO | S_IWUSR ,NULL, &value);
	if(unlikely(!f_entry && IS_ERR(f_entry)))
	{
		pr_info("%s: had troubled creating %s entry\n", KBUILD_MODNAME, ENTRY_NAME);
		ret = -1;
	}
	else
		pr_info("%s: created dentry successfully\n", KBUILD_MODNAME);

	return ret;
}

static void __exit m_exit(void)
{
	if(likely(f_entry))
	{
		debugfs_remove(f_entry);
		pr_info("%s: removed dentry successfully\n", KBUILD_MODNAME);
	}
}

module_init(m_init);
module_exit(m_exit);
MODULE_AUTHOR("Paulo Miguel Almeida");
MODULE_LICENSE("GPL v2");
