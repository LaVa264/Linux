# Linux Device Driver

## 1. An introduction to Device Drivers

### 1.1. The Role of the Device Driver

- Most programming problems can indeed be split into two parts:
  - 1. What capabilities are to be provided - **The mechanism**.
  - 2. How those capabilities can be used - **The policy**.

- When writing drivers, a programmer should pay particular attention to this fundamental concept: `Write kernel code to access the hardware, but don't force particular policies on the user, since different users have different needs.`
  - The driver should deal with making the hardware available, leaving all the issues about *HOW* to use the hardware to the applications.

- You can also look at your driver from a different perspective: `It is a software layer that lies between the applications and the actual device.` This privileged role of the driver allows the driver programmer to choose exactly how the device should appear: Different drivers can offer different capabilities, even for the same device.

### 1.2. Splitting the Kernel

- In a Unix system, several concurrent *processes* attend to different tasks. Each process asks for system resources, be it computing power, memory, network connectivity, or some other resource. The *Kernel* is the big chunk of executable code in charge of handling all such requests. The kernel's role can be split into the following parts:
  - 1. **Process management**: The kernel is in charge of creating and destroying processes and handling their connection to the outside world (input and output, IPC). In addition, the scheduler, which controls how processes share the CPU, is part of process management.
  - 2. **Memory management**: The computer's memory is major resource, and the policy used to deal with it is a critical one for system performance. The kernel builds up a virtual addressing space for any and all processes on top of the limited available resources.
  - 3. **File systems**: Unix is heavily based on the filesystem concept; almost everything in UNIX can be treated as a **FILE**. The kernel builds a structured filesystem on top of unstructured hardware, and the resulting file abstraction is heavily used throughout the whole system.
  - 4. **Device control**: Almost every system operation eventually maps to a physical device. With the exception of the processor, memory, and a very few other entities, any and all device control operations are performed by code that is specific to the device being addressed. That code is called a **device driver**.
  - 5. **Networking**: Networking must be managed by the OS, because most network operations are not specific to a process: incoming packets are asynchronous events. The packets must be collected, identified, and dispatched before a process takes care of them. The system is in charged of delivering data packets across program and network interfaces, and it must control the execution of programs according to their network activity. Additionally, all the routing and address resolution issues are implemented within the kernel.

### 1.3. Loadable module

- One of the good features of Linux is the ability to extend at runtime the set of features offered by the kernel.
- Each piece of code that can be added to the kernel at runtime is called a **module**.
- Each module is made up of object code that can be dynamically linked to the running kernel by the `insmod` program and can be unlinked by the `rmmod` program.

### 1.4. Classes of Devices and Modules

- Three fundamental device types:
  - 1. `character module`.
  - 2. `block module`.
  - 3. `network module`.

- 1. **Character devices**:
  - A character (char) device is one that can be accessed as a stream of bytes.
  - A character driver is in charge of implementing this behavior. Such a driver usually implements at least the `open()`, `close()`, `read()` and `write()` system calls.
  - For example: `devconsole` and `devtty`.
- 2. **Block devices**:
  - Like char devices, block devices are accessed by filesystem nodes in the `/dev` directory. A block device is a device (e.g., a disk) that can host a filesystem.
- 3. **Network interfaces**:
  - Any network transaction is made through an interface, that is, a device that is able to exchange data with other hosts.
  - Usually, an *interface* is a hardware device, but it might also be a pure software device, like the `loopback interface`.
  - A network interface is in charge of sending and receiving data packets, driven by the network subsystem of the kernel.
  - Many network connections (especially those using TCP) are stream-oriented, but network devices are, usually, designed around the transmission and receipt of packets. A network driver knows nothing about individual connections; it only handles packets.

- Not being a stream-oriented device, a network interface isn't easily mapped to a node in the filesystem, as `devtty1` is. The Unix way to provide access to interfaces is still by assigning a unique name to them (such as `eth0`), but that name doesn't have a corresponding entry in the filesystem. Instead of `read` and `write`, the kernel calls functions related to packet transmission.

## 2. Building and running modules

### 2.1. Kernel module versus Applications

- While most small and medium-sized applications perform a single task from beginning to end, every *kernel module just registers itself in order to serve future requests*, and its initialization function terminates immediately.
  - In other words, the task of the module's initialization function is to prepare for later invocation of the module's functions; `Here I am, and this is what I can do`.
  - The module's exit function gets invoked just before the module is unloaded. It should tell the kernel, `I'm not there anymore; don't ask me to do anything else`.

- This kind of approach to programming is similar to **event-driven programming**.

- In applications, you can call functions it doesn't define: the linking stage resolves external references using the appropriate library of functions.
- But in kernel, is linked only to the kernel, no libraries to link.
- Most of the relevant headers live in `include/linux` and `include/asm`.

#### 2.1.1. User space and kernel space

- All current processors have at least two protection levels, and some, like the x86 family, have more levels; when several levels exist, the highest and lowest levels are used.

- Unix transfer execution from user space to kernel space whenever an application issues a system call or is suspended by a hardware interrupt.
  - **Kernel code executing a system call is working in the context of process** -- it operates on behalf of the calling process and is able to access data in the process's address space.
  - Code that handles interrupts, on the other hand, is asynchronous with respect to processes and is not related to any particular process.

- **The role of a module is to extend kernel functionality**; modularized code runs in kernel space. Usually a driver performs both the tasks outlined previously: some functions in the module are executed as part of system calls, and some are in charge of interrupt handling.

#### 2.1.2. Concurrency in the kernel

- Even the simplest kernel modules must be written with the idea that many things can be happening at once.

- There are a few sources of concurrency in kernel programming.
  - 1. Naturally, Linux system run multiple processes, more than one of which can be trying to use your driver at the same time.
  - 2. Most devices are capable of interrupting the processor; interrupt handlers run asynchronously and can be invoked at the same time that your driver is trying to do something else.
  - 3. Several software abstractions (such as kernel timer) run asynchronously as well.
  - 4. Moreover, of course, Linux can run on symmetric multiprocessor (SMP) systems, with the result that your driver could be executing concurrently on more than one CPU.
  - 5. Finally, in 2.6, kernel code has been made **preemptible**; that change causes even uni-processor systems to have many of the same concurrency issues as multiprocessor systems.

- As a result, Linux kernel code, including driver code, must be **REENTRANT** -- it must be capable of running in more than one context at the same time.
- Data structures must be carefully designed to keep multiple threads of execution separate, and he code must take care to access shared data in ways that prevent corruption of the data.

- *A common mistake made by driver programmers is to assume that concurrency is not a problem as long as a particular segment of code does not go to sleep*. If you do not write your code with concurrency in mind, it will be subject to catastrophic failures that can be exceedingly difficult to debug.

#### 2.1.3. Current process

- Although kernel modules, don't execute sequentially as applications do, *most actions* performed by the kernel are done on behalf of a specific process.
- Kernel code can refer to the current process by accessing the global item `current`, defined in `asm/current.h`, which yields a pointer to `struct task_struct`.
- For example for x86 (`arch/x86/include/asm/current.h`):

    ```C
    struct task_struct;

    struct pcpu_hot {
        union {
            struct {
                struct task_struct    *current_task;
                int            preempt_count;
                int            cpu_number;
    #ifdef CONFIG_CALL_DEPTH_TRACKING
                u64            call_depth;
    #endif
                unsigned long        top_of_stack;
                void            *hardirq_stack_ptr;
                u16            softirq_pending;
    #ifdef CONFIG_X86_64
                bool            hardirq_stack_inuse;
    #else
                void            *softirq_stack_ptr;
    #endif
            };
            u8    pad[64];
        };
    };
    static_assert(sizeof(struct pcpu_hot) == 64);

    DECLARE_PER_CPU_ALIGNED(struct pcpu_hot, pcpu_hot);

    static __always_inline struct task_struct *get_current(void)
    {
        return this_cpu_read_stable(pcpu_hot.current_task);
    }

    #define current get_current()
    ```

- The `current` pointer refers to the process that is currently executing.
- Actually, `current` is not truly a global variable. The need to support SMP systems forced the kernel developers to develop a mechanism that finds the current process on the relevant CPU.
- This mechanism must also be fast, since references to `current` happen frequently. The result is an architecture-dependent mechanism that, usually, hides a pointer to the `task_struct` structure on the kernel stack.

#### 2.1.4. A Few Other Details

- 1. Application are laid out in virtual memory with a very large stack area. The kernel, instead, has a very small stack; it can be as small as a single, 4096 byte page. Your functions must share that stack with the entire kernel-space call chain. Thus, **It is never a good idea to declare large automatically variables**; If you need larger structures, you should allocate them dynamically at call time.

- 2. Often, as you look at the kernel API, you will encounter function names starting with a double underscore `__`. Functions so marked are generally a low-level component of the interface and should be used with caution.
  - Essentially, the double underscore says to the programmer: `If you call this function, be sure you know what you are doing.`

- 3. Kernel code cannot do floating point arithmetic. Enabling floating point would require that the kernel save and restore the floating point processor's state on each entry to, and exit from, kernel space--at least, on some architectures. `Given that there really is no need for floating point in kernel code`.

### 2.2. Compiling and Loading

- The kernel build system is a complex beast, and we just look at a tiny piece of it.

#### 2.2.1. Loading and Unloading Modules

- `insmod` loads the module code and data into the kernel, which, in turn, performs a function similar to that of `ld`, in that it links any unresolved symbol in the module to the symbol table of the kernel.

- 1. The function `sys_init_module` allocates kernel memory to hold a module (this memory is allocated with `vmalloc`).
- 2. Copies the module text into that memory region.
- 3. Resolves kernel references in the module via the kernel symbol table.
- 4. And calls the module's initialization function to get everything going.

##### 2.2.1.1. `modprobe`

- This utility, like insmod, load a module into the kernel. It differs in that it will look at the module to be loaded to see whether it references any symbols that are not currently defined in the kernel. If any such references are found, `modprobe` looks for other modules in the current module search path hat define the relevant symbols.
- When the `modprobe` finds those module (which are needed by module being loaded), it loads them into the kernel as well.

- If you use `insmod` in this case, `unresolved symbols` message left.

#### 2.2.2. Version dependency

- You can use some macros to to something version dependency:

- 1. `LINUX_VERSION_CODE`: binary representation of the kernel version.
- 2. `UTS_RELEASE`: a string describing the version of this kernel tree.
- 3. `KERNEL_VERSION(major,minor,release)`: an integer version code from individual numbers that build up a version number.

### 2.3. The Kernel Symbol Table

- The table contains the addresses of global kernel items -- functions and variables -- that are needed to implement modularized drivers.
- When a module is loaded, any symbol exported by the module becomes part of the kernel symbol table.

- New modules can use symbols exported by your module, and can *stack* new modules on top of other modules. For example:
  - 1. Each input USB device module stacks on the `usbcore` and `input` modules.

- **Module stacking** is useful in complex project. If a new abstraction is implemented in the form of a device driver, it might offer a plug for hardware specific implementations. For example:

```text
     __________
    |  __lp__  |
     ||      || Port sharing and device registration.
     ||     _\/______________
     ||    |_  _parport___  _|
     ||      ||           || Low-level device operations.
     ||      ||          _\/____________
     ||      ||         |__partport_pc__|
     ||      ||                ||
     \/______\/________________\/_________
    |           Kernel API                |
    |(Message print, driver registration, |
    |port allocation, etc.)               |
    |_____________________________________|
```

- When using stacked modules, it is helpful to be aware of the `modprobe` utility.
- Using stacking to split modules into multiple layers can help reduce development time by simplifying each layer.

- If your module needs to export symbols for other modules to use, the following macros should be used:

    ```C
    EXPORT_SYMBOL(name);
    EXPORT_SYMBOL_GPL(name);
    ```

### 2.3. Initialization and shutdown

- Initialization functions should be declared `static`, since they are not meant to be visible outside the specific file.
- `init` token is a hint to the kernel that the given function is used only at initialization time. The module loader drops the initialization function after the module is loaded, making its memory available for other uses.

#### 2.3.1. The cleanup function

- The `exit` modifier marks the code as being for module unload only (by causing the compiler to place it in a special ELF section). If your module is built directly into the kernel, or if your kernel is configured to disallow the unloading of modules, functions marked `exit` are simply discarded.

#### 2.3.2. Error Handling During Initialization

- One thing you must always bear in mind when registering facilities with the kernel is that the registration could fail. Even the simplest action often requires memory allocation, and the required memory may not be available. So **module code must always check return values**, and be sure that the requested operations have actually succeeded.

```C
int init my_init_function(void)
{
    int err;
    /* registration takes a pointer and a name */
    err = register_this(ptr1, "skull");
    if (err) goto fail_this;
    err = register_that(ptr2, "skull");
    if (err) goto fail_that;
    err = register_those(ptr3, "skull");
    if (err) goto fail_those;

    return 0; /* success */
    fail_those: unregister_that(ptr2, "skull");
    fail_that: unregister_this(ptr1, "skull");
    fail_this: return err; /* propagate the error */
}
``

- Or your initialization is more complex, you can define the clean function like this:

```C
struct something *item1;
struct somethingelse *item2;
int stuff_ok;

void my_cleanup(void)
{
    if (item1)
        release_thing(item1);
    if (item2)
        release_thing2(item2);
    if (stuff_ok)
        unregister_stuff( );
    return;
}

int init my_init(void)
{
    int err = -ENOMEM;
    item1 = allocate_thing(arguments);
    item2 = allocate_thing2(arguments2);
    if (!item2 || !item2)
        goto fail;
    err = register_stuff(item1, item2);
    if (!err)
        stuff_ok = 1;
    else
        goto fail;
    return 0; /* success */
    fail:
        my_cleanup( );
        return err;
}
```

### 2.4. Doing it in User space

- There are some arguments in favor of user-space programming, and sometimes writing a so-called `user-space device driver` is a wise alternative to kernel hacking.

- The advantages of user-space device driver:
  - 1. Full C library can be linked in.
  - 2. Conventional debugger on the driver code.
  - 3. If a user-space driver hangs, you can simply kill it.
  - 4. User memory is swappable, unlike kernel memory.
  - 5. A well-designed driver program can still, like kernel-space drivers, allow concurrent access to a device.

- For example, USB drivers can be written for user space; see the `libusb` project.

- But the user-space approach to device driving has a number of drawbacks. Some important are:
  - 1. Interrupts are not available in user space. There are workarounds for this limitation of some platforms.
  - 2. Direct access to memory is possible only by `mmap` `devmem`, and only a privileged user can do.
  - 3. Access to I/O ports is available only after calling `ioperm` or `iopl`.
  - 4. Response time is slower, because a context switch is required to transfer information or actions between client and the hardware.
  - 5. If the driver has been swapped to disk, response time is unacceptable long.
  - 6. Some important devices can't be handled in user space: network interfaces and block devices.

## 3. Char Drivers

- `scull`: Simple Character Utility for Loading Localities. `scull` is a char driver that acts on a memory area as though it were a device.
  - The advantage of scull is that it isn't hardware dependent. `scull` jut acts on some memory, allocated from the kernel.

### 3.1. The design of `scull`

- The first step of driver writing is defining the capabilities (the mechanism) the driver will offer to user programs.
- To make `scull` useful as a template for writing real drivers for real devices, we'll show you how to implement several device abstractions on top of the computer memory, each with a different personality.

- The `scull` source implements the following devices. Each kind of device implemented by the module is referred to as a *type*.
  - `scull0` to `scull3`:
    - Four devices, each consisting of a memory area that is both global and persistent.
      - **Global** means if device is opened multiple times, the data contained within the device is shared by all the file descriptors.
      - **Persistent** means that if the device is closed and reopened, data isn't lost.
    - This device can be fun to work with, because it can be accessed and tested using conventional commands: `cp`, `cat`, shell IO redirection.
  - `scullpipe0` to `scullpipe3`:
    - Four FIFO devices, which act like pipes. One process reads and another writes.
  - `scullsingle`
  - `scullpriv`
  - `sculluid`
  - `scullwuid`

- Each of the `scull` devices demonstrates different features of a driver and presents different difficulties.

### 3.2. Major and Minor

- Char devices are accessed through name in the filesystem. Those names are called special files or device files or simply nodes of the file system tree; they are conventionally located in the `/dev` directory.

- For example `ls -l`:

```text
crw-rw-rw-1 root root 1, 3 Apr 11 2002 null
crw------- 1 root root 10, 1 Apr 11 2002 psaux
crw------- 1 root root 4, 1 Oct 28 03:04 tty1
crw-rw-rw-1 root tty 4, 64 Apr 11 2002 ttys0
crw-rw---- 1 root uucp 4, 65 Apr 11 2002 ttyS1
crw--w---- 1 vcsa tty 7, 1 Apr 11 2002 vcs1
crw--w---- 1 vcsa tty 7, 129 Apr 11 2002 vcsa1
crw-rw-rw-1 root root 1, 5 Apr 11 2002 zero
```

- `c` means character device file. `b` block device file.
- You can see 1, 10, 4, 7 are **MAJOR** numbers and 3, 1, 64, 65, 129, 5 are **MINOR** numbers.

- Traditionally, the major number identifies the driver associated with the device.
  - For example, `devnull` and `devzero` are both managed by driver 1, whereas `virtual console` and `serial terminals` are managed by driver 4;

- The minor number is used by the kernel to determine exactly which device is being referred to. Kernel itself knows almost nothing about minor numbers beyond the fact that they refer to devices implemented by your driver.

#### 3.2.1. The Internal Representation of Device Numbers

- Within the kernel, the `dev_t` type is used to hold device numbers -- both the major and minor parts.
- To obtain the major or minor parts of a `dev_t`, use:

```C
MAJOR(dev_t dev);
MINOR(dev_t dev);
```

- Make `dev_t` struct from numbers:

```C
MKDEV(int major, int minor);
```

#### 3.2.2. Allocating and Freeing Device Numbers

- One of the first things your driver will need to do when setting up a char device is to obtain one or more device numbers to work with. The necessary function:

    ```C
    int register_chrdev_region(dev_t first, unsigned int count, char *name);
    ```

  - `first` is the beginning device number of the range you would like to allocate.
  - `count` is the total number of contiguous device numbers you are requesting.
  - `name` name of the device.

- The `register_chrdev_region` works well if you know ahead of time exactly which device numbers you want.

- Often, however, you will not know which major numbers your device will use; there is a constant effort within the Kernel community to move over to the use of dynamically-allocated device numbers. The kernel will happily allocate a major number for you on the fly, but you must request this allocation by using a different function:

    ```C
    int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, unsigned int count, char *name);
    ```

  - `dev` is output parameter that will, on successful completion, hold the first number in your allocated range.

- You should free them when they are no longer in use:

```C
void unregister_chrdev_region(dev_t first, unsigned int count);
```

### 3.2.3. Dynamic Allocation of Major Numbers

- Some major device numbers are statically assigned to the most common devices. A list of those devices can be found in `Documentation/devices.txt`.

- So as a driver writer, you have a choice: you can simply pick a number that appears to unused, you can allocate major numbers in a dynamic manner.

- We recommend use dynamic allocation to obtain your major device number. In other words, your driver should almost certainly be using `alloc_chrdev_region` rather than `register_chrdev_region`.

- Read `procdevices` file to see major numbers of current drivers:

```text
Character devices:
    1 mem
    2 pty
    3 ttyp
    4 ttyS
    6 lp
    7 vcs
    10 misc
    13 input
    14 sound
    21 sg
    180 usb
Block devices:
    2 fd
    8 sd
    11 sr
    65 sd
    66 sd
```

- Script to create 4 scull devices [scull_load](https://github.com/starpos/scull/blob/master/scull/scull_load):

```bash
#!binsh
module="scull"
device="scull"
mode="664"

# invoke insmod with all arguments we got
# and use a pathname, as newer modutils don't look in . by default
sbininsmod ./$module.ko $* || exit 1

# remove stale nodes
rm -f dev${device}[0-3]
major=$(awk "\\$2= =\"$module\" {print \\$1}" procdevices)

mknod dev${device}0 c $major 0
mknod dev${device}1 c $major 1
mknod dev${device}2 c $major 2
mknod dev${device}3 c $major 3

# give appropriate group/permissions, and change the group.
# Not all distributions have staff, some have "wheel" instead.
group="staff"
grep -q '^staff:' etcgroup || group="wheel"

chgrp $group dev${device}[0-3]
chmod $mode dev${device}[0-3]
```

- `mknod` was originally used to create the character and block devices that populate `/dev/`.
- The script can be adapted for another driver by redefining the variables and adjusting the `mknod` lines.

- A [scull_unload](https://github.com/starpos/scull/blob/master/scull/scull_unload) script is also available to clean up the `/dev` directory and remove module.

- The best way to assign major numbers, is by defaulting to dynamic allocation while leave yourself the option of specifying the major number at load time, or even compile time.
  - The `scull` work in this way. For example, if you init scull_major with `0` we use the dynamic allocation:

    ```C
    if (scull_major) {
        dev = MKDEV(scull_major, scull_minor);
        result = register_chrdev_region(dev, scull_nr_devs, "scull");
    } else {
        result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
        "scull");
        scull_major = MAJOR(dev);
    }
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    }
    ```

### 3.2.4. Some Important Data structures

- Most if the fundamental driver operations involve three important kernel data structures:
  - 1. `file_operations`.
  - 2. `file`.
  - 3. `inode`.

#### 3.2.4.1. File Operations

- The `file_operations` structure is how a char driver sets up connection between device numbers and our driver's operations. The structure is a collection of function pointers.

- We can consider the file to be an `object` and the functions operating on it to be its `methods`, using OOP term.
  - This is the first sign of OOP we see in the kernel.

- Now we explains the role of the most important operations and offers hints, caveats, and read code examples.
  - NOTE: `user` keyword indicate the pointer is user space pointer.

  - 1. `struct module *owner`: A pointer to the module that owns the structure. It's used to prevent the module being unloaded while its operations are in use.
  - 2. `loff_t (*llseek)(struct file *, loff_t, int);`: The `llseek` method is used to change the current read/write position in a file and the new position is returned as a return value (positive).
  - The `loff_t` parameter is a `long offset`.
  - 3. `ssize_t (*read) (struct file, char user, size_t, loff_t *);` used to retrieve data from the device.
  - 4. `ssize_t (*aio_read) (struct kiocb , char user , size_t, loff_t);` Initiates an asynchronous read -- a read operation that might not complete before the function returns.
  - 5. `ssize_t (*write)(struct file, const char user, size_t, loff_t *);` Sends data to the device.
  - 6. `ssize_t (*aio_write)(struct kiocb , const char user , size_t,loff_t *);` Initiates and asynchronous write operation on the device.
  - 7. `unsigned int (*readdir)(struct file, void, filldir_t);` This field should be NULL for device files; it is used for reading directories and is useful only for file systems.
  - 8. `unsigned int (*poll)(struct file, struct poll_table_struct);` The `poll` method is the backend of three system calls: `poll`, `epoll`, and `select`, all of which are used to query whether a read or write to one or more file descriptors would block. The `poll` method should return a bit mask indicating whether non-blocking reads or writes are possible, and, possibly, provide the kernel with information that can be used to put the calling process to sleep until I/O becomes possible.
  - 9. `int (*ioctl)(struct inode, struct file, unsigned int, unsigned long));` The `ioctl` system call offers a way to issue device-specific commands. A few commands are recognized by the kernel without referring to the `file_operations` table.
  - 10. `int (*mmap)(struct file, struct vm_area_struct);` The `mmap` is used ti request a mapping of device memory to a process's address space.
  - 11. `int (*open)(struct inode, struct file);` though this is always the first operation performed on the device file, the driver is not required to declare a corresponding method.
  - 12. `int (*flush)(struct file*);` the flush operation is invoked when a process closes its copy of a file descriptor for a device. Don't confuse with `fsync`.
  - 13. `int (*release)(struct inode, struct file);` invoked when the file structure is being released.
  - 14. `int (*fsync)(struct file, struct dentry, int);` this method is the backend of the `fsync` system call, which a user calls to flush any pending data.
  - 15. `int (*fasync)(int, struct file *, int);` this operation is used to notify the device of a change in its `FASYNC` flag.
  - 16. `int (*lock)(struct file, int, struct file_lock);` The `lock` method is used to implement file locking.
  - 17. `ssize_t (*readv) (struct file , const struct iovec , unsigned long, loff_t *);`
  - 18. `ssize_t (*writev) (struct file , const struct iovec , unsigned long, loff_t *);` These methods implement scatter/gather read and write operations. Applications occasionally need to do a single read or write operation involving multiple memory areas; these system calls allow them to do so without forcing extra copy operations on the data.
  - 19. `ssize_t (*sendfile)(struct file, loff_t, size_t, read_actor_t, void *);` this method implements the read side of the `sendfile` system call, which moves the data from one file descriptor to another with a minimum of copying. It is used, for example, by a web server that needs to send the contents of a file out a network connection.
  - 20. `ssize_t (*sendpage)(struct file, struct page, int, size_t, loff_t *, int);` The `sendpage` is the other half of `sendfile`; it is called by the kernel to send data, one page at a time, to the corresponding file.

#### 3.2.4.2. The File Structure

- `struct file`, is the second most important data structure used in device drivers. NOTE that a `file`, has nothing to do with the `FILE` pointers of userspace programs. A `FILE` is defined in the C library and never appears in kernel code. A `struct file`, on the other hand, is a kernel structure that never appears in user programs.

- The `file` structure represents an `open file`. (It is not specific to device drivers; every open file in the system has an associated `struct file` in kernel space). It is created by the kernel on `open` and is passed to any function that operates on the file, until the last `close`. After all instances of the file are closed, the kernel release the data structure.

- The most important fields of `struct file`:
  - 1. `mode_t f_mode;` The file mode identifies the file as either readable or writable (or both), by means of the bits `FMODE_READ` and `FMODE_WRITE`. You might want to check this field for read/write permission in your `open` or `ioctl` function, but you don't need to check permissions for `read` and `write`, because the kernel checks before invoking your method.
  - 2. `loff_t f_pos;` The current reading or writing position. The driver can read this value if it needs to know the current position in the file but should not normally change it; It should change by read(), write(), llseek().
  - 3. `unsigned int f_flags;` these are the file flags, such as `O_RDONLY`, `O_NONBLOCK`, `O_SYNC`. A driver should check the `O_NONBLOCK` flag to see if nonblocking operation has been requested; The other flags are seldom used.
  - 4. `struct file_operations *f_op;` The operations associated with the file. The kernel assigns the pointer as part of its implementation of `open` and then reads it when it needs to dispatch any operations.
    - That means that we can change the file operations associated with your file, and the new methods will be effective after you return to the caller.
    - The ability to replace the file operations is the kernel quivalent of **method overriding** in OOP.
  - 5. `void *private_data;` you can free to make its own use of the field or to ignore it; you can use the field to point to allocated data, but then you must remember to free that memory in the release method before the file structure is destroyed by the kernel.
  - 6. `struct dentry *f_dentry;`: The directory entry structure associated with the file. Device driver normally need not concern with this structure, other than to access the `inode` as `filp->f_dentry->d_inode;`

#### 3.2.4.3. The inode structure

- The `inode` structure is used by the kernel internally to represent files. Therefore it is different from the `file` structure that represents an open file descriptor.
- There can be numerous `file` structures representing multiple open descriptors on a single file, but they all point to a single `inode` structure.

- The `inode` structure contains a great deal of information about the file. As a genral rule, only two fields of this structure are of interest for writing driver code:
  - `dev_t i_rdev;` For inodes that represent device files, this field contains the actual device number.
  - `struct cdev *i_cdev;` struct cdev is the kernel's internal structure that represents char devices; this field contains a pointer to that structure when the inode refers to a char device file.

### 3.2.5. Char Device Registration

- The kernel use structures of type `struct cdev` to represent char devices internally. Before the kernel invokes your device's operations, you must allocate and register one or more of these structures.
  - Almost helper functions are defined at `<linux/cdev.h>`.

- There are **two ways** of allocating and initializing one of these structures. If you wish to obtain a standalone `cdev` structure at runtime, you may do so with code such as:

```C
struct cdev *my_cdev = cdev_alloc();
my_cdev->ops = &my_ops;
```

- Chances are, howerver, that you will want to embed the `cdev` structure within a device-specific structure of your own; In thay case, you should initialize the structure that you have already allocated with:

```C
void cdev_init(struct cdev cdev, struct file_operations fops);
```

- Final step is to tell kernel about it with a call to:

```C
int cdev_add(struct cdev *dev, dev_t num, unsigned int count);
```

- Here, `dev` is the `cdev` structure, `num` is the first device number to which this device responds, and `count` is the number of device number that be associated with the device. Offten `count` is 1.

#### 3.2.5.1. The Older way

- The classic way to register a char device driver is with:

```C
int register_chrdev(unsigned int major, const char *name, struct file_operations *ops);
```

- A call to `register_chardev()` registers minor numbers 0-255 for the given `major`, and set up a default `cdev` structure for each. Drivers using this interface must be prepared to handle `open` calls on all 256 minor numbers (whether they correspond to real devices or not).

## 17. Network drivers

- Network interfaces are the third standard class of Linux devices.
- The role of a network interface within the system is similar to that of a mounted block device. A block device regiters its disks and methods with the kernel, and then `transmits` and `receives` blocks on request, by means of its `request` function. Similarly, a network interface must register itself within specific kernel data structures in order to be invoked when packets are exchanged with the outside world.

- There are a few important differences between mounted disks and packet-delivery interfaces.
  - 1. To begin with, a disk exists as a special file in the `/dev` directory, whereas a network interface has no such entry point.
  - 2. The normal file operations (read, write, and so on) do not make sense when applied to network interfaces, so it is **NOT POSSIBLE** to apply the UNIX **everything is a file** approach to them.
    - Thus, network interfaces exist in their own namespace and export a different set of operation.
    - Although you may object that applications use the `read` and `write` system calls when using sockets, those calls act on a software object that is distinct from the interface. Several hundred sockets can be multiplexed on the same physical interface.
  - 3. The most important difference is that block drivers operate only in response to requests from the kernel, whereas network drivers receive packets **ASYNCHRONOUSLY** from outside.

- The network subsystem of the Linux Kernel is designed to be **completely protocol-independent**. This applies for both networking protocols (IP, etc.) and hardware protocols (Ethernet, etc.).
  - Interaction between a network driver and the kernel properly **deals with one network packet at a time**; this allows **protocol issues to be hidden neatly from the driver** and **the physical transmission to be hidden from the protocol**.

- NOTE: The term `octet` means a group of eight bits, which is generally the smallest unit understood by networking devices and protocols. The term `byte` is almost never encountered in this context.
- NOTE: The term `header` is a set of bytes prepended to a packet as it is passed through  the various layers of the networking subsystem.

- When a application sends a block of data through a TCP socket:
  - 1. The network subsystem breaks that data up into packets and puts a TCP header, describing where each packets fits within the stream, at the beginning.
  - 2. The lower levels then put an IP headers, used to route the packet to its destination, in front of the TCP heaer.
  - 3. If the packets moves over an Ethernet-like medium, an Ethernet header, interpreted by the hardware, goes in front of the rest.
- Network drivers need **not concern themselves with higher-level headers**, but they often **must be involved in the creation of the hardware-level header**.

### 17.1. How `snull` is designed

- `snull` network interface.
- The sample interfaces should remain indpendent of real hardware.
- This constraint led to something that resembles the loopback interface. **`snull` is not a loopback interface**; however, it simulates conversations with **real remote hosts** in order to better demonstrate the task of writing a network driver.
- The Linux loopback driver is actually quite simple; it can be found in `drivers/net/loopback.c`

- Another feature of `snull` is that it supports only IP traffic. This is a consequence of the internal workings of the interface--`snull` has to look inside and interpret the packets to properly emulate a pair of hardware interfaces.
- **Real interfaces don't depend on the protocol being transmitted**.

#### 17.1.1. Assigning IP numbers

- The `snull` module creates two interfaces. These interfaces are different from a simple loopback, in that whatever you transmit through one of the interfaces loops back to the other one, not to itself. It looks like you have two external links, but actually your computer is replying to itself.

### 17.1.2. The physical Transport of packets

- Snull emulates Ethernet because:
  - 1. the vast majority of existing networks, **at least the segments that a workstation connects to** are based on Ethernet technology, be it 10base-T, 100base-T or Gigabit.
  - 2. The kernel offers some generalized support for Ethernet devices, and there's no reason not use it.
  - 3. The advantage of being an Ethernet device is so strong that even the `plip` interface (the interface that uses the printer ports) declares itself as an Ethernet device.
  - 4. The last advantage of using the Ethernet setup for snull is that you can run `tcpdump` on the interface to see the packets go by.

### 17.2. Connecting to the Kernel

- We start looking at the structure of network drivers by dissecting the the `snull` source. Keeping the source code for several drivers handy might help you follow the discussion and to see how real-world Linux drivers operate.

- As a place to start, we suggest `loopback.c` -> `plip.c` -> `e100.c` in order of increasing complexity.
  - All these files live in `drivers/net`, within the kernel source tree.

#### 17.2.1. Device Registration

- When a driver module is loaded into a running kernel, it requests resources and offers facilities;
- The driver should probe for its device and its hardware location (I/O ports and IRQ line) -- but not register them.
- The way a network driver is registered by its module initialization function is different from char and block drivers. Since there is no equivalent of major and minor numers for network interfaces, a network driver does not request such a number. **Instead, the driver inserts a data structure for each newly detected interface into a global list of network devices**.

- Each interface is describe by a `struct net_device` item, which is defined in `linux/netdevice.h`.
- The `net_device` structure, like many other kernel structures, contains a `kobject` and is, therefore, reference-counted and exported via `sysfs`. As with other such structures, it must be allocated dynamically. The kernel function provided to perform this allocation is `alloc_netdev`, which has the following prototype:

```C
struct net_device *alloc_netdev(int sizeof_priv,
                                const char *name,
                                void (*setup)(struct net_device *));
```

- The `sizeof_priv` private data will be allocated along with the `net_device` structure.
- The `name` can have a `printf-style %d`. The kernel replaces the `%d` with the next available interface number.
- Finally, `setup` is a pointer to an initialization function that is called to set up the rest of the `net_device` structure.

- The networking subsytem provides a number of helper functions wrapped around `alloc_netdev()` for various types of interfaces. The most common is `alloc_etherdev()`, which is defined in `<linux/etherdev.h>`:

    ```C
    struct net_device *alloc_etherdev(int sizeof_priv);
    ```

  - This function allocates a network device using `eth%d` for the name argument. It provides its own initialization function (`ether_setup()`) that sets several `net_device` fields with appropriate values for Ethernet devices. The driver should simply do its required initialization directly after a successful allocation.
- We have similar wrappers like `alloc_fcdev()`, `alloc_fddidev()`, `alloc_trdev()`.

- Once the `net_device` structure has been initialized, completing the process is just a matter of passing the structure to `register_netdev`. For example:

```C
struct net_device* dev = alloc_netdev(0, "lava_loopback", &setup);

/* Configure more for your driver here. */

/* Register it to the kernel, add to the netdevice list. */
register_netdev(dev);
```

#### 17.2.2. Initializing Each Device

- We have looked at the allocation and registration of `net_device` structures, but we passed over the intermediate step of completely initializing that structure.
- Note that `struct net_device` is always put together at runtime; it cannot be set up at compile time in the same manner as a `file_operations` or `block_device_operations` structure.
- This initialization must be complete before calling `register_netdev()`. The `net_device` structure is large and complicated; fortunately, the kernel takes care of some Ethernet-wide defaults through the `ether_setup()` function.

#### 17.2.3. Module unloading

- Nothing special happens when the module is unloaded. The module cleanup function simply unregisters the interfaces, performs whatever internal cleanup is required, and releases the `net_device` structure back to the system:

```C
void snull_cleanup(void)
{
  int i;
  for (i = 0; i < 2; i++) {
    if (snull_devs[i]) {
      unregister_netdev(snull_devs[i]);
      snull_teardown_pool(snull_devs[i]);
      free_netdev(snull_devs[i]);
    }
  }
  return;
}
```

### 17.3. The `net_device` Structure in Detail

- The `net_device` structure is at the very core of the network driver layer and deserves a complete description.

#### 17.3.1. Global information

- 1. `char name[IFNAMESIZ];`: The name of the device.
- 2. `unsigned long state;`: Device State. This field includes several flags. Drivers do not normally manipulate these flags directly.
- 3. `struct net_device *next;`: Pointer to the next device in the global linked list. This field shouldn't be touched by the driver.
- 4. `int (*init)(struct net_device *dev);`: An initialization function. If this pointer is set, the function is called by `register_netdev()` to complete the initialization of the `net_device` structure.

#### 17.3.2. Hardware information

- These fields contain low-level hardware information fo relatively simple devices. Most modern drivers do make use of them.

- 5. `unsigned long rmem_end;`
- 6. `unsigned long rmem_start;`
- 7. `unsigned long mem_end;`
- 8. `unsigned long mem_start;`
  - Device memory information. These fields hold the begining and ending addresses of the shared memory used by the device. If the device has different receive and transmit memories, the `mem` fields are used for transmit memory and the `rmem` are used for receive memory.
- 9. `unsigned long base_addr;`
  - The I/O base address of the network interface. Should be assigned during the device probe.
  - `ifconfig` command can display or modify this config.
- 10. `unsigned char irq;`: The assigned interrupt number.
  - `ifconfig` can display this.
- 11. `unsigned char if_port;`: The port in use on multiport.
- 12. `unsigned char dma;`: The DMA channel allocated by the device.

#### 17.3.3. Interface Information

- Most of the information about the interface is correctly setup by the `ether_setup()` function. Ethernet cards can rely on this general-purpose function for most of these fields, but the `flags` and `dev_addr` fields are device specific and must be explicitly assigned at initialization time.

- Some non-Ethernet interfaces can use helper functions similar to `ether_setup()`. `drivers/net/net_init.c` exports a number of such functions, including the following:
  - `void ltalk_setup(struct net_device *dev);`: Sets up the fields for a LocalTalk device.
  - `void fc_setup(struct net_device *dev)`: Initializes fields for fiber-channel devices
  - Etc.

- Most devices are covered by one of these classes. If yours is something radically new and different, however, you need to assign the following fields by hand:

- 13. `unsigned short hard_header_len;`: The hardware header length, that is, the number of octets that lead the transmitted packet before the IP header, or other protocol information.
- 14. `unsigned mtu;`: The maximum transfer unit (MTU). This field is used by the network layer to drive packet transmission. Ethernet has an MTU of 1500 octets (ETH_DATA_LEN).
- 15. `unsigned long tx_queue_len;`: The maximum number of frames that can be queued on the device's transmission queue.
- 16. `unsigned short type;`: The hardware type of the interface. ARP use this to determine what kind of hardware address the interface supports.
- 17. `unsigned char addr_len;`
- 18. `unsigned char broadcast[MAX_ADDR_LEN];`
- 19. `unsigned char dev_addr[MAX_ADDR_LEN];`
  - Hardware (MAC) address length and device hardware addresses. The Ethernet address length is six octets.
- 20. `unsigned short flags;`
- 21. `int features;`
  - Interface flags:
    - `IFF_UP`: read-only, kernel turns it on when the interface is active and ready to transfer packets.
    - `IFF_BROADCAST`: The interface allows broadcasting.
    - `IFF_LOOPBACK`: This flag should be set only in the loopback interface.

#### 17.3.4. The device method

- Each network device declares the function that act on it.
- Device methods for a network interface can be divided into two groups: `fundamental` and `optional`.

- `Fundamental` methods include those that are needed to be able to use the interface:

- 22. `int (*open)(struct net_device *dev);`: Opens the interface. The interface is opened whenever `ifconfig` activates it. The `open` method should register any system resource it needs (I/O ports, IRQ, DMA, etc.) turn on the hardware, and perform any other setup your device requires.
- 23. `int (*stop)(struct net_device *dev);`: Stops the interface. This should reverse operations that are performed by `open()`.
- 24. `int (*hard_start_xmit)(struct sk_buff *skb, struct net_device *dev);`: Method that initiates the transmission of a packet. The full packet(protocol headers and all) is contained in a socket buffer (`sk_buff`) structure.
- 25. `int (*hard_header)(struct sk_buff *skb, struct net_device *dev, unsigned short type, void *daddr, void *saddr, unsigned len);`: is called before `hard_start_xmit()` that builds the hardware header from the source and destination hardware addresses that were previously retrieved.
- 26. `void (*tx_timeout)(struct net_device *dev);` Method called by the networking code when a packet transmission fails to complete within a reasonable period. It should handle the problem and resume packet transmission.
- Etc.

- Optional:
- 27. `int weight;`
- 28. `int (*poll)(struct net_device *dev, int quota);`
  - Operate the interface in a polled mode, with interrupts disabled.
- 29. `int (*do_ioctl)(struct net_device dev, struct ifreq ifr, int cmd);`
  - performs interface-specific `ioctl` command.
- 30. `int (*set_mac_address)(struct net_device *dev, void *addr);`
  - Can be implemented if the interface supports the ability to change its hardware address.

### 17.4. Opening and Closing

- The kernel opens and closes an interface in response to the `ifconfig` command.
- When `ifconfig` is used to assign an address to the interface, it performs two tasks. First, it assigns the address by means of `ioctl(SIOCSIFADDR)`(Socket I/O Control Set Interface Address). Then it sets the `iFF_UP` bit in `dev->flag` by means of `ioctl(SIOCSIFFLAGS)` (Socket I/O Control Set Interface Flags) to turn the interface on.

- `open` requests any system resources it needs and tells the interface to come up; `stop` shuts down the interface and release system resources. Some drivers must perform some addiontional steps at `open` time.

- First, the hardware (MAC) address needs to be copied from the hardware device to `dev->dev_addr` before the interface can communicate with the outside world.
  - The hardware address can then be copied to the device at open time.

- The `open` method should also start the interface's transmit queue (allowing accepting packets for transmission) once it is ready to start sending data. The kernel provides a function to start the queue:

```C
void netif_start_queue(struct net_device *dev);
```

- So our `open()` look like:

```C
int snull_open(struct net_device *dev)
{
  /* request_region( ), request_irq( ), .... (like fops->open) */

  /*
  * Assign the hardware address of the board: use "\0SNULx", where
  * x is 0 or 1. The first byte is '\0' to avoid being a multicast
  * address (the first byte of multicast addrs is odd).
  */
  memcpy(dev->dev_addr, "\0SNUL0", ETH_ALEN);
  
  if (dev == snull_devs[1])
    dev->dev_addr[ETH_ALEN-1]++; /* \0SNUL1 */

  netif_start_queue(dev);

  return 0;
}

```

- And the close:

```C
int snull_release(struct net_device *dev)
{
  /* release ports, irq and such -- like fops->close */

  netif_stop_queue(dev); /* can't transmit any more */
  return 0;
  }

```
