// --- 1. leak_incremental_module.c ---
// This module creates a /proc entry that, when written to, allocates
// additional memory that is NOT freed. This allows for observing
// consecutive, incremental growth in slab usage via /proc/slabinfo.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>   // kmalloc, kfree
#include <linux/init.h>
#include <linux/proc_fs.h> // For /proc file system
#include <linux/uaccess.h> // For copy_from_user
#include <linux/string.h>  // For memset
#include <linux/seq_file.h> // For simple_read_from_buffer (used by proc_read)

#define INCREMENTAL_LEAK_SIZE (1024 * 16) // 16 KB per increment
#define MAX_INCREMENTS 100 // Max number of times we can increment
#define PROC_FILENAME "incremental_leak"

static struct proc_dir_entry *proc_entry;
static void *leaked_pointers[MAX_INCREMENTS]; // Array to hold pointers to leaked memory
static int current_increments = 0; // Tracks how many blocks have been leaked

// Function called when writing to /proc/incremental_leak
static ssize_t incremental_leak_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    // We don't care about the content written, just that a write occurred.
    // Read a dummy character to consume the input.
    char kbuf[2];
    if (count > sizeof(kbuf) - 1) {
        count = sizeof(kbuf) - 1;
    }
    if (copy_from_user(kbuf, buf, count)) {
        return -EFAULT; // Failed to copy from user space
    }
    kbuf[count] = '\0'; // Null-terminate for safety, though not strictly needed for logic

    if (current_increments < MAX_INCREMENTS) {
        // Allocate a new block of memory that will be leaked
        leaked_pointers[current_increments] = kmalloc(INCREMENTAL_LEAK_SIZE, GFP_KERNEL);
        if (!leaked_pointers[current_increments]) {
            printk(KERN_ERR "incremental_leak_module: Failed to allocate incremental block %d. Out of memory?\n", current_increments);
            return -ENOMEM; // Return error if allocation fails
        }
        // Fill the allocated memory with a pattern (optional, but good for debugging)
        memset(leaked_pointers[current_increments], 0xCC, INCREMENTAL_LEAK_SIZE);
        printk(KERN_INFO "incremental_leak_module: Allocated incremental block %d (%lu bytes) at %p.\n",
               current_increments, (unsigned long)INCREMENTAL_LEAK_SIZE, leaked_pointers[current_increments]);
        current_increments++; // Increment the counter for leaked blocks
    } else {
        printk(KERN_WARNING "incremental_leak_module: Max increments reached (%d). Cannot allocate more.\n", MAX_INCREMENTS);
    }

    *ppos += count; // Update file position (important for procfs writes)
    return count; // Report how many bytes were "written"
}

// Function called when reading from /proc/incremental_leak (optional, for debugging)
static ssize_t incremental_leak_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    char s[128];
    int len = snprintf(s, sizeof(s), "Current leaked increments: %d (Total %lu bytes)\n",
                       current_increments, (unsigned long)current_increments * INCREMENTAL_LEAK_SIZE);
    // Use simple_read_from_buffer to copy data to user space
    return simple_read_from_buffer(buf, count, ppos, s, len);
}

// File operations structure for the /proc entry
static const struct proc_ops incremental_leak_fops = {
    .proc_read = incremental_leak_read,
    .proc_write = incremental_leak_write,
};

// Module initialization function
static int __init incremental_leak_module_init(void) {
    printk(KERN_INFO "incremental_leak_module: Loading. Creating /proc/%s for incremental leaks.\n", PROC_FILENAME);
    // Create the /proc entry with read/write permissions (0666)
    proc_entry = proc_create(PROC_FILENAME, 0666, NULL, &incremental_leak_fops);
    if (!proc_entry) {
        printk(KERN_ERR "incremental_leak_module: Failed to create /proc/%s entry. Check permissions/kernel config.\n", PROC_FILENAME);
        return -ENOMEM; // Return error if creation fails
    }
    return 0; // Success
}

// Module exit (cleanup) function
static void __exit incremental_leak_module_exit(void) {
    printk(KERN_INFO "incremental_leak_module: Unloading. Removing /proc/%s.\n", PROC_FILENAME);
    proc_remove(proc_entry); // Remove the /proc entry

    // Intentionally NOT freeing the allocated memory here.
    // This is crucial for the test case: the leaks persist after module unload,
    // allowing your user-space tool to detect them.
    printk(KERN_INFO "incremental_leak_module: All %d allocated blocks are now leaked (persisting until reboot).\n", current_increments);
    // If you *did* want to free them (e.g., for a "no-leak" version):
    // for (int i = 0; i < current_increments; i++) {
    //     if (leaked_pointers[i]) kfree(leaked_pointers[i]);
    // }
}

// Register module init and exit functions
module_init(incremental_leak_module_init);
module_exit(incremental_leak_module_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Team");
MODULE_DESCRIPTION("Kernel module to create incremental memory leaks via /proc for testing user-space detectors.");
MODULE_VERSION("0.1");