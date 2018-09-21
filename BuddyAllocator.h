fndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;
typedef unsigned int uint;

// extend range_error from <stdexcept>
struct EmptyDLinkedListException : std::range_error {
  explicit EmptyDLinkedListException(char const* msg=NULL): range_error(msg) {}
};

struct NotEnoughMemException : std::range_error {
  explicit NotEnoughMemException(char const* msg=NULL): range_error(msg) {}
};


/* declare types as you need */

class BlockHeader{
	// decide what goes here
	// hint: obviously block size will go here
	public:
		bool isFree;	//Is the block used?
		int size;		//the size of the block
		BlockHeader *next;	//the next header
		BlockHeader (int size = 0, bool isFree = true, 
			BlockHeader *next = NULL): 
			isFree(isFree), size(size), next(next){}

		void  setBlockHeader(int size, bool isFree = true, 
			BlockHeader *next = NULL){
			this->size  = size;
		}

		void setIsFree(bool isFree){
			this->isFree = isFree;
		}
};

class LinkedList{
	// this is a special linked list that is made out of BlockHeader s. 
private:
	BlockHeader* head;		// you need a head of the list
public:
	LinkedList(): head(NULL){};	//Linked List Constructor

	void insert (BlockHeader* b){	// adds a block to the list to the front
		if(head == NULL)
		{
			head = b;
			return; 
		}
		b->next = head;
		head = b;
	}

	void remove (BlockHeader* b){  // removes a block from the list
		if(head == NULL || b == NULL)
			throw EmptyDLinkedListException("empty list or empty block");
		if(b == head){
			head = b->next;
			b->next = NULL;
			return;
		}

		BlockHeader * curr = head;
		BlockHeader * prev = head;

		while(curr != NULL){
			if (curr == b){
				prev->next = curr->next;
				b->next = NULL;
				return;
			}
			prev = curr;
			curr = curr->next;
		}
	}

	BlockHeader* getHead(){
		return head;
	}

	void setHead(BlockHeader* newHead){
		head = newHead;
	}
	
	bool findBuddy(BlockHeader* myBuddy){
		BlockHeader * curr = head;

		while(curr != NULL){
			if(curr == myBuddy)
				return true;
			curr = curr->next;
		}
		return false;
	}
	
};


class BuddyAllocator{
private:
	/* declare member variables as necessary */
	uint _basic_block_size;
	uint _total_memory_length;
	char *HeadofBlock;
	vector<LinkedList> freeList;
	
private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */
	
	char* getbuddy (char * addr); 
	// given a block address, this function returns the address of its buddy 
	
	bool isvalid (char *addr);
	// Is the memory starting at addr is a valid block
	// This is used to verify whether the a computed block address is actually correct 

	bool arebuddies (char* block1, char* block2);
	// checks whether the two blocks are buddies are not

	char* merge (char* block1, char* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	char* split (char* block);
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected

	void setFreeList(int  size);
	//Initialize Freelist variable


public:
	BuddyAllocator (uint _basic_block_size, uint _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation. The function returns the amount of 
	   memory made available to the allocator. If an error occurred, 
	   it returns 0. 
	*/ 

	~BuddyAllocator(); 
	/* Destructor that returns any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 

	char* alloc(uint _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	int free(char* _a); 
	/* Frees the section of physical memory previously allocated 
	   using ’my_malloc’. Returns 0 if everything ok. */ 
   
	void debug ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point. The output format should be the following (assuming basic block size = 128 bytes):

	128: 5
	256: 0
	512: 3
	1024: 0
	....
	....
	 which means that at point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

#endif 
