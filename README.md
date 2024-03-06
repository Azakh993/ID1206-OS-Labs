# Operating Systems Lab Assignments

This repository contains the assignments for the Operating Systems (ID1206) course offered by KTH, focusing on practical aspects of operating system functionalities, including process management, disk scheduling, concurrency, and memory management. Each lab assignment is designed to deepen the understanding of these concepts through hands-on tasks.

## Lab 1: Process Management
Introduction to process creation and communication using `fork`, `exec`, and `wait` system calls, and inter-process communication through pipes.

- **Objective**: Implement a program that demonstrates process creation and IPC.
- **Tasks**:
  - Implement `ls / | wc -l` using `fork`, `exec`, and `pipe`.
  - Implement two processes communicating through a message queue.
- **Deliverables**: `Message_Queue.c`, `Pipe.c`

## Lab 2: Concurrency and Synchronization
Exploration of thread synchronization and the reader-writer problem using mutexes and semaphores.

- **Objective**: Implement multithreading and synchronization mechanisms.
- **Tasks**:
  - Create threads to manipulate a shared buffer with mutexes.
  - Solve the reader-writer problem with semaphores.
- **Deliverables**: `Reader_Writer.c`, `Threads.c`

## Lab 3: Memory Management
Simulation of a virtual memory system, including address translation, TLB, page table management, and handling page faults.

- **Objective**: Simulate the steps involved in translating logical to physical addresses.
- **Tasks**:
  - Translate logical to physical addresses using TLB and page table.
  - Implement demand paging with a simulated backing store.
- **Deliverables**: `lab3.c`

## Lab 4: Disk Scheduling Algorithms
Implementation and comparison of various disk scheduling algorithms to understand their efficiency and behavior.

- **Objective**: Implement and evaluate different disk scheduling algorithms.
- **Tasks**:
  - Implement FCFS, SSTF, SCAN, C-SCAN, Look, and C-Look algorithms.
  - Simulate inode manipulation and understand hard and soft links.
- **Deliverables**: `CLOOK.py`, `CSCAN.py`, `FCFS.py`, `LOOK.py`, `SCAN.py`, `SSTF.py`, `main.py`

