// --- 3. no_leak_correct_module.c ---
// This module allocates memory and immediately frees it.
// This is a crucial "negative" test case to demonstrate your tool *doesn't* report false positives.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/string.h>

#define NO_LEAK_ALLOC_SIZE (1024 * 32) // 32 KB temporary allocation

static int __init no_leak_correct_module_init(void) {
    void *temp_ptr = NULL;

    printk(KERN_INFO "no_leak_correct_module: Loading. Allocating and immediately freeing %lu bytes.\n", (unsigned long)NO_LEAK_ALLOC_SIZE);

    temp_ptr = kmalloc(NO_LEAK_ALLOC_SIZE, GFP_KERNEL);
    if (!temp_ptr) {
        printk(KERN_ERR "no_leak_correct_module: Failed to allocate memory.\n");
        return -ENOMEM;
    }

    memset(temp_ptr, 0xBB, NO_LEAK_ALLOC_SIZE);
    printk(KERN_INFO "no_leak_correct_module: Memory allocated at %p.\n", temp_ptr);

    kfree(temp_ptr); // Correctly free the memory
    printk(KERN_INFO "no_leak_correct_module: Memory at %p successfully freed. No leak expected.\n", temp_ptr);

    return 0;
}

static void __exit no_leak_correct_module_exit(void) {
    printk(KERN_INFO "no_leak_correct_module: Unloading. All memory was correctly managed.\n");
}

module_init(no_leak_correct_module_init);
module_exit(no_leak_correct_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Team");
MODULE_DESCRIPTION("Kernel module to demonstrate correct memory management (no leak).");
MODULE_VERSION("0.1");
