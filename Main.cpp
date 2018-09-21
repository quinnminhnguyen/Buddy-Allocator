#include "Ackerman.h"
#include "BuddyAllocator.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string>
#include <iostream>
using namespace std;

void print_help(){
        printf("the command is as followed\n");
        printf( "memtest [-b <blocksize>] [-s <memsize>]\n");
        printf("-b <blocksize> is the block size, in bytes. Default is 128 bytes.\n");
        printf("-s <memsize> is the size of the memory to be allocated, in bytes. Default is 512kB\n");
}

int main(int argc, char ** argv) {

  int basic_block_size = 128, memory_length = 512 * 1024;
  int c = 0;

  while((c = getopt(argc, argv, "b:s:")) != -1){
    switch(c)
    {
      case 'b':
        //cout << "assigning block size" <<endl;
        basic_block_size = atoi(optarg); //convert from string to int
        break;
      case 's':
        //cout << "assigning memory_length" <<endl;
        memory_length = atoi(optarg);  //convert from string to int
        break;
      case '?':
        if (optopt == 'c')
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt)){
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            print_help();
        }else{
            fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
            print_help();
                    }
            return 1;
      default:
        //cout << "No command " <<endl;
        abort();
    }
  }

  
  


  //cout << "Reading input basic_block_size = " << basic_block_size <<endl;
  //cout << "Reading input memory_length = " << memory_length <<endl;
  // create memory manager
  BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);
  //cout << "Debug: BuddyAllocator initialized" <<endl;

  // test memory manager
  Ackerman* am = new Ackerman ();
  am->test(allocator); // this is the full-fledged test. 
  //allocator->debug();

  // destroy memory manager
  delete allocator;
}
