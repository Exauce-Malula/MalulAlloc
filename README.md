# MalulAlloc - An alternative method of allocating memory:
This project is being done to attempt to learn on how memory is allocated, firstly starting with an arena. 
This allocates a large block of memory, where the programmer can provide memory to objects/variables. My arena implementation supports:
- Initalising the arena
- Allocating aligned blocks (multiples of 4) of memory to an object/variable via void pointers and casting.
- Freeing the whole arena.
- Clearing the arena by 'zeroing' it out (similar to *calloc*).

An implementation for *malloc* will be implemented at a later date. 
This memory is being requested via the syscall *mmap*, providing a virtual address to the programmer to use instantly.
