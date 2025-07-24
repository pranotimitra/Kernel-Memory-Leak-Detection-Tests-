// --- 5. transient_alloc_module.c ---
// This module allocates memory, uses it briefly, and frees it
// *before* the module's init function returns.
// Your user-space detector should ideally see no persistent change from this,
// demonstrating it correctly ignores short-lived, correctly managed allocations.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/string.h>

#define TRANSIENT_SIZE (1024 * 8) // 8 KB transient allocation

static int __init transient_alloc_module_init(void) {
    void *buffer = NULL;
    printk(KERN_INFO "transient_alloc_module: Loading. Performing transient allocation.\n");

    buffer = kmalloc(TRANSIENT_SIZE, GFP_KERNEL);
    if (!buffer) {
        printk(KERN_ERR "transient_alloc_module: Failed to allocate transient memory.\n");
        return -ENOMEM;
    }

    memset(buffer, 0xDD, TRANSIENT_SIZE); // Use the buffer
    printk(KERN_INFO "transient_alloc_module: Transient memory allocated at %p, now freeing.\n", buffer);
    kfree(buffer); // Free immediately

    printk(KERN_INFO "transient_alloc_module: Transient allocation completed and freed.\n");
    return 0;
}

static void __exit transient_alloc_module_exit(void) {
    printk(KERN_INFO "transient_alloc_module: Unloading. No persistent memory left from this module.\n");
}

module_init(transient_alloc_module_init);
module_exit(transient_alloc_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Team");
MODULE_DESCRIPTION("Kernel module for transient memory allocation testing.");
MODULE_VERSION("0.1");
