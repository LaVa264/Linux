# Linux Programming Interface

## 2. Fundamental concepts

### 2.1. The Core OS: Kernel

- The term OS is commonly used with two different meanings:
  - 1. To denote the entire package consisting of the central software managing a computer's resources and all of the accompanying standard software tools, such as command-line interpreter, graphical user interfaces, file utilities, and editors.
  - 2. More narrowly, to refer to the central software that manages and allocates computer resources (i.e. the CPU, RAM, and devices).

- The term `kernel` is often used as a synonym for the second meaning, and it is with this meaning of the term `OS`.

- Although it is possible to run programs on a computer without a kernel, the presence of a kernel greatly simplifies the writing and use of other programs, and increases the power and flexibility available to programmers. The kernel does this by providing a software layer to manage the limited resources of a computer.

#### 2.1.1. Tasks performed by the kernel

- 1. **Process scheduling**: A computer has one or more central processing units (CPUs), which execute the instructions of programs. Like other UNIX systems, Linux is a **Preemptive Multitask** OS, that means multi-processes can simultaneously reside in memory and each may receive use of the CPU(s). **Preemptive** means that the rules governing which processes receive use of the CPU and how long are determined by the kernel process scheduler.

- 2. **Memory Management**: While computer memories are enormous by the standards of a decade or two ago, the size of software has also correspondingly grown, so that physical memory (RAM) remains a limited resource that the kernel must share among processes in an equitable and efficient fashion.
  - Like most modern OSes, Linux employs virtual memory management, a technique that confers two main advantages:
    - 1. Processes are isolated from one another and from the kernel, so that one process can't read and modify the memory of another process or the kernel.
    - 2. Only part of a process needs to be kept in memory, thereby lowering the memory requirements of each process and allowing more processes to be held in RAM simultaneously. This leads to better CPU utilization, since it increases the likelihood that, at any moment in time, there is at least one process that the CPU(s) can execute.
- 3. **Provision of a file system**: The kernel provides a file system on disk, allowing files to be created, retrieved, updated, deleted, and so on.

- 4. **Creation and termination of processes**: The kernel can load a new program into memory, providing it with the resources (e.g., CPU, memory, and access to files) that it needs in order to run.
  - Such an instance of a running program is termed a `process`.
  - Once a process has completed execution, the kernel ensures that the resources it uses are freed for subsequent reuse by later programs.

- 5. **Access to devices**: The devices (mice, monitors, keyboards, disk and tape drives, and so on) attached to a computer allow communication of information between the computer and the outside world. permitting input, output, or both. The kernel provides programs with an interface that standardizes and simplifies access to devices, while at the same time arbitrating access by multiple processes to each device.

- 6. **Networking**: The kernel transmits and receives network messages (packets) on behalf of user processes. This task including routing of network packets to the target system.

- 7. **Provision of a system call application programming interface (API)**: Processes can request the kernel to perform various tasks using kernel entry points known as **system calls**.

- In addition to the above features, multi-user OSes such as Linux provide users with the abstraction of a *virtual private computer;* that is each user can log on to the system and operate largely independently of other users. For example, each user has their own disk storage space (home directory). In addition, users can run programs, each of which gets a share of the CPU and operates in its own virtual address space, and these programs can independently access devices and transfer information over the network. The kernel resolves potential conflicts in accessing hardware resources, so users and processes are generally unaware of the conflicts.

#### 2.1.2. Kernel mode and User mode

- Modern processor architectures typically allow the CPU to operate in at least two different modes: `user mode` and `kernel mode`.
  - **Hardware instructions allow switching from one mode to the other.**
- Correspondingly, areas of virtual memory can be marked as being part of `user space` and `kernel space`. When running in user mode, the CPU can access only memory that is marked as being in user-space; attempts to access memory in kernel space result in a hardware exception. When running in kernel mode, the CPU can access both user and kernel memory space.

- Certain operations can be performed only while the processor is operating in kernel mode. Examples include:
  - Executing the `halt` instruction to stop the system,
  - accessing the memory-management hardware,
  - and initiating device I/O operations.
- By takin advantage of this hardware design to place the operating system in kernel space, operating system implementers can ensure that user processes are not able to access the instructions and data structures of the kernel, or to perform operations that would adversely affect the operation of the system.

#### 2.1.3. Process versus kernel views of the system

- A running system typically has numerous processes. For a process, many things happen asynchronously.

- 1. An executing process doesn't know when it will next timeout, which other processes will then be scheduled for the CPU, or when it will next be scheduled.
- 2. The delivery of signals and the occurrence of inter-process communication events are mediated by the kernel, and can occur at any time for a process. Many things happen transparent for a process.
- 3. A process doesn't know where it is located in RAM or, in general whether a particular part of its memory space is currently resident in memory or held in swap area (a reserved area of disk space ued to supplement the computer's RAM).
- 4. Similarly, a process doesn't know where on the disk drive the files it accesses are being held; it simply refers to the files by name.
- 5. A process operates in isolation; it can't directly communicate with another process.
- 6. A process can't itself create a new process or even end its own existence.
- 7. Finally, a process can't communicate directly with the input and output devices attached to the computer.

- By contrast, a running system has one kernel that knows and controls everything. The kernel facilitates the running of all processes on the system.
- 1. The kernel decides which process will next obtain access to the CPU, when it will do so, and for how long.
- 2. The kernel maintains data structures containing information about all running processes and updates these structures as processes are created, change state and terminate.
- 3. The kernel maintains all of the low-level data structures that enable the filenames used by programs to be translated into physical locations on the disk.
- 4. The kernel also maintains data structures that map the virtual memory of each process into the physical memory of the computer and swap areas on disk.
- 5. All communication between processes is done via mechanisms provided by the kernel.
- 6. In response to requests from processes, the kernel creates new processes and terminates existing processes.
- 7. Lastly, the kernel (device drivers) performs all direct communication with input and output devices, transferring information to and from user processes as required.

- When we say things such as `a process can create another process`, `a process can create a pipe`, `a process can write data to a file`, and `a process can terminate by calling exit()`. Remember, however, that the kernel mediates all such actions, and these statements are just shorthand for **A process can `request that the kernel` create another process**, and so on.

### 2.2. The Shell

- A shell is a special-purpose program designed to read commands typed by a user and execute appropriate programs in response to those commands.
- A number of important shells have appeared over time:
  - 1. Bourne shell (sh).
  - 2. C shell (csh).
  - 3. Korn shell (ksh).
  - 4. Bourne again shell (bash): This shell is the GNU project’s re-implementation of the Bourne shell.

### 2.3. Users and Groups

- Each user on the system is uniquely identified, and users may belong to groups.

- Users:
  - Every user has a unique `login name` (username) and a corresponding numeric user ID (UID).
  - For each user, these are defined by a line in the system *password file*, `/etc/passwd`, which includes the additional information:
    - 1. **Group ID**: The numeric group ID of the first of the groups of which the user is a member.
    - 2. **Home Directory**: the initial directory into which the user is placed after logging in.
    - 3. **Login Shell**: the name of the program to be executed to interpret user commands.

- Groups:
  - For administrative purposes - in particular, for controlling access to files and other system resources - it is useful to organize user into `groups`.

- Superuser:
  - One user, known as the `superuser`, has special privileges within the system. The superuser account has the user ID 0, and normally has the login name `root`.
  - On typical Unix system, the superuser bypasses all permission checks in the system.

### 2.4. Single directory Hierarchy, Directories, Links, and Files

- The kernel mains a single hierarchical directory structure to organize all files in the system.
- At the base of this hierarchy is the `root directory`, named `/` (slash). All files and directories and children or further removed descendants of the root directory.

#### 2.4.1. File types

- Within the filesystem, each file is marked with a `type`, indicating what kind of file it is.

- One of these file type denotes ordinary data files, which are usually called `regular` or `plain` files to distinguish them from other file types. These other file types include devices, pipes, sockets, directories, and symbolic links.

- The term `file` is commonly used to denote a file of any type, not just a regular file.

#### 2.4.2. Directories and links

- **A directory is special file** whose contents take the form of a table of filenames coupled with references to the corresponding files. This filename-plus-reference association is called a `link`, and files may have multiple links, and thus multiple names, in the same or in different directories.

- Directories may contain links both to files and to other directories.

- Every directory contains at least two entries: `.` (dot), which is a link to the directory itself, and `..` (dot-dot), which is a link to its `parent directory`.

#### 2.4.3. Symbolic links

- Like a normal link, a `symbolic link` provides an alternative name for a file. But whereas a normal link is a `filename-plus-pointer` entry in a directory list, **a symbolic link is a specially marked file containing the name of another file**. This later file is often called the target of the symbolic link, and it is common to say that the symbolic link `points` or `refers` to the target file.

- When a pathname is specified in a system call, in most circumstances, the kernel automatically *dereferences* each symbolic link in the pathname, replace it with the filename to which it points. This process may happen recursively if the target of a symbolic.

- If a symbolic link refers to a file that doesn't exist, it is said to be a `dangling link`.

- hard-link is normal link.
- soft-link is symbolic link.

#### 2.4.4. File ownership and permissions

- Each file has an associated user IS and group ID that define the owner of the file and the group to which it belongs. The ownership of a file is used to determine the access rights available to users of the file.

### 2.5. File I/O model

- One of the distinguishing features of the I/O model in Unix systems is the concept of `universality of I/O`. This means that the same system calls (`open()`, `read()`, `write()`, `close()` and so on) are used to perform I/O on all types of files, including devices. ( The kernel translates the application's I/O requests into appropriate file-system or device-driver operations that perform I/O on the target file or device).
- Thus, a program employing these system calls will work on any type of file.

#### 2.5.1. File descriptors

- The I/O system calls refer to open files using a `file descriptor`, a (usually small) non-negative integer.
- A file descriptor is typically obtained by a call to `open()`, which takes a pathname argument specifying a file upon which I/O is to be performed.
- Normally, a process inherits three open file descriptors when it is started by the shell:
  - 1. descriptor 0 is standard input.
  - 2. descriptor 1 is standard output.
  - 3. descriptor 2 is standard error.

#### 2.5.2. The `stdio` library

- To perform file I/O, C programs typically employ I/O functions contained in the standard C library. This set of functions, referred to as the `stdio` library, includes `fopen()`, `fclose()`, `scanf()`, `printf()`, `fgets()`, `fputs()` and so on. The `stdio` functions are layered on top of the I/O system calls (`open()`, `close()`, `read()`, `write()` and so on).

### 2.6. Programs

- Programs normally exist in two forms. The first form is `source code`, human-readable. To be executed, source code must be converted to the second form: `binary` machine language instructions that the computer can understand.
- The two meanings of the term `program` are normally considered synonymous.

#### 2.6.1. Filters

- A `filter` is the name often applied to a program that reads its input from `stdin`, performs some transformation of that input, and writes the transformed data to `stdout`. Examples of filters include `cat`, `grep`, `tr`, `sort`, `wc`, `sed`, and `awk`.

#### 2.6.2. Command-line arguments

- In C, programs can access the command-line arguments, the words that are supplied in the command line when the program is run. To access the command arguments, the main function of the program is declared as follows:

```C
int main(int argc, char *argv[])
```

### 2.7. Processes

- Put most simply, a `process` is an instance of an executing program. When a program is executed, the kernel loads the code of the program into virtual memory, allocates space for program variables, and sets up kernel bookkeeping data structures to record various information (such as process ID, termination status, user IDs, and group IDs) about the process.

- From a kernel point of view, processes are the entities among which the kernel must share the various resource of the kernel.
- For resources that are limited, such as memory, the kernel initially allocates some amount of the resource to the process, and adjusts this allocation over the lifetime of the process in responsible to the demands of the process and overall system demand for that resource.
- When process terminates, all such resources are released for reuse by other processes.
- Other resources, such as the CPU and network bandwidth, are renew-able, but must be shared equitably among all processes.

#### 2.7.1. Process memory layout

- A process is logically divided into the following parts, known as `segments`:
  - 1. `Text`: the instructions of the program.
  - 2. `Data`: the static variables used by the program.
  - 3. `Heap`: an area from which programs can dynamically allocate extra memory.
  - 4. `Stack`: a piece of memory that grows and shrinks as functions are called and return and that is used to allocate storage for local variables and function call linkage information.

#### 2.7.2. Process creation and program execution

- A process can create a new process using `fork()` system call. The process that calls `fork()` is referred to as the `parent process`, and the new process is referred to as `child process`.

- The kernel creates the child process by making a duplicate of the parent process. The child inherits copies of the parent's data, stack, and heap segments, which it may then modify independently of the parent's copies. (The program text, which is placed in memory marked as read-only, is shared by the two processes).

- The child process goes on either to execute a different set of functions in the same code as the parent, or frequently, to use the `execve()` system call to load and execute an entirely new program. An `execve()` call destroys the existing text, data, stack and heap segments, replacing them with new segments based on the code of the new program.

#### 2.7.3. Process ID and parent process ID

- Each process unique integer `process identifier (PID)`. Each process also has a `parent process identifier (PPID)` attribute, which identifies the process that requested the kernel to create this process.

#### 2.7.4. Process termination and termination status

- A process can terminate in one of two ways:
  - 1. By requesting its own termination using the `_exit()` system call (or the related `exit()` lib function),
  - 2. Or by being killed by the delivery of a signal.

- In either case, the process yields a `termination status`, a small non-negative integer value that is available for inspection by the parent process using the `wait()` system call.

- In the case of a call to `_exit()`, the process explicitly specifies its own termination status.
- If a process is killed by a signal, the termination status is set according to the type of signal that caused the death of the process.

- By convention, a termination status of 0 indicates that process succeeded, and a nonzero status indicates that some error occurred.

- Most shells make the termination status of the last executed program available via a shell variable named `$?`.

#### 2.7.5. Process user and group identifiers (credentials)

- Each process has a number of associated user IDs (UIDs) and group IDs (GIDs). These include:
  - `Real user ID` and `real group ID`: These identify the user and group to which the process belongs. A new process inherits these IDs from its parent. A login shell gets its real user ID and real group ID from the corresponding fields in the system password file.
  - `Effective user ID` and `Effective group ID`: These two IDs (in conjunction with the supplementary groups IDs discussed in a moment) are used in determining the permission that process has when accessing protected resources such as files and inter-process communication objects.
    - Typically, `Effective user IS` has the same values as the corresponding real IDs.

#### 2.7.6. Privileged Processes

- Traditionally, on Unix systems, a `privileged process` is one whose `effective user ID` is 0 (superuser).
- A process may be privileged because it was created by another privileged process - for example, by a login shell started by root (superuser).
- Another way a process may become privileged is via the set-user-ID mechanism, which allows a process to assume an effective user ID that is the same as the user ID of the program file that it is executing.

#### 2.7.7. Capabilities

- Linux divides the privileges traditionally accorded to the super user into a set of distinct units called `capabilities`. Each privileged operation is associated with a particular capability, and a process can perform an operation only if it has the corresponding capability.

#### 2.7.8. The `init` process

- When booting the system, the kernel creates a special process called `init`, the `parent of all processes`, which is derived from the program file (`sbin/init`).

- All processes on the system are created (using `fork()`) either by `init` or by one of its descendants.

- The init process always has the process ID `1` and runs with superuser privileges.

- The `init` process **CAN NOT** be killed (not even by the superuser), and it terminates only when the system is shutdown.

- **The main task of `init` is to create and monitor a range of processes required by a running system**.

#### 2.7.9. Daemon Processes

- A `daemon` is a special-purpose process that is created and handled by the system in the way as other processes, but which is distinguished by the following characteristics:
  - 1. It is long-lived. A daemon process is often started at system boot and remains in existence until the system is shutdown.
  - 2. It runs in the background, and has no controlling terminal from which it can read input or to which it can write output.

- Examples of daemon processes include `syslogd`, which records messages in the system log, and `httpd`, which serves web pages.

#### 2.7.10. Environment list

- Each process has an `environment list`, which is a set of `environment variables` that are maintained within the user-space memory of the process.
- Each element of this list consists of a name an associated value.

- When a new process is created via `fork()`, it inherits a copy of it's parent's environment. Thus, the environment provides a mechanism for a parent process to communicate information to a child process.

- When a process replaces the program that it is running `exec()`, the new program either inherits the environment used by the old program or receives a new environment specified as part of the `exec()` call.

- Environment variables are created with the `export` command in most shells, as in the following example:

```bash
export VAR="Hello world"
```

- C program can access the environment using an external variable `char **environ`.

- Environment variables are used for variety of purposes. For example, the shell defines and uses a range of variables that can be accessed by scripts and programs executed from the shell.

#### 2.7.11. Resource limits

- Each process consumes resources, such as open files, memory and CPU time. Using `setlimit()` system call, a process can establish upper limits on its consumption of various resources.

- When a new process is created with `fork()`, it inherits copies of its parent's resource limit settings.

- The resource limits of the shell can be adjusted using the `ulimit` command. These limit settings are inherited by the child processes that shell creates to execute commands.

### 2.8. Memory Mappings

- The `mmap()` system call creates a new `memory mapping` in the calling process's virtual address space.

- Mappings fall into two categories:
  - 1. A `file mapping` maps a region of a file into the calling process's virtual memory. Once mapped, the file's contents can be accessed by operations on the bytes in the corresponding memory region. The pages of the mapping are automatically loaded from the file as required.
  - 2. By contrast, an `anonymous mapping` doesn't have a corresponding file. Instead, the pages of the mapping are initialized to 0.

- The memory in one process's mapping may be shared with mappings in other processes. This can occur either because two processes map the same region of a file or because a child process created by `fork()` inherits a mapping from its parent.

- When two or more processes share the same pages, each process may see the changes made by other processes to the contents of the pages, depending on whether the mapping is created as `private` or `shared`.
  - When a mapping is `private`, modifications to the contents of the mapping are not visible to other processes and are not carried through to the underlying file.
  - When a mapping is `shared`, modifications to the contents of the mapping are visible to other processes sharing the same mapping and are carried through to the underlying file.

- Memory mappings serve a variety of purposes:
  - 1. Including initialization of a process's text segment from the corresponding segment of an execute file,
  - 2. allocation of new (zero-filled) memory,
  - 3. file I/O (memory-mapped I/O),
  - 4. and inter-process communication (via a shared mapping).

### 2.9. Static and Shared Libraries

- An `object library` is a file containing the compiled object code for a (usually logically related) set of functions that may be called from application programs.

- Placing code for a set of functions in a single object library eases the tasks of program creation and maintenance. Modern UNIX systems provide two types of object libraries:
  - `static` and `shared`.

#### 2.9.1. Static libraries

- Static lib (sometimes also known as `archives`) were the only type of library on early UNIX systems.
- A static library is essentially a structured bundle of compiled object modules.
- To use functions from a static lib, we specify that library in the link command used to build a program.

- After resolving the various function references from the main program to the modules in the static library, the linker extracts copies of the required object modules from the library and copies these into the resulting executable file. We say that such a program is `statically linked`.

- The fact that each statically linked program includes its own copy of the object modules required from the library creates a number of disadvantages.
- 1. One is that the duplication of object code in different executable files wastes disk space.
- 2. A corresponding waste of memory occurs when statically linked programs using the same library function are executed at the same time.
- 3. Each program requires its own copy of the function to reside in memory.
- 4. Additionally, if a library function requires modification, then after recompiling that function and adding it to the static lib, all applications that need to use the updated function must be relinked against the lib.

#### 2.9.2. Shared lib

- Shared libs were design to address the problems of statics.
- If a program is linked against a shared lib, then, instead of copying object modules from the lib into the executable, the linker just writes a record into the executable to indicate that at runtime the executable needs to use that shared lib.
- When the executable is loaded into memory at runtime, a program called the **dynamic linker** ensures that the shared libraries required by the executable are found and loaded into memory, and performs run-time linking to resolve the function calls in the executable to the corresponding definitions in the shared libs.
- At runtime, only a single copy of the code of the code of the shared lib needs to be resident in memory; all running programs can use that copy.

- The fact that a shared lib contains the sole compiled version of a function saves disk space. It also greatly eases the job of ensuring that programs employ the newest version of a function.
- Simply rebuilding the shared lib with the new function definition causes existing programs to automatically use the new definition wne they are next executed.

### 2.10. Inter-process Communication and Synchronization

- A running Linux system consists of numerous processes, many of which operate independently of each other. Some processes, however, cooperate to achieve their intended purposes, and these processes need methods of communicating with one another and synchronizing their actions.

- One way for processes to communicate is by reading and writing information in disk files. However, for many applications, this is too slow and inflexible.

- Therefore, Linux, like all modern UNIX implementations, provides a rich set of mechanisms for IPC, including the following:
  - 1. **signals**, which are used to indicate that an event has occurred.
  - 2. **pipes** (familiar to shell users as the `|` operator) and **FIFOs**, which can be used to transfer data bw processes.
  - 3. **sockets**, which can be used to transfer data from one process to another, either on the same host computer or on different hosts connected by a network;
  - 4. **file locking**, which allows a process to lock regions of a file in order to prevent other processes from reading or updating the file contents.
  - 5. **message queues**, which are used to exchange messages (packets of data) bw processes.
  - 6. **semaphores**, which are used to synchronize the actions of processes; and
  - 7. **shared memory**, which allows two or more processes to share a piece of memory. When one process changes the contents of the shared memory, all of the other processes can immediately see the changes.

- The wide variety of IPC mechanisms on UNIX systems, with sometimes overlapping functionality, is in part due to their evolution under different variants of the UNIX system and the requirements of various standard. For example, FIFOs and UNIX domain sockets essentially perform the same function of allowing unrelated processes on the same system to exchange data. Both exists in modern UNIX system because FIFOs came from System V, while sockets came from BSD.

### 2.11. Signals

- Although we listed them as a method of IPC in the previous section, signals are more usually employed in a wide range of other contexts, and so deserve a longer discussion.

- Signals are often described as `software interrupt`. The arrival of a signal informs a process that some event or exceptional condition has occurred. There are various types of signals, each of which identifies a different event or condition. Each signal type is identified by a different integer, defined with symbolic names of the form `SIGxxxx`.

- Signals are sent to a process by the kernel, by another process (with suitable permissions), or by the process itself. For example, the kernel may send a signal to a process when one of the following occurs:
  - 1. The user typed the `interrupt` character (usually `Control-C`) on the key board.
  - 2. one of the process's children has terminated.
  - 3. a timer (alarm clock) set by the process has expired;
  - 4. The process attempted to access an invalid memory address.

- Within the shell, the `kill` command can be used to send a signal to a process. The `kill()` system call provides the same facility within programs.

- When a process receives a signal, it takes one of the following actions, depending on the signal:
  - 1. It ignore the signal.
  - 2. It is killed by the signal;
  - 3. It is suspended until later being resumed by receipt of a special-purpose signal.

- For most signal types, instead of accepting the default signal action, a program can choose to ignore the signal, or to establish a `signal handler`. A signal handler is a programmer-defined function that is automatically invoked when the signal is delivered to the process. This function performs some action appropriate to the condition that generated the signal.

- In the interval between the time it is generated and the time it is delivered, a signal said to be `pending` for a process. Normally, a pending signal is delivered as soon as the receiving process is next scheduled to run, or immediately of the process is already running. However, it is also possible to `block` a signal by adding it to the process's `signal mask`. If a signal is generated while it is blocked, it remains pending until it is later unlocked.

### 2.12. Threads

- In modern UNIX implementations, each process can have multiple `threads` of execution. One way of envisaging threads is as a set of processes that share the same virtual memory, as well as a range of other attributes.
- Each thread is executing the same program code and shares the same data area and heap. However, each thread has it own stack containing local variables and function call linkage information.

- Threads can communicate with each other via the global variables that they share. The threading API provides `condition variables` and `mutexes`, which are primitives that enable the threads of a process to communicate and synchronize their actions, in particular, their use of shared variables. Threads can also communicate with one another using the IPC and synchronization mechanism.

- The primary advantages of using threads are that they make it easy to share data (via global variables) bw cooperating threads and that some algorithms transpose more naturally to a multithreaded implementation than to a multi-process implementation.

- Furthermore, a multi-threaded application can transparently take advantage of the possibility for parallel processing on multi-processor hardware.

### 2.13. Process groups and shell job control

- Each program executed by the shell is started in a new process. For example, the shell creates three processes to execute the following pipeline of commands:

```bash
ls -l | sort -k5n | less
```

- All major shell, provide an interactive feature called `job control`, which allows the user to simultaneously execute and manipulate multiple commands and pipelines.
- In job control shells, all of the processes in a pipeline are placed in a new `process group` or `job`.
- Each process in a process group has the same integer `process group identifier`, which is the same as the process ID of one of the processes in the group, termed the `process group leader`.

- The kernel allows various actions, notably the delivery of signals, to be performed on all members of a process group. `Job-control` shells use this feature to allow the user to suspend or resume all of the processes in a pipeline

### 2.14. Sessions, Controlling Terminals, and Controlling Processes

- A `session` is a collection of process groups (jobs). All of the processes in a session have the same `session identifier`. A `session leader` is the process that created the session, and its process ID becomes the session ID.

- Sessions are used mainly by job-control shells. All of the process groups created by a job-control shell belong to the same session as the shell, which is the session leader.

- Sessions usually have an associated `controlling terminal`. The controlling terminal is established when the session leader process first opens a terminal device. For a session created by an interactive shell, this is the terminal at which the user logged in. A terminal may be the controlling terminal of at most one session.

- As a consequence of opening the controlling terminal, the session leader becomes the `controlling process` for the terminal. The controlling process receives a `SIGHUP` signal if a terminal disconnect occurs.

### 2.15. Pseudo-terminals

- A `pseudo-terminal` is a pair of connected virtual devices, known as the `master` and `slave`. This device pair provides an IPC channel allowing data to be transferred in both directions bw the two devices.

- The key point about a `pseudo-terminal` is that the slave device provides an interface that behaves like a terminal, which makes it possible to connect a terminal-oriented program to the slave device and then use another program connected to the master device to drive the terminal-oriented program. Output written by the driver program undergoes the usual input processing performed by the terminal driver and is then passed as input to the terminal-oriented program connected to the slave.

- Pseudo-terminal are used in a variety of applications, most notably in the implementation of terminal windows provided under an X window system login and in applications providing network login services, such as `telnet` and `ssh`.

### 2.16. Date and time

- Two type of time are of interest to a process:
  - 1. `real time` is measured either from some standard point (calendar time) or from some fixed point. On Unix systems, calendar time us measured ni seconds since midnight on the 01/01/1970. This date, which is close to the birth of the UNIX system, is referred to as the `Epoch`.
  - 2. `Process time`, also called `CPU time`, is the total amount of CPU time that a process has used since starting. It's divided into `system CPU time` (kernel mode) and `user CPU time` (user mode).

- `time` command display all of them.

### 2.17. Client-Server Architecture

- A client-server application is one that is broken into two component processes:
  - 1. a `client`, which asks the server to carry out some service by sending it a request message;
  - 2. a `server`, which examines the client's request, performs appropriate actions and then sends a response message back to the client.

- Sometimes, the client interact with users, and server interact with resources.

- Encapsulating a service within a single server is useful for a number of reasons, such as following:
  - 1. **Efficiency**: It may be cheaper to provide one instance of resources that is managed by a server than to provide the same resource locally on every computer.
  - 2. **Control, coordination, and security**.
  - 3. **Operations in a heterogeneous environment**: In a network, the various clients and the server, can be running on different hw and OS platforms.

### 2.18. Realtime

- `Realtime applications` are those that need to respond in a timely fashion to input. Frequently, such input comes from an external sensor or a specialized input device, and output takes the form of controlling some external hw.
  - For example: back ATM, automated assembly lines, aircraft navigation systems.

- Although many realtime applications require rapid responses to input, the defining factor is that the response is guaranteed to be delivered within a certain deadline time after the triggering event.

- The provision of realtime, responsiveness, especially where short response times are demanded, requires support from the underlying OS. Most OSes don't natively provide such support because the requirements of real responsiveness can conflict with the requirements of multi-user time-sharing OSes.

- Traditional UNIX implementations are **NOT** realtime OSes, although realtime variants have been devised. Real time variants of Linux have also been created, and recent Linux Kernels are moving toward full native support for real time applications.

- `POSIX.1b` defined a number of extensions to `POSIX.1` for the support of real-time applications. These include:
  - asynchronous I/O.
  - shared memory.
  - memory mapped files.
  - memory locking.
  - realtime clocks and timers.
  - alternative scheduling policies.
  - realtime signals.
  - messages queue.
  - semaphores.
- Even though they do strictly qualify as realtime.

### 2.19. The `/proc` File System

- Linux provides a `/proc` file system, which consists of a set of directories and files mounted under `/proc` directory.
- The `/proc` file system is a **VIRTUAL** file system that provides an interface to kernel data structures in a form that looks like files and directories on a file system.

- This provides an easy mechanism for viewing and changing various system attributes.

- In addition, a set of directories with names of the form `/proc/PID` where PID is process ID, allows us to view information about each running process.

- The content of `/proc` files are generally in human-readable text form and can parsed by shell script. A program can easy to open, read, write the desired files.

## 20. Signals: Fundamental concepts

- This section covers the following topics:
  - 1. The various different signals and their purposes.
  - 2. The circumstances in which the kernel may generate a signal for a process, and the system calls that one process may use to send a signal to another process;
  - 3. How a process responds to a signal by default, and the means by which a process can change its response to a signal, in particular, through the use of a signal handler, a programmer-defined function that is automatically invoked on receipt of a signal.
  - 4. The use of a process signal mask to block signals, and the associated notion of pending signals;
  - 5. How a process can suspend execution and wait for the delivery of a signal.

### 20.1. Concepts and Overview

- A`signal` is a notification to a process that an event has occurred. Signals are sometimes describes as `software interrupts`. Signal are analogous to hardware interrupts in that they interrupt the normal flow of execution of a program; in most cases, it is not possible to predict exactly a signal will arrive.

- 1. One process can send a signal to another. In this use, signals can be employed as a synchronization technique, or event as a primitive form of IPC.
- 2. It is also possible for a process to send a signal itself.
- 3. However, the usual source of many signals sent to a process is the kernel. Among the types of events that cause the kernel to generate a signal for a process are the following:
  - 1. A hardware exception occurred, meaning that the hw detected **A FAULT** condition that was notified to the kernel, which in turn sent a corresponding signal to the process concerned. Examples of hw exceptions include executing a malformed machine-language instruction, dividing by 0, or referencing a part of memory that is inaccessible.
  - 2. The user typed one of the terminal special characters that generate signals. These characters include the `interrupt` character (usually `Control-C`) and the `suspend` character (usually `Control-Z`).
  - 3. A software event occurred. For example, input became available on a file descriptor, the terminal window was resized, a timer went off, the process's CPU time limit was exceeded, or a child of this process terminated.

- Each signal is defined as a unique (small) integer, starting sequentially from `1`. These integers are defined in `<signal.h>` with symbolic names of the form `SIGxxxx`.
  - For example, when the user types the `interrupt` character, `SIGINT` (signal number 2) is delivered to a process.

- The signals fall into two broad categories.
  - 1. The first set constitutes the `traditional` or `standard` signals, which are used by the kernel to notify processes of events. This from `1` to `31`.
  - 2. The other of signals consists of the `realtime` signals.

- A signal is said to be `generated` by some event. Once generated, a signal is later `delivered` to a process, which then takes some action in response to the signal. Between the time it is generated and the time it is delivered, a signal is said to be `pending`.

- Normally, a pending signal is delivered to a process as soon as it is next scheduled to run, or immediately if the process is already running (e.g. if the process sent a signal to it self). Sometimes, however, we need to ensure that a segment of code is not interrupted by the delivery of a signal. To do this, we can add a signal to the process's `signal mask` - a set of signals whose delivery is currently `blocked`. If a signal is generated while it is blocked, it remains pending until it is later unblocked. Various system calls allow a process to add and remove signals from its signal mask.

- Upon delivery of a signal, a process carries out one of the following default actions, depending on the signal:
  - 1. The signal is `ignored`; that is, it is discarded by the kernel and has no effect on the process. (The process never even knows that it occurred).
  - 2. The process is `terminated` (killed). This is sometimes referred to as `abnormally process termination`, as opposed to the normal process termination that occurs when a process terminates using the `exit()`.
  - 3. A `core dump file` is generated, and the process is terminated. A core dump file contains an image of the virtual memory of the process, which can be loaded into a debugger in order to inspect the state of the process at the time thar it terminated.
  - 4. The process is `stopped` - execution of the process is suspended.
  - 5. Execution of the process is `resumed` after previously being stopped.

- Instead of accepting the default for a particular signal, a program can change the action that occurs when the signal is delivered. This is known as setting the `disposition` of the signal. A program can set one of the following dispositions of the following dispositions:
  - 1. The `default action` should occur. This is useful to undo an earlier change of the disposition of the signal to something other than its default.
  - 2. The signal is `ignored`.
  - 3. A signal handler is executed.

- `signal handler` is a function that performs appropriate tasks in response to the delivery of a signal.
  - For example, the shell has a handler for the `SIGINT` signal (generated by the `interrupt` character) that causes it to stop what it is currently doing and return control to the main input loop, so that the user is once more presented with the shell prompt.

- Note that it **IS NOT POSSIBLE** to set the disposition of a signal to **TERMINATE** or **DUMP CORE**. The nearest we can get to this is to install a handler for the signal that then calls either `exit()` or `abort()`. The `abort()` function  generates a `SIGABRT` signal for the process, which causes it to dump core and terminate.

### 20.2. Signal types and default actions

- We mentioned that the standard signals are numbered from 1 to 31 on Linux. The following list describes the various signals:
  - 1. `SIGABRT`: A process is sent this signal when it calls the `abort()` function. By default, this signal terminates the process with a core dump. This achieves the intended purpose of the `abort()` call: to produce a core dump for debugging.
  - 2. `SIGALRM`: The kernel generates this signal upon the expiration of a real-time timer set by a call to `alarm()` or `setitimer()`.
  - 3. `SIGBUS`: This signal (`bus error`) is generated to indicate certain kinds of memory-access errors.
  - 4. `SIGCHLD`: This signal is sent (by the kernel) to a parent process when one of its children terminates.
  - 5. `SIGCLD`: This is synonym.
  - 6. `SIGCONT`: When sent to a stopped process, this signal causes the process to resume.
  - 7. `SIGEMT`: In Unix systems generally, this signal is used to indicate an implementation-dependent hardware error.
  - 8. `SIGFPE`: This signal is generated for certain types of arithmetic errors, such as divide-by-zero. The suffix FPE is an abbreviation for `floating-point exception`.
  - 9. `SIGHUP`: When a terminal disconnect (hangup) occurs, this signal is sent to the controlling process of the terminal.
  - 10. `SIGILL`: This signal is sent to a process of it tries to execute an illegal machine-language instruction.
  - 11. `SIGINFO`: On Linux, this signal name is a synonym for `SIGPWR`. On BSD systems, the `SIGINFO` signal, generated by typing `Control-T`, is used to obtain status information about the foreground process group.
  - 12. `SIGINT`: When the user types the terminal `interrupt` character (usually `Control-C`), the terminal driver sends this signal to the foreground process group.
  - 13. `SIGIO`: Using the `fcntl()` system call, it is possible to arrange for this signal to be generated when an I/O event occurs on certain types of open file descriptors, such as those for terminals and sockets.
  - 14. `SIGIOT`: On Linux, this is a synonym for `SIGABRT`.
  - 15. `SIGKILL`: This is the **SURE KILL** signal. It can't be blocked, ignored, or caught by a handler, and thus always terminates a process.
  - 16. `SIGLOST`: This signal name exists on Linux, but is unused.
  - 17. `SIGPIPE`: This signal is generated when a process tries to write to a pipe, a FIFO, or a socket for which there is no corresponding reader process.
  - 18. `SIGPOLL`: This signal, which is derived from System V, is a synonym for `SIGIO` on Linux.
  - 19. `SIGPROF`: The kernel generates this signal upon the expiration of a profiling timer set by a call to `setitimer()`.
  - 20. `SIGPWR`: This is the `power failure` signal.
  - 21. `SIGQUIT`: When the user types the `quit` character (usually `Control-\`) on the keyboard, this signal is sent to the foreground process group.
  - 22. `SIGSEGV`: This very popular signal is generated when a program makes an invalid memory reference. The name of this signal derives from the term `segmentation violation`.
  - 23. `SIGSTKFLT`: `stack fault on coprocessor`.
  - 24. `SIGSTOP`: This is the sure stop signal. It can't be blocked, ignored or caught by a handler; thus, it always stops a process.
  - 25. `SIGSYS`: This signal is generated if a process makes a `bad` system call.
  - 26. `SIGTERM`: This is standard signal used for terminating a process and is the default signal sent by the `kill` and `killall` command. NOTE: It's different with `SIGKILL` - `kill -9`.
  - 27. `SIGTRAP`: This signal is used to implement debugger breakpoints and system call tracing, as performed by `stract()`.
  - 28. `SIGTSTP`: This is the job-control `stop` signal, sent to stop the the foreground process group when the user types the `suspend` character (usually `Control-Z`) on the keyboard.
  - 29. `SIGTTIN`: When running under a job-control shell, the terminal driver sends this signal to a background process group when it attempts to `read()` from the terminal.
  - 30. `SIGTTOU`: This signal serves an analogous purpose to `SIGTTIN`, but fot terminal output by background jobs.
  - 31. `SIGUNUSED`: As the name implies, this signal is unused.
  - 32. `SIGURG`: This signal is sent to a process to indicate the presence of `out-of-band`.
  - 33. `SIGUSR1`: This signal and `SIGUSR2` are available for programmer-defined purposes.
  - 34. `SIGUSR2`: similar with `SIGUSR1`.
  - 35. `SIGVTALRM`: The kernel generates this signal upon expiration of a virtual timer set by a call to `setitimer()`.
  - 36. `SIGWINCH`: In a windowing environment, this signal is sent to the foreground process group when the terminal window size change.
  - 37. `SIGXCPU`: This signal is sent to a process when it exceeds its CPU time resource limit.
  - 38. `SIGXFSZ`: This signal is sent to a process of it attempts to increase the size of a file beyond the process's file size resource limit.

### 20.3. Changing Signal Dispositions: `signal()`

- Unix system provide two way of changing the disposition of a signal: `signal()` and `sigaction()`.

- `sigaction()` provides functionality that is not available with signal(). Because of these portability issues, `sigaction()` is the (strongly) preferred API for establishing a signal handler.

```C
#include <signal.h>
void ( *signal(int sig, void (*handler)(int))) (int);
```

- A signal handler has the following general form:

```C
void
handler(int sig)
{
  /* Code for the handler */
}
```

- The return value of `signal()` is the previous disposition of the signal.
- NOTE: we can make the prototype for `signal()` much more comprehensive by using the following type definition for a pointer to a signal handler function:

```C
typedef void (*sig_handler_t)(int);
```

- This enable us to rewrite the prototype for `signal()` as follows:

```C
sig_handler_t signal(int sig, sig_handler_t handler);
```

### 20.4. Introduction to Signal Handlers

- A `signal handler` (also called a `signal catcher`) is a function that is called when a specified signal is delivered to a process.

- Invocation of a signal handler may interrupt the main program flow at any time; the kernel calls the handler on the process's behalf, and when the handler returns, execution of the program resumes at the point where the handler interrupted it.

```text
                Main program
             ____________________________
            | start of program      ||   |
            |                      Flow  |
            |                       Of   |
            |                   Execution|
            |                       ||   |
            |                       \/   |               __signal_handler___
 Deliver    |                       || //==>====(2)=====|==\\ (3)           |
of signal   | instruction m         ||// |              |   Code of signal  |
  (1)-------|-->                     <<  |              |   handler is      |
            | instruction m + 1     ||\\ |              |   executed        |
            | ...                   || \\==<====(4)=====|==// return        |
            |                       ||   |              |___________________|
            | exit()                \/   |
            |____________________________|
```

- (1) Deliver of signal.
- (2) Kernel calls signal handler on behalf or process.
- (3) Code of signal handler is executed.
- (4) Program resumes at point of interruption.

- When the kernel invokes a signal handler, it passes the number of the signal that caused the invocation as an integer argument to the handler.
- If a signal handler catches only one type of signal, then this argument is of little use. We can, however establish the same handler to catch different types of signals and use this argument to determine which signal caused the handler to be invoked.

### 20.5. Sending signals: `kill()`

- One process can send a signal to another process using the `kill()` system call, which is the analog of the `kill` shell command.

```C
#include <signals.h>

int kill(pid_t pid, int sig);
```

- NOTE:
  - if `pid` equal 0, the signal is sent to every process in the same process group as the calling process, include calling process itself.
  - If `pid` less than -1, the signal is sent to all of the processes in the process group whose ID equals the absolute value of `pid`.

### 20.6. Checking for existence of a process

- The `kill()` system can serve another purpose. If the `sig` argument os specified as 0, then no signal is sent. Instead, `kill()` merely performs error checking to see if the process can be signaled. In other way, we can use this feature to check the PID exist.

### 20.7. Other ways of sending signals: `raise()` and `killpg()`

- Sometimes, it is useful for a process to send a signal to itself.

```C
#include <signal.h>

int raise(int sig);
```

- In a single-threaded program, a call to `raise()` is equivalent to the following call to `kill()`:

```C
kill(getpid(), sig);
```

- On a system that supports threads, `raise(sig)` is implemented as:

```C
pthread_kill(pthread_self(), self);
```

- When a process send itself a signal using `raise()` (or `kill()`) the signal is delivered immediately.

- The killpg() function sends a signal to all of the members of a process group.

```C
#include <signal.h>
int killpg(pid_t pgrp, int sig);
```

### 20.8. Displaying Signal Description

- Each signal has an associated printable description. These description are listed in the array `sys_siglist`. For example we can check the info by access: `sys_siglist[SIGINT];`.

- The `strsignal()` function is preferable.

```C
#include <signal.h>

extern const char *const sys_siglist[];
char *strsignal(int sig);
```

## 29. Threads: Introduction

### 29.1. Overview

- Like processes, threads are a mechanism that permits an application to perform multiple tasks concurrently.
- All of these threads are independently executing the same program, and they all share the same global memory, including the initialized data, uninitialized data, and heap segments.

- On a multiprocessor system, multiple threads can execute in parallel. If one thread is blocked on I/O, other threads are still eligible to execute.

```text
Virtual memory address
    (hexadecimal)___________________
    0xC0000000  |__argv,_environ____|
                |  Stack for main   |
                |       thread      |
                |- - - - - - - - - -|
                |        ||         |
                |        \/         |
                |                   |
                |                   |
                |-------------------|
                | Stack for thread 3|
                |-------------------|
                | Stack for thread 2|
                |-------------------|
                | Stack for thread 1|
                |-------------------|
                | Shared libraries, |
    0x40000000  |   shared memory   |
                |-------------------|
 UNMAPPED_BASE  |                   |
                |                   |
                |        /\         |
                |        ||         |
                |- - - - - - - - - -|
                |       Heap        |
                |___________________|
                |Uninitialized data |
                |______(bss)________|
                |  Initialized data |
                |___________________|
                |                   |<--- thread 3 executing here.
                |                   |<--- main thread executing here.
                |   Text (program   |<--- thread 1 executing here.
                |     code)         |<--- thread 2 executing here.
    0x80480000  |___________________|
    0x00000000  |___________________|
```

- Threads offer advantages over processes in certain applications. Consider the traditional UNIX approach to achieving concurrency by creating multiple processes. An example of this is a network server design in which a parent process accepts incoming connections from clients, and then use `fork()` to create a separate child process to handle communication with each client. Such a design makes it possible to serve multiple clients simultaneously. While this approach works well for many scenarios, it does have the following limitations in some applications:
  - 1. It is difficult to share information bw processes. Since the parent and child don't share memory (other than the read-only text segment), we must use some form of IPC in order to exchange information bw processes.
  - 2. Process creation with `fork()` is relatively expensive. Even with the `copy-on-write` technique, the need to duplicate various process attributes such as page tables and file descriptor tables means that a `fork()` call is still time-consuming.

- Threads address both of these problems:
  - 1. Sharing information bw threads is easy and fast. It is just a matter of copying data into shared variables.
  - 2. Thread creation is faster than process creation. Ten times faster or better. Thread creation is faster because many of the attributes that must be duplicated in a child created by `fork()` are instead shared bw threads. In particular, copy-on-write duplication of pages of memory is not required, nor is duplication of page tables.

- Besides global memory, threads also shared a number of other attributes (i.e. these attributes are global to a process, rather than specific to a thread). These attributes include the following:
  - 1. Process ID and parent process ID;
  - 2. Process group ID and and session ID;
  - 3. Controlling terminal;
  - 4. Process credentials (user and group IDs);
  - 5. Open file descriptors;
  - 6. Record locks created using `fcntl()`;
  - 7. signal dispositions;
  - 8. file system-related information: umask, current work directory, and root directory;
  - 9. Interval timers and POSIX timers.
  - 10. System V semaphore undo (semadj) values;
  - 11. Resource limits;
  - 12. CPU time consumed;
  - 13. Resources consumed;
  - 14. Nice value.

- Among the attributes that are distinct for each thread are the following:
  - 1. Thread ID;
  - 2. Signal mask;
  - 3. Thread-specific data;
  - 4. Alternate signal stack;
  - 5. The `errno` variable;
  - 6. Floating-point environment;
  - 7. real time scheduling policy and priority;
  - 8. CPU affinity;
  - 9. Capabilities;
  - 10. Stack (local variable and function call linkage information);

### 29.2. Background Details of the Pthreads API

#### 29.2.1. Pthreads data types

- The Pthreads API defines a number of data types, some of which:

| Data type           | Description                   |
|---------------------|-------------------------------|
| pthread_t           | Thread identifier             |
| pthread_mutex_t     | Mutex                         |
| pthread_mutexattr_t | Mutex attributes              |
| pthread_cond_t      | Condition variable            |
| pthread_condattr_t  | Condition variable attributes |
| pthread_key_t       | Key for thread-specific data  |
| pthread_once_t      | One-time init control context |
| pthread_attr_t      | Thread attributes             |

#### 29.2.2. Threads and `errno`

- In the traditional UNIX API, `errno` is a global integer variable. However,this doesn't suffice for threaded programs. If a thread made a function call that returned an error in a global `errno` variable, then this would confuse other threads that might also be making function calls and checking `errno`. In other words, race conditions would result. **Therefore, in threaded programs, each thread has its own `errno`**.

- On Linux, a thread-specific `errno` is achieved in a similar manner to most other UNIX implementations: `errno` is defined as a macro that expands into a function call returning a modifiable `lvalue` that is distinct for each thread.

#### 29.2.3. Return value from Pthreads functions

- The traditional method of returning status from system calls and some library functions is return to `0` on success and `-1` on error, with `errno` being set to indicate the error.

- The functions in the Pthreads API do things differently. All Pthreads functions return 0 on success or a **positive value on failure**. The failure value is one of the same values that can be placed in `errno` by traditional UNIX system calls.

- Because each reference to `errno` in a threaded program carries the overhead of function call, our programs don't directly assign the return value of Pthreads function to `errno`. Instead, we use an intermediate variable and employ our `errExitEN()` diagnostic, like so:

```C
pthread *thread;
s = pthread_create(&thread, NULL, func, &arg);
if (s != 0)
{
  errExitEN(s, "pthread_create");
}
```

#### 29.2.4. Compiling Pthreads programs

- On Linux, programs that use the Pthreads API must be compiled with the `cc -pthread` option. The effects of this option include the following:
  - 1. The `_REENTRANT` pre-processor macro is defined. This causes the declarations of a few reentrant functions to be exposed.
  - 2. The program is linked with the `libthread` library (the equivalent `-lpthread`).

### 29.3. Thread Creation

- When a program is started, the resulting process consists of a single thread, called the `initial` or `main` thread.

- The `pthread_create()` function creates a new thread:

```C
#include <pthread.h>

int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void* (start_routine)(void *), void *arg);
```

- The new thread commences execution by calling the function identified by `start_routine` with the argument `arg`. The thread that calls `pthread_create()` continues execution with the next statement that follows the call.

- The `arg` argument is declared as `void *`, meaning that we can pass a pointer to any type of object to the `start_routine` function. Typically, `arg` points to a global or heap variable, but it can also be specified as `NULL`.
- If we need to pass multiple argument to `start_routine`, then arg can be specified as a pointer to a structure containing the arguments as separate fields.

- NOTE: The return value of `start_routine` is likewise of type `void *`, and it can be employed in the same way as the `arg` argument. We can get this use `pthread_join()`.

- NOTE: The `attr` argument is a pointer to a `pthread_attr_t` object that specifies various attributes for the new thread.
  - If `attr` is NULL, then the thread is created with various default attributes.

- After a call to `pthread_create()`, a program has no guarantees about which thread will next be scheduled to use the CPU (on a multiprocessor system, both threads may simultaneously execute on different CPUs).

### 29.4. Thread Termination

- The execution of a thread terminates in one of the following ways:
  - 1. The thread's start function performs a `return` specifying a return value for the thread.
  - 2. The thread calls `pthread_exit()`.
  - 3. The thread is canceled using `pthread_cancel()`.
  - 4. Any of the threads calls `exit()`, ot the main thread perform a`return` (in `main()`), which causes all threads in process to terminate immediately.

- `pthread_exit()` function terminates the calling thread, and specifies a return that can be obtained in another thread by calling `pthread_join()`.

```C
#include <pthread.h>
void pthread_exit(void *ret);
```

- Calling `pthread_exit()` is equivalent to perform a `return` in the thread's start function, with the different that `pthread_exit()` **can be called from any function** that has been called by the thread's start function.

- This function do not work with main thread.

### 29.5. Thread IDs

- Each thread within a process is uniquely identified by a thread ID. And thread can obtain its own ID using `pthread_self()`.

```C
#include <pthread.h>
pthread_t pthread_self(void);
```

- Thread IDs are useful within applications for the following reasons:
  - 1. Various Pthreads function use thread IDs to identify the thread on which they are to act. Examples of such functions include `pthread_join()`, `pthread_detach()`, `pthread_cancel()`, `pthread_kill()`.
  - 2. In some applications, it can be useful to tag dynamic data structures with the ID of a particular thread. This can serve to identify the thread that created or **owns** a data structure, or can be used by one thread to identify a specific thread that should subsequently do something with the data structure.

- The `pthread_equal()` function allows us check whether two thread IDs are the same.

- NOTE: SUSv3 doesn't require `pthread_t` to be implemented as a scalar type; it could be a structure. Therefore, we **CAN NOT** portably use code such as following to display a thread ID:

```C
pthread_t thr;
printf("Thread ID = %ld\n", (long) thr); /* WRONG! */
```

- In the Linux threading implementations, thread IDs are unique across processes.

### 29.6. Joining with a Terminated Thread

- The `pthread_join()` function waits for the thread identified by `thread` to terminate. (If the thread has already terminated, `pthread_join()` returns immediately). This operation is termed **JOINING**.

```C
#include <pthread.h>
int pthread_join(pthread_t thread, void **retval);
```

- A process can join itself.
- Joining a joined thread may lead to unpredictable behaviour. Because maybe thread ID is reuse by a new one.
- If thread is not detacted, then we **MUST** join with it using pthread_join(). If we fail to do this, then, when the thread terminates, it produces the thread equivalent of a zombie process. Aside from wasting system resources, if enough thread zombies accumulate, we won't be able to create additional threads.

- The task that `pthread_join()` performs for threads is similar to that performed by `waitpid()` for processes. However, there are some notable differences:
  - 1. Threads are peers. Any thread in a process can use `pthread_join()` to join with any other thread in the process.
  - 2. There is noway of saying *join with any thread* (for processes, we can do this using the call `waitpid(-1, ...)`); nor is there a way to do a nonblocking join.

### 29.7. Detaching a Thread

- By default, a thread is **joinable**, meaning that when it terminates, another thread can obtain its return status using `pthread_join()`. Sometimes, we don't care about the thread's return status; we simply want the system to automatically clean up and remove the thread when it terminates. In this case, we can mark the thread as **detached**:

```C
#include <pthread.h>
int pthread_detach(pthread_t thread);
```

- NOTE: A thread can detach itself using:

```C
pthread_detach(pthread_self());
```

- Once a thread has been detached, it is no longer possible to use the `pthread_join()` to obtain its return status, and the thread can't be made joinable again.
- Detaching a thread doesn't make it immune to a call to `exit()` in another thread or a `return` in the main thread. In such event, all threads in the process are immediately terminated, regardless of whether they are joinable or detached.

### 29.8. Thread attributes

- The attributes include information such as the location and the size of the thread's stack, the thread's scheduling policy and priority, and wether the thread is joinable or detached.

- For example, we can set detach attribute for thread at creation time, that means we don't need to call `pthread_detach()` after the creation.

```C
pthread_t thr;
pthread_attr_t attr;
int s;
s = pthread_attr_init(&attr); /* Assigns default values */
if (s != 0)
errExitEN(s, "pthread_attr_init");
s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
if (s != 0)
  errExitEN(s, "pthread_attr_setdetachstate");
s = pthread_create(&thr, &attr, threadFunc, (void *) 1);
if (s != 0)
  errExitEN(s, "pthread_create");
s = pthread_attr_destroy(&attr); /* No longer needed */
if (s != 0)
  errExitEN(s, "pthread_attr_destroy");
```

### 29.9. Thread Versus Process

- Advantages of a multithreaded approach:
  - 1. Share data bw threads is easy. By contrast, sharing data bw processes requires more work.
  - 2. Thread creation is faster than process creatrion; context-switch time may be lower for threads than for processes.

- Using threads can have some disadvantages compared to using processes:
  - 1. When programming with threads, we need to ensure that the functions we call are thread-safe or called a thread-safe manner. Multiprocess applications don't need to be concerned with this.
  - 2. A bug in one thread can damage all of the threads in the process, since they share the same address space and other attributes. By constrast, processes are more isolated from one another.
  - 3. Each thread is competing for use of the finite virtual address space of the host process. In particular, each thread's stack and thread-specific data (thread-local storage) consumes a part of the process virtual address space, which is consequently unavailable for other threads.
    - Although the available virtual address space is large, this factor may be a significant limitation for processes employing large numbers of threads or threads that require large amount of memory.
    - By contrast, separate processes can each employ the full range of available virtual memory (subject to the limitations of RAM and swap space).

- Some other points that influence our choice of threads versus processes:
  - 1. Dealing with signals in a multithreaded application requires careful design.
  - 2. In a multithreaded application, all threads must be running the same program.
  - 3. Aside from data, threads also share certain other informaion.

### 29.10. Summary

- 1. Multiple threads are concurrently executing the same program.
- 2. Threads share the same global and heap variables, but each thread has a private stack for local variables.
- ...

## 30. Threads: Thread Synchronization

- we have two basic tools that threads can use to synchronize their actions: mutexes and condition variables.
  - Mutexes allow threads to synchronize their use of a shared resource, so that, for example, one thread doesn't try to access a shared variable at the same time as another thread is modifying it.
  - Condition variables perform a complementary task: They allow threads to inform each other that a shared variable (or other shared resource) has changed state.

### 31.1. Protecting Accesses to shared variables: Mutexes

- One of the principle advantages of threads is that they can share information via global variables. However, this easy sharing comes at a cost: we must take care that multiple threads do not attemp to modify the same variables at the same time, or that one thread doesn't try to read the value of a variable while another thread is modifying it.

- The term **critical section** is used to refer to a section of code that accesses a shared resource and whose execution should be atomic; that is, its execution should not be interrupted by another thread that simultaneously accesses the same resource.

- To avoid the problems that can occur when threads try to update a shared variable, we must use a **mutex** (short for mutual execlusion) to ensure that only one thread at a time can access the variable.

- A mutex has two states: **locked** and **unlocked**. At any moment, at most one thread may hold the lock on a mutex. Attempting to lock a mutex that is already locked either blocks or fails with an error.

- When a thread locks a mutex, it becomes the **OWNER** of that mutex. Only the mutex owner can unlock the mutex.

- In general, we employ a different mutex for each shared resource (which may consist of multiple related variables), and each thread employs the following protocol for accessing a resource:
  - 1. Lock the mutex for the shared resource.
  - 2. Access the resource.
  - 3. Unlock the mutex.

- If multiple threaads try to execute this block code, the fact that only one thread can hold the mutex (the others remain blocked) means that only one thread at a time can enter the block.

#### 31.1.1. Statically Allocated Mutexes

- A mutex can either be allocated as a static variable or be created dynamically at runtime (via `malloc()`).

- Allocate static. Before it can be used, a mutex must always be initialized.

```C
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
```

#### 31.1.2. Locking and Unlocking a Mutex

- After initialization, a mutex is unlocked. We use these function to lock and unlock:

```C
#include <pthread.h>

int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

- Calling `pthread_mutex_lock()` on a locked mutex in the same thread may lead to deadlock or return an error.
- If call `pthread_mutex_unlock()` on a unlocked mutex that may lead to an error.
- If call `pthread_mutex_unlock()` on a locked mutex but in another threads that lead to undefined behavior result.

#### 31.1.3. `pthread_mutex_trylock()` and `pthread_mutex_timelock()`

- The Pthreads API provides two variants of the `pthread_mutex_lock()` function:
  - 1. `pthread_mutex_trylock()`: Try to lock, if the mutex is locked, this function return the error `EBUSY` immediately.
  - 2. `pthread_mutex_timedlock()`: called can specify an additional argument: `abstime` limit on the time that thread will sleep while waiting to acquire the mutex. It can return an error `ETIMEOUT`.

- These functions are much less frequently used than `pthread_mutex_lock()` because in most well-designed applications, a thread should hold a mutex for only a short time.

#### 31.1.4. performance of mutexes

- What is the cost of using a mutex?

#### 31.1.5. Mutex Deadlocks

- Sometimes, a thread needs to simultaneously access two or more different shared resources, each of which is governed by a separate mutex. When more than one thread is locking the same set of mutexes, deadlock situations can arise.

- The simplest way to avoid such deadlocks is to define a mutex hierarchy. When threads can lock the same set of mutexes, they should always lock them in the same order.

- Alternative strategy that is less frequenly used is **try, and then back off**.

#### 31.1.6. Dynamically initializing a Mutex

- Static initializer value `PTHREAD_MUTEX_INITIALIZER` can be used only for initializing a statically allocated mutex with default attributes. In other cases we must dynamically initialize the mutex using `pthread_mutex_init()`.

```C
#include <pthread.h>
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
```

- Among the cases where we must use `pthread_mutex_init()` rather than a static initializer are the following:
  - The mutex was dynamically allocated on the heap.
  - The mutex is an automatically varialbe allocated on the stack.
  - We want to initialize a statically allocated mutex with attributes other than defaults.

- If use it, we need to destroy the mutex:

```C
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

#### 31.1.7. Mutex types

- We made a number of statements about the behavior of mutex:
  - 1. A single thread may not lock the same mutex twice.
  - 2. A thread may not unlock a mutex that it doesn't own.
  - 3. A thread may not unlock a mutex that is not currently locked.

- What happens in each of these cases depends on the `type` of mutex:
  - 1. PTHREAD_MUTEX_NORMAL: deadlock can happens in case 1. In case 2,3 may lead to undefined behavior.
  - 2. PTHREAD_MUTEX_ERRORCHECK: All above cases will return errors, but it's slower than a normal mutex.
  - 3. PTHREAD_MUTEX_RECURSIVE: It maintain a lock count, so in case 1, the thread call multiple times, it just increase the count, the mutex only release when the count back to 0.

### 31.2. Signaling Changes of State: Condition Variables

- A mutex prevents multiple threads from accessing a shared variable at the same time. A condition variable allows one thread to inform others threads about changes in the state of a shared variable (or other shared resource) and allows the other threads to wait (block) for such notification.

- A condition variable is always used in conjunction with a mutex. The mutex provides mutual exclusion for accessing the shared variable, while the condition variable is used to signal changes in the variable's state.

#### 31.2.1. Statically Allocated Condition Variables

- Similar with mutex, you can do:

```C
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```

#### 31.2.2. Signaling and Waiting on Condition Variables

- The principle condition variable operations are **signal** and **wait**. The signal operation is a notification to one or more waiting threads that a shared variables's state has changed. The **wait** operation is the means of blocking until such a notification is received.

- The `pthread_cond_signal()` and `pthread_cond_broadcast()` functions both signal the condition variable specified by `cond`, The `pthread_cond_wait()` function blocks a thread until the condition variable `cond` is signaled.

```C
#include <pthread.h>

int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
```

- Different bw `pthread_cond_signal()` and `pthread_conf_broadcast()` lies on what happens if multiple threads are blocked in `pthread_cond_wait()`. With `pthread_cond_signal()`, we are simply guaranteed that at least one of the blocked threads is woken up; with `pthread_cond_broadcast()`, all blocked threads are woken up.

- The `pthread_cond_signal()` can be more eifficient than `pthread_cond_broadcast()`, because it **avoids** the following possibility:
  - 1. All waiting threads are awoken.
  - 2. One thread is scheduled first. This thread checks the state of the shared variable and sees that there is work to be done. The thread performs the required work, changes the state of the shared variable(s) to indicate that the work has been done, and unlocks the associated mutex.
  - 3. Each of the remaining threads in turn locks the mutex and tests the state of the shared variable. However, because of the change made by the first thread, these threads see that there is no work to be done, and so unlock the mutex and go back to sleep.
