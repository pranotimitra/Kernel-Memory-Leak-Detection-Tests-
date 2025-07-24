# Kernel Memory Leak Detection Test Cases
This repository contains a set of kernel modules designed to create specific memory allocation scenarios within the Linux kernel. These modules serve as test cases for user-space kernel memory leak detection tools, such as the one developed in the ankushT369/Kernel-Memory-Leak-Detection project.  

The goal of these tests is to verify that a user-space detector can accurately identify different types of kernel memory leaks by observing changes in /proc/slabinfo and /proc/vmstat.

# Repository Structure
- kernel_modules/: Contains the C source code for the kernel modules and their Makefile.
- README.md: This file, providing setup and usage instructions.

# Prerequisites
To compile and run these test cases, you will need:
- A Linux Environment: Preferably a Virtual Machine (e.g., VirtualBox, VMware) where you have sudo (root) access.
- Linux Kernel Headers: Installed for your specific kernel version.
  - Debian/Ubuntu: sudo apt update && sudo apt install linux-headers-$(uname -r)
- GCC and Make: Standard build tools, usually pre-installed or easily installed via your package manager.
- Your User-Space Detector Tool: The compiled executable of your Kernel-Memory-Leak-Detection project (https://github.com/ankushT369/Kernel-Memory-Leak-Detection). Ensure it's accessible from your terminal.

# Setup Instructions
 1. Clone this Repository:
  - git clone https://github.com/your-username/Kernel-Memory-Leak-Detection-Tests.git (Replace your-username with your actual GitHub username).
  - cd Kernel-Memory-Leak-Detection-Tests
 2. Compile the Kernel Modules:
  - Navigate into the kernel_modules directory:
    - cd kernel_modules
  - Run make to compile the .c files into kernel object (.ko) files:
    - make
  - This will generate:
    - leak_incremental_module.ko
    - large_single_leak_module.ko
    - no_leak_correct_module.ko
    - multiple_small_leaks_module.ko
    - transient_alloc_module.ko
  - You can return to the root of the repository: cd ..
 3. Ensure Your Detector is Ready:
  - Make sure your Kernel-Memory-Leak-Detection tool is compiled and its executable is accessible (e.g., in a build/ directory within its own repository). You will run this tool in a separate terminal during testing.

  # Running the Test Cases (Demonstration Guide)
  For each test case, you will typically:
  - Start your detector tool in one terminal (e.g., ./your_detector_tool_executable -diff to show changes).
  - Perform the module operations (load/unload, or write to /proc) in a second terminal.
  - Observe the output of your detector tool in the first terminal.

 ## Important Notes for Demonstration:
 - Root Privileges: Loading/unloading kernel modules and interacting with /proc requires sudo.
 - Reboot Between Leak Tests: After running a test that intentionally leaks memory, the memory remains leaked until the system reboots. For a clean demonstration of subsequent tests, it is highly recommended to reboot your Linux VM after each test that causes a persistent leak. This ensures /proc/slabinfo starts from a a clean baseline.
 - Clear dmesg: Use sudo dmesg -c before each test to clear the kernel log buffer, making it easier to see printk messages from your modules and your detector.
 - Detector Output: Your Kernel-Memory-Leak-Detection tool should be configured to show clear, actionable output (e.g., using a -diff or -top N flag) that highlights changes in slab allocations.

 # Test Case 1: Consecutive Growth Slabs(leak_incremental_module.ko)
 - Goal: Show your tool detecting a leak that grows incrementally over time.
 - Steps:
     1. Clear dmesg: sudo dmesg -c
     2. Terminal 1: Start your detector:/path/to/your/detector/your_detector_tool_executable -diff(Let it run for a few seconds to establish a baseline.)
     3. Terminal 2: Load the module:sudo insmod kernel_modules/leak_incremental_module.ko
     4. Terminal 2: Repeatedly trigger more leaks (wait 5-10 seconds between each for detector to poll) 
     - echo "1" | sudo tee /proc/incremental_leak
     - #Wait 5-10 seconds
     - echo "1" | sudo tee /proc/incremental_leak
     - #Wait 5-10 seconds
     - echo "1" | sudo tee /proc/incremental_leak
     - #Repeat 2-3 more times
      5. Terminal 1: Observe your detector reporting a continuous, increasing growth in a specific slab cache (e.g., kmalloc-16K). Point out how the "total objects" or "total size" for that slab keeps going up with each new leak you trigger.
      6. Terminal 2: Unload the module:sudo rmmod leak_incremental_module.ko

# Test Case 2: Growth Percentage (Large Single Leak) (large_single_leak_module.ko)
- Goal: Show your tool detecting a large, sudden leak that results in a high growth percentage.
- Steps:
    1. Reboot Your System: sudo reboot (Crucial for a clean slate after the previous leak).
    2. Clear dmesg: sudo dmesg -c
    3. Terminal 1: Start your detector
    4. Terminal 2: Load the module:sudo insmod kernel_modules/large_single_leak_module.ko
    5. Terminal 1: Observe your detector reporting a massive, sudden increase in a large slab cache (e.g., kmalloc-10M). Your tool should show a very high growth percentage for this specific slab.
    6. Terminal 2: Unload the module:sudo rmmod large_single_leak_module.ko

# Test Case 3: No Leak (Correct Allocation/Deallocation) (no_leak_correct_module.ko)
- Goal: Verify your tool does not report false positives when memory is correctly managed.
- Steps:
    1. Reboot Your System: sudo reboot (For a clean slate).
    2. Clear dmesg: sudo dmesg -c
    3. Terminal 1: Start your detector
    4. Terminal 2: Load the module:sudo insmod kernel_modules/no_leak_correct_module.ko
    5. Terminal 2: Unload the module:sudo rmmod no_leak_correct_module.ko
    6. Terminal 1: Observe your detector reporting no significant or persistent changes in any slab caches or vmstat counters.

  # Test Case 4: Multiple Small, Dispersed Leaks(multiple_small_leaks_module.ko)
  - Goal: Test your detector's ability to track multiple leaks across different slab sizes.
  - Steps:
    1. Reboot Your System: sudo reboot (For a clean slate).
    2. Clear dmesg: sudo dmesg -c
    3. Terminal 1: Start your detector
    4. Terminal 2: Load the module:sudo insmod kernel_modules/multiple_small_leaks_module.ko
    5. Terminal 2: Unload the module:sudo rmmod multiple_small_leaks_module.ko
    6. Terminal 1: Observe your detector reporting persistent increases in several different slab caches (e.g., kmalloc-32, kmalloc-64, kmalloc-1K, kmalloc-16K, etc.).

    # Test Case 5: Transient Allocation (transient_alloc_module.ko)
- Goal: Show that very short-lived, correctly managed allocations don't trigger leak alerts.
- Steps:
    1. Reboot Your System: sudo reboot (For a clean slate).
    2. Clear dmesg: sudo dmesg -c
    3. Terminal 1: Start your detector
    4. Terminal 2: Load the module:sudo insmod kernel_modules/transient_alloc_module.ko
    5. Terminal 2: Unload the module:sudo rmmod transient_alloc_module.ko
    6. Terminal 1: Observe your detector reporting no significant or persistent changes. If your tool's polling interval is slow enough, it might not even catch the momentary allocation.
