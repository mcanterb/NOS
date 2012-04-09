

/**
* Malloc for the kernel. All Ring 0 processes can use it, including drivers.
* Returns a pointer to the allocated memory. Returns null if it cannot 
* allocate the amount of memory requested.
*/
void* kmalloc(unsigned int size);

/**
* Free for the kernel. All Ring 0 processes can use it, including drivers.
* Frees memory allocated by kmalloc at the specified address. If memory cannot
* be freed safely, nothing happens. Freeing memory that is still in use will
* likely result in Page Faults.
*/
void kfree(void *ptr);

/**
* Initializes the structures neccessary for kmalloc to function.
* Is called by MMInit().
*/
void InitKMalloc();