#include "ackerman.h"
#include "my_allocator.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
  
  int basic_block_size = 128;
  int length = 524288;

  char* b_val = NULL;
  char* s_val = NULL;
  
  int input_opt;
  while((input_opt = getopt(argc, argv, "b:s:")) != -1){
    switch(input_opt) {
      case 'b':
        b_val = optarg;
        basic_block_size = atoi(b_val);
        break;
      case 's':
        s_val = optarg;
        length = atoi(s_val);
        break;
      case '?':
        printf("unknown argument format, proper format: -b <block size> -s <length>");
        exit(1);
      default:
        break;
          
    }
  }
  
  init_allocator(basic_block_size, length );
  
  ackerman_main();
  
  // RECURSION TEST CODE IN RUBRIC
  /*
  PrintList();   // this should show only 1 entry in the list, the last one, the others should be all 0
  Addr addr = my_malloc(1);  
  PrintList ();   // this should everything 1, except the last entry 0
  my_free (addr);
  PrintList();	// this again should show 1 only in the last item	
  */
   
  release_allocator();
  
  
}
