/* 
    File: my_allocator.h


#ifndef _my_allocator_h_                   // include file only once
#define _my_allocator_h_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

typedef struct node {
	struct node* next;
	int block_size;
	bool is_full;
} node_t;

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

typedef void * Addr; 

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* MODULE   MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

unsigned int init_allocator(unsigned int _basic_block_size, 
			    unsigned int _length); 
// initialize allocator of size _length with bbs _basic_block_size

void release_allocator(); 
// releases memory when program exits or aborts

Addr my_malloc(unsigned int _length); 
// Allocate smallest memory block that can fit size _length data; calls block_splitter

int my_free(Addr _a); 
// Sets blocks to free, and calls merge to merge binary buddies; recurrsive calls to ensure proper merging

int find_size_head_array(int basic_block_size, int length);
// Returns how many times memory can be subdivided

int find_block_index(int block_length);
// Returns smallest size block location that data can be entered

void block_splitter(int i, int proper_block_length_index);
// Splits larger blocks until a proper sized block is created

void PrintList();
// Prints out how many blocks are free of each size

bool binary_buddy_checker(Addr blk1, Addr blk2, int block_size);
// checks if two input addresses are binary buddies, using xor

node_t* merge(node_t* node1, node_t* node1_previous, node_t* node2, node_t* node2_previous, int i);
// Merges empty blocks that are binary buddies; calls binary_buddy_checker

#endif 
