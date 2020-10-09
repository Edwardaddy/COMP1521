// COMP1521 19t2 ... Assignment 2: heap management system

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myHeap.h"

/** minimum total space for heap */
#define MIN_HEAP 4096
/** minimum amount of space for a free Chunk (excludes Header) */
#define MIN_CHUNK 32


#define ALLOC 0x55555555
#define FREE  0xAAAAAAAA

/// Types:

typedef unsigned int  uint;
typedef unsigned char byte;

typedef uintptr_t     addr; // an address as a numeric type

/** The header for a chunk. */
typedef struct header {
	uint status;    /**< the chunk's status -- ALLOC or FREE */
	uint size;      /**< number of bytes, including header */
	byte data[];    /**< the chunk's data -- not interesting to us */
} header;

/** The heap's state */
struct heap {
	void  *heapMem;     /**< space allocated for Heap */
	uint   heapSize;    /**< number of bytes in heapMem */
	void **freeList;    /**< array of pointers to free chunks */
	uint   freeElems;   /**< number of elements in freeList[] */
	uint   nFree;       /**< number of free chunks */
};


/// Variables:

/** The heap proper. */
static struct heap Heap;
void insertFreelist(void);

/// Functions:

static addr heapMaxAddr (void);

/** Initialise the Heap. */
int initHeap (int size)
{
	if (size < MIN_HEAP) {
		size = MIN_HEAP;
	}
	if (size % 4 != 0) {
		size = size + 4 - size % 4;
	}
	Heap.heapSize = size;
	Heap.heapMem = calloc(sizeof(byte),size * sizeof(void));
	assert(Heap.heapMem != NULL);
	Heap.freeList = malloc(size/MIN_CHUNK * sizeof(void *));
	assert (Heap.freeList != NULL);
	header *chunk =(header *)Heap.heapMem;	//point to head
	chunk->status = FREE;
	chunk->size = Heap.heapSize;

	Heap.freeList[0] = chunk;
	Heap.nFree = 1;
	Heap.freeElems = Heap.heapSize/MIN_CHUNK;
	return 0;

}

/** Release resources associated with the heap. */
void freeHeap (void)
{
	free (Heap.heapMem);
	free (Heap.freeList);
}

/** Allocate a chunk of memory large enough to store `size' bytes. */
void *myMalloc (int size)
{
	/// TODO ///
	if (size < 1) {
		return NULL;
	}
	if (size % 4 != 0) {
		size = size + 4 - size % 4;
	}
	// find smallest chunk
	size = size + sizeof(header);
	int size_min = Heap.heapSize;
	int find = -1;
	
	for(int i = 0; i < Heap.nFree; i++) {
		header *curr = (header *) Heap.freeList[i];
		if (size <= curr->size && curr->size <= size_min){
			find = i;
			size_min = curr->size;
		}
	}
	if (find == -1) {
		return NULL;
	}

	addr curr = (addr ) Heap.freeList[find]; // point the chunk
	void *f = Heap.freeList[find];
	header *chunk =(header *) curr;

	chunk->status = ALLOC;
	int t = chunk->size - size;
	chunk->size = size;
	if(t != 0) {
		curr += chunk->size;
		chunk = (header *) curr;
		chunk->status = FREE;
		chunk->size = t;
	}
	insertFreelist();
	return f+sizeof(header); // this just keeps the compiler quiet
}

/** Deallocate a chunk of memory. */
void myFree (void *obj)
{
	/// TODO ///
	obj -= sizeof(header);
	header *chunk = (header *)obj;
	if (heapOffset (obj) < 0 || heapOffset (obj) > heapMaxAddr() || obj == NULL || chunk->status != ALLOC) {
		printf("Attempt to free unallocated chunk\n");
		exit(1);
	}
	chunk->status = FREE;
	// point to the current address
	addr curr = (addr)Heap.heapMem;
	// merge two chunks if they are both free
	while (curr < heapMaxAddr()) {
		chunk = (header *)curr;	
		// chunk next to the curr
		header *next = (header *)(curr + chunk->size);
		if (next->status == FREE && chunk->status == FREE) {
			chunk->size += next->size;
			memset ((void *)(curr + sizeof(header)), 0, chunk->size - sizeof(header));
			continue;
		} 
		curr += chunk->size;
	}
	insertFreelist();
	return;
}

/** Return the first address beyond the range of the heap. */
static addr heapMaxAddr (void)
{
	return (addr) Heap.heapMem + Heap.heapSize;
}

/** Convert a pointer to an offset in the heap. */
int heapOffset (void *obj)
{
	addr objAddr = (addr) obj;
	addr heapMin = (addr) Heap.heapMem;
	addr heapMax =        heapMaxAddr ();
	if (obj == NULL || !(heapMin <= objAddr && objAddr < heapMax))
		return -1;
	else
		return (int) (objAddr - heapMin);
}

/** Dump the contents of the heap (for testing/debugging). */
void dumpHeap (void)
{
	int onRow = 0;

	// We iterate over the heap, chunk by chunk; we assume that the
	// first chunk is at the first location in the heap, and move along
	// by the size the chunk claims to be.
	addr curr = (addr) Heap.heapMem;
	while (curr < heapMaxAddr ()) {
		header *chunk = (header *) curr;

		char stat;
		switch (chunk->status) {
		case FREE:  stat = 'F'; break;
		case ALLOC: stat = 'A'; break;
		default:
			fprintf (
				stderr,
				"myHeap: corrupted heap: chunk status %08x\n",
				chunk->status
			);
			exit (1);
		}

		printf (
			"+%05d (%c,%5d)%c",
			heapOffset ((void *) curr),
			stat, chunk->size,
			(++onRow % 5 == 0) ? '\n' : ' '
		);

		curr += chunk->size;
	}

	if (onRow % 5 > 0)
		printf ("\n");
}

void insertFreelist() {
	// point to the head of the heap
	addr head = (addr)Heap.heapMem;
	header *chunk;
	int n_free = 0;
	while (head < heapMaxAddr()) {
		chunk = (header *)head;
		if (chunk->status == FREE) {
			Heap.freeList[n_free] = (addr *)head;
			n_free++;
		}
		Heap.nFree = n_free;
		head += chunk->size;
	}

}