# mergesort_shared_memory
Using shared memory and parallel programming in merge sort algorithm to compare estimated sorting time between single processing and parallel processing.

This program is written in C++ and work on Linux system.
Download this code and compile using (g++ -o [program_name] [C++_file_name]), make sure that you installed build essentials. (On Unbutu : sudo apt install build-essentials)

The important part of the solution to this problem is not algorithmic, but to explain concepts of Operating System and kernel. 
To achieve concurrent sorting, we need a way to make two processes to work on the same array at the same time. To make things easier Linux provides a lot of system calls via simple API endpoints. Two of them are, shmget() (for shared memory allocation) and shmat() (for shared memory operations). We create a shared memory space between the child process that we fork. Each segment is split into left and right child which is sorted, the interesting part being they are working concurrently! The shmget() requests the kernel to allocate a shared page for both the processes.

Why traditional fork() does not work? 
The answer lies in what fork() actually does. From the documentation, “fork() creates a new process by duplicating the calling process”. The child process and the parent process run in separate memory spaces. At the time of fork() both memory spaces have the same content. Memory writes, file-descriptor(fd) changes, etc, performed by one of the processes do not affect the other. Hence we need a shared memory segment.
