// --- 4. multiple_small_leaks_module.c ---
// This module allocates many small blocks of varying sizes that are NOT freed.
// This tests if your detector can track growth across multiple, potentially different, slab caches.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/string.h>

#define NUM_SMALL_LEAKS 20 // Number of small blocks to leak
static void *small_leaked_ptrs[NUM_SMALL_LEAKS]; // Array to hold pointers
// Array of varying sizes to allocate. These will map to different slab caches.
static const size_t small_leak_sizes[] = {
    32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, // Sizes from 32B to 16KB
    32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384  // Repeat to ensure enough blocks
};

static int __init multiple_small_leaks_module_init(void) {
    int i;
    printk(KERN_INFO "multiple_small_leaks_module: Loading. Allocating %d small leaks of varying sizes.\n", NUM_SMALL_LEAKS);

    for (i = 0; i < NUM_SMALL_LEAKS; i++) {
        // Use modulo to cycle through the defined sizes
        size_t current_size = small_leak_sizes[i % (sizeof(small_leak_sizes) / sizeof(small_leak_sizes[0]))];
        small_leaked_ptrs[i] = kmalloc(current_size, GFP_KERNEL);
        if (!small_leaked_ptrs[i]) {
            printk(KERN_ERR "multiple_small_leaks_module: Failed to allocate small block %d (size %zu). Out of memory?\n", i, current_size);
            // Clean up any already allocated blocks if an error occurs
            for (int j = 0; j < i; j++) {
                if (small_leaked_ptrs[j]) kfree(small_leaked_ptrs[j]);
            }
            return -ENOMEM;
        }
        memset(small_leaked_ptrs[i], 0xEE, current_size); // Fill with pattern
        // printk(KERN_INFO "multiple_small_leaks_module: Allocated small block %d (%zu bytes) at %p.\n", i, current_size, small_leaked_ptrs[i]);
    }
    printk(KERN_INFO "multiple_small_leaks_module: All %d small blocks allocated and will be leaked.\n", NUM_SMALL_LEAKS);
    return 0;
}

static void __exit multiple_small_leaks_module_exit(void) {
    // Intentionally NOT freeing small_leaked_ptrs.
    printk(KERN_INFO "multiple_small_leaks_module: Unloading. All %d small blocks are now leaked (persisting until reboot).\n", NUM_SMALL_LEAKS);
}

module_init(multiple_small_leaks_module_init);
module_exit(multiple_small_leaks_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Team");
MODULE_DESCRIPTION("Kernel module to create multiple small, dispersed memory leaks.");
MODULE_VERSION("0.1");
