#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define MAX_MESSAGE_SIZE 100
static char kernel_buffer[MAX_MESSAGE_SIZE];

static ssize_t write_to_kernel(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos)
{
    // TODO Bonus 1: check if the user is trying to write more than MAX_MESSAGE_SIZE bytes
    // if so, return -EINVAL (Invalid argument)
    if (count > MAX_MESSAGE_SIZE)
        return -EINVAL;

    // TODO 1: copy from the user buffer to kernel buffer by using copy_from_user()
    // Hint: Only one line of code is needed
    if (copy_from_user(kernel_buffer, user_buffer, min((size_t)count, (size_t)MAX_MESSAGE_SIZE)))
        return -EFAULT;

    pr_info("Received from user space: %s\n", kernel_buffer);
    return count;
}

static ssize_t read_from_kernel(struct file *file, char __user *user_buffer, size_t count, loff_t *ppos)
{
    size_t len = strlen(kernel_buffer);
    size_t to_copy = min(count, len - *ppos);

    // TODO 2: 
    // General solution: initialize retval to count
    // Bonus solution for Test Case 5: initialize retval to the minimum value between count and the length of the message
    // Hint: Only one line of code is needed
    ssize_t retval = to_copy;

    // TODO 3: copy from the kernel buffer to user buffer by using copy_to_user()
    // Hint: Only one line of code is needed
    if (copy_to_user(user_buffer, kernel_buffer + *ppos, retval))
        return -EFAULT;

    // TODO 4: update the current position of the file
    // Hint: Only one line of code is needed
    *ppos += retval;

    pr_info("Read %zd bytes from kernel space starting at position %lld\n", retval, *ppos);
    pr_info("Read from kernel space: %.*s\n", (int)retval, kernel_buffer + *ppos);

    return retval;
}

static const struct proc_ops proc_operations = {
    .proc_write = write_to_kernel,
    .proc_read = read_from_kernel,
};

static int __init vfs_init(void)
{
    // TODO 6: create a proc entry at /proc/my_proc_file
    proc_create("my_proc_file", 0666, NULL, &proc_operations);

    pr_info("Module loaded\n");
    return 0;
}

static void __exit vfs_exit(void)
{
    // TODO 7: remove the proc entry at /proc/my_proc_file
    remove_proc_entry("my_proc_file", NULL);

    pr_info("Module unloaded\n");
}

module_init(vfs_init);
module_exit(vfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ananya Yadav");
MODULE_DESCRIPTION("CSE330 Project 3 Virtual File System");
MODULE_VERSION("0.1");
