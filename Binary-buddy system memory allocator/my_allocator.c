/* 
    File: my_allocator.c

    Author: Mohammad Sagor
            Department of Computer Science
            Texas A&M University
    Date  : 9-9-2016

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/


/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdlib.h> 
#include "my_allocator.h"
#include <math.h>
#include <stdio.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

node_t* head;
int global_length;
int basic_block_size;

node_t** head_array;
int size_head_array;
	
/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

// initialize allocator of size _length with bbs _basic_block_size
unsigned int init_allocator(unsigned int _basic_block_size, 
			    unsigned int _length){
    
    // exit cleanly if program aborted prematurely
    atexit(release_allocator);

	// Error check for insufficiant inputs
	if (_basic_block_size > _length){
        printf("ERROR: basic block size larger than memory size");
        exit(1);
    }
    
    double check_log = log((double)_basic_block_size) / log(2.0);
    if (check_log - int(check_log) != 0){
        printf("ERROR: basic block size not a power of 2; needs exact value");
        exit(1);
    }
    
    double check_log2 = log((double)_length) / log(2.0);
    if (check_log2 - int(check_log2) != 0){
        printf("ERROR: memory length not a power of 2; needs exact value");
        exit(1);
    }
    
	//set global variables
	global_length = _length;
	basic_block_size = _basic_block_size;
	size_head_array = find_size_head_array(basic_block_size, _length);
	
	// allocate head array
	head_array =(node_t**)malloc(sizeof(node_t*)*size_head_array);

	//Allocate main memory
	head = (node_t*)malloc((size_t)global_length);
	
	// create first node
	head->next = NULL;
	head->block_size = _length;
	head->is_full = false;
	
    // store head of allocated memory
	head_array[0] = head;
    
    
	
}

// releases memory when program exits or aborts
void release_allocator(){
	free(head);
}

// Allocate smallest memory block that can fit size _length data
extern Addr my_malloc(unsigned int _length) {
	node_t* temp_pointer;
    int proper_block_length_index = find_block_index(_length);
	
	//iterate through blocks, if no empty blocks of proper size, run block_splitter to create one
	for(int i =proper_block_length_index; i>=0; --i){
		
		if(head_array[i] != NULL) { 
			temp_pointer = head_array[i];
			
			do{ //iterate through list of same size
				if(temp_pointer->is_full){ // case: block full
					temp_pointer = temp_pointer->next;
					
				}
				else if(i == proper_block_length_index){ // case: block empty of proper size
					temp_pointer->is_full = true;
					return ((char*)temp_pointer)+sizeof(node_t);
					
				}
				else{ // case: otherwise, split larger blocks
					block_splitter(i, proper_block_length_index);
					i = proper_block_length_index+1;
					break;
					
				}
			}while(temp_pointer != NULL);
			
			
			
		}
	}
	printf("Error: not enough memory");
	exit(1);
  return NULL;
}

// Sets blocks to free, and calls merge to merge binary buddies; recursive to ensure proper merging
extern int my_free(Addr _a) {
	 
  for(int i =0; i<size_head_array; ++i){ 
	node_t* main_node = head_array[i];
	node_t* main_previous = main_node;
	
	while(main_node != NULL){ // search for main node
		
		if((char*)main_node + sizeof(node_t) == _a){ // if main node found
		    main_node->is_full = false;
			
			node_t* buddy_node = head_array[i];
			node_t* buddy_previous = buddy_node;
	
			while(buddy_node != NULL){ // search for buddy node
				
				if(binary_buddy_checker(main_node, buddy_node, main_node->block_size) && buddy_node->is_full == false){ // buddy node found and empty
					node_t* recurse_node = merge(main_node, main_previous, buddy_node, buddy_previous, i); // merge nodes
					my_free((char*)recurse_node + sizeof(node_t)); // recursive call to check if more merges need to occur
				}				
				
				buddy_previous = buddy_node;
				buddy_node = buddy_node->next;
			}
		}
		
		main_previous = main_node;
		main_node = main_node->next;
	}
	
  }
  
  return 0;
}

// Returns how many times memory can be subdivided
int find_size_head_array(int basic_block_size, int length) {
		
	double n = 0.0;
	double return_size = 1;
	while(pow(2.0,n) <= basic_block_size){
		++n;
	}
	
	while(pow(2.0,n) <= length){
		++n;
		++return_size;
	}
	return (int)return_size;
}

// find proper array position for start of my_malloc
int find_block_index(int block_length) { 
	int current_length = global_length;						
	for(int i =0; i< size_head_array; ++i){
		if((current_length/2 - sizeof(node_t)) < block_length ){ // if next smallest block is too small,
			return i; // return current block
		}
		current_length = current_length/2;
	}
	return size_head_array-1; // base case, fit into basic block size
}

//Splits blocks down until a proper sized block is created
void block_splitter(int i, int proper_block_length_index){
	for(int j = i; j<proper_block_length_index; ++j){
		node_t* temp_pointer_1 = head_array[j];
		node_t* previous_temp_pointer;
		
		int current_block_size = temp_pointer_1->block_size;
		
		
		
		// Finds last block in list and split it by moving it to the next smallest list, and removing it from its current list
		if(temp_pointer_1->next == NULL) { // if only block in list, point list to NULL
			head_array[j] = NULL;
		}
		else{
			while(temp_pointer_1->next != NULL){ // find last block in list
				previous_temp_pointer = temp_pointer_1;
				temp_pointer_1 = temp_pointer_1->next;
			}
			previous_temp_pointer->next = NULL;
		}
		
		node_t* temp_pointer_2 = head_array[j+1];
		
		if(temp_pointer_2 == NULL){ // if new list is empty, point list to new block
			head_array[j+1] = temp_pointer_1;
		}
		
		else{ // otherwise, append to end of list
			while(temp_pointer_2->next != NULL){
				temp_pointer_2 = temp_pointer_2->next;
			}
			temp_pointer_2->next = temp_pointer_1;
		}
		
		
		temp_pointer_1->block_size = current_block_size/2;
		
		// Splits new, smaller block into two blocks
		temp_pointer_1->next = (node_t*)((char*)temp_pointer_1 + temp_pointer_1->block_size);
		
		temp_pointer_2 = temp_pointer_1->next;
		
		temp_pointer_2->block_size = temp_pointer_1->block_size;
		temp_pointer_2->is_full = false;
		temp_pointer_2->next = NULL;
		
	}
}

//Print out how many blocks are free for each list
void PrintList(){
	printf("\n");
    int num_free;
	for(int i =0; i<size_head_array; ++i){
        num_free = 0;
		node_t* temp = head_array[i];
        printf("\nhead_array[%d]: ",i);
            
       while(temp != NULL){
            if(temp->is_full == false)
                ++num_free;
                
			temp = temp->next;
			
	   }
       printf("%d", num_free);
    }   
}

// Checks if two addresses are binary buddies
bool binary_buddy_checker(Addr blk1, Addr blk2, int block_size){
	
	unsigned int addr_blk1 = (unsigned int)blk1;
	unsigned int addr_blk2 = (unsigned int)blk2;
	
	unsigned int addr_head = (unsigned int)head;

	addr_blk1 = addr_blk1 - addr_head; // zero addresses
	addr_blk2 = addr_blk2 - addr_head;

	
	
	int xor_result = (addr_blk1 ^ addr_blk2);
	
	if(xor_result == block_size || xor_result == block_size*(-1))
		return true;
	return false;
}

// merges two free nodes of the same size together
node_t* merge(node_t* node1, node_t* node1_previous, node_t* node2, node_t* node2_previous, int i){
	
		
	// 7 cases for pointing around removed nodes
	if(head_array[i] == node1 && node1->next == node2){ // n1 is first, n2 is second
		head_array[i] = node2->next;
	}	
	else if(head_array[i] == node2 && node2->next == node1){ //n2 is first, n1 is second
		head_array[i] = node1->next;
	}
	else if(node1->next == node2){ // n1 is right before n2
		node1_previous->next = node2->next;
	}
	else if(node2->next == node1){ // n2 is right before n1
		node2_previous->next = node1->next;
	}
	else if (head_array[i] == node1){ // n1 is the first node
		head_array[i] = node1->next;
		node2_previous->next = node2->next;
	}
	else if (head_array[i] == node2){ // n2 is the first node
		head_array[i] = node2->next;
		node1_previous->next = node1->next;
	}
	else{ //any other case (neither are first, not next to each other)
		node1_previous->next = node1->next;
		node2_previous->next = node2->next;
	}
	
	node_t* new_node;
	
	// Merge actual nodes
	if( node1 < node2){
		node1->next = NULL;
		node1->block_size = node1->block_size * 2;
		new_node = node1;
	}
	else{
		node2->next = NULL;
		node2->block_size = node2->block_size * 2;
		new_node = node2;
	}
	
	// Place in new linked list
	node_t* temp = head_array[i-1];
	if(temp == NULL)
		head_array[i-1] = new_node;
	else{
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = new_node;
	}
	
	return new_node;

}





