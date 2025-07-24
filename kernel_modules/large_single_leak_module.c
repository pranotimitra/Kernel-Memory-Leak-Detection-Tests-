// --- 2. large_single_leak_module.c ---
// This module allocates a single, very large block of kernel memory
// that is NOT freed on module unload.
// Designed to cause a significant, sudden jump in slab usage (high growth percentage)
// detectable by your user-space tool.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>   // kmalloc, kfree
#include <linux/init.h>
#include <linux/string.h> // For memset

#define LARGE_LEAK_SIZE (1024 * 1024 * 10) // 10 MB leak (adjust if too large/small for your system)

static void *large_leaked_ptr = NULL; // Pointer to the large leaked block

// Module initialization function
static int __init large_single_leak_module_init(void) {
    printk(KERN_INFO "large_single_leak_module: Loading. Attempting to leak %lu bytes (approx 10MB).\n", (unsigned long)LARGE_LEAK_SIZE);

    // Allocate the large block of memory
    large_leaked_ptr = kmalloc(LARGE_LEAK_SIZE, GFP_KERNEL); // GFP_KERNEL is standard flag
    if (!large_leaked_ptr) {
        printk(KERN_ERR "large_single_leak_module: Failed to allocate large block. No leak will occur.\n");
        return -ENOMEM; // Return error if allocation fails
    }

    // Fill the allocated memory with a pattern (optional)
    memset(large_leaked_ptr, 0xDE, LARGE_LEAK_SIZE);
    printk(KERN_INFO "large_single_leak_module: Large memory block allocated at %p. This will be leaked.\n", large_leaked_ptr);

    return 0; // Success
}

// Module exit (cleanup) function
static void __exit large_single_leak_module_exit(void) {
    // Intentionally NOT freeing large_leaked_ptr.
    // This creates a persistent leak for your user-space tool to detect.
    printk(KERN_INFO "large_single_leak_module: Unloading. The %lu bytes at %p are now leaked (persisting until reboot).\n", (unsigned long)LARGE_LEAK_SIZE, large_leaked_ptr);
    // If you *did* want to free it: kfree(large_leaked_ptr);
}

// Register module init and exit functions
module_init(large_single_leak_module_init);
module_exit(large_single_leak_module_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Team");
MODULE_DESCRIPTION("Kernel module to create a single large memory leak for growth percentage testing.");
MODULE_VERSION("0.1");
