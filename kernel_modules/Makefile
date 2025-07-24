# --- Makefile ---
# Save this as 'Makefile' in the same directory as your .c files.

# KERNELRELEASE will be set by the kernel's build system
# if not, assume a default (e.g., current running kernel)
ifeq ($(KERNELRELEASE),)
    # If KERNELRELEASE is not set, we are likely running 'make' directly.
    # KERNELDIR should point to your kernel build directory (where headers are).
    # PWD is the current working directory where your module source is.
    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)

# 'modules' is the default target for building kernel modules
modules:
	# Call the kernel's build system (MAKE -C)
	# M=$(PWD) tells the kernel build system where to find your module source
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
else
    # This part is executed by the kernel build system when you run 'make'
    # from the KERNELDIR.
    # obj-m specifies which object files (your .o files) should be built
    # into modules (.ko files).
    obj-m := leak_incremental_module.o \
             large_single_leak_module.o \
             no_leak_correct_module.o \
             multiple_small_leaks_module.o \
             transient_alloc_module.o
endif

# 'clean' target to remove generated files
clean:
	# Call the kernel's build system to clean up module-related files
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	# Remove other common build artifacts
	rm -f *.ko *.mod.c *.o modules.order Module.symvers
