/* 
    File: my_allocator.cpp
*/
#include "BuddyAllocator.h"
#include <math.h>
#include <iostream>
#include <string.h>
using namespace std;

BuddyAllocator::BuddyAllocator (uint _basic_block_size, uint _total_memory_length){
	//initialize basic block size and memory length
	//cout << "BuddyAllocator constructor " << endl;
	this->_basic_block_size = _basic_block_size;
	this->_total_memory_length = _total_memory_length;
	//cout << "input _total_memory_length = " << this->_total_memory_length <<endl;

	//debug purpose
	//cout << "_basic_block_size = " << this->_basic_block_size <<endl;
	//cout << "_total_memory_length = " << this->_total_memory_length <<endl;

	//initialize the free list
	setFreeList(log2(_total_memory_length/_basic_block_size) + 1);
	
	//initialize HeadofBlock
	char * tempHead = new char[_total_memory_length];
	//printf("tempHead: %x \n", tempHead);


	HeadofBlock = tempHead;
	//printf("HeadofBlock: %x \n", HeadofBlock);
	//put the first block into free list
	BlockHeader * firstBlock = (BlockHeader *) tempHead;

	firstBlock->setBlockHeader(_total_memory_length);
	//cout << "firstBlock size = " <<  firstBlock->size <<endl;
	freeList.at(0).insert(firstBlock);

	//Debug purpose
	/*
	if(firstBlock == freeList.at(0).getHead())
		cout << "first element inserted into free list correctly" <<endl;
	if((char *) firstBlock == HeadofBlock)
		cout << "first block == HeadofBlock" <<endl;
	*/
	//cout << endl <<endl <<endl <<endl;
}

BuddyAllocator::~BuddyAllocator (){
	delete HeadofBlock;
}

void BuddyAllocator::setFreeList(int size){
	//cout << "freeList size = " << size <<endl;
	for(int i = 0; i < size; ++i){
		LinkedList myFreeList;
		freeList.push_back(myFreeList);
	}

	//cout << "freeList size = " << freeList.size() << endl;
	return;
}

char* BuddyAllocator::alloc(uint _length) {
  /* This preliminary implementation simply hands the call over the 
     the C standard library! 
     Of course this needs to be replaced by your implementation.
  */

	uint actualNeededSize = _length + sizeof(BlockHeader);	

	//assert(_total_memory_length >= actualNeededSize);
	int nearest2Exp = ceil(log2(actualNeededSize));
	int reqAllocSize = pow(2, nearest2Exp) < _basic_block_size ? _basic_block_size : pow(2, nearest2Exp);
	int potentialIndex =  log2(_total_memory_length/reqAllocSize);
	
	//cout << "request memory from user  " << actualNeededSize << endl;
	//cout << "actual block requested " << reqAllocSize<<endl;
	for(int i = potentialIndex; i >= 0 ; --i){
		if(freeList.at(i).getHead() != NULL){
			if(freeList.at(i).getHead()->size   == reqAllocSize){

				BlockHeader * returnBlock = freeList.at(i).getHead();
				returnBlock->setIsFree(false);
				freeList.at(i).remove(returnBlock);
				return ((char*) returnBlock + sizeof(BlockHeader));
			}
			else{
				split((char*)freeList.at(i).getHead());
				i += 2;
			}
		}
	}

	//cout << "return NULL " <<endl; 	//debugging purpose
	throw NotEnoughMemException("Not enough memory");	// Crashes when required more than having
	return NULL;	//error: 
  //return new char [_length];
}

int BuddyAllocator::free(char* _a) {
  /* Same here! */
	// Insert the 
	BlockHeader * tempBlock = (BlockHeader *) (_a -  sizeof(BlockHeader));
	int indexList = log2(_total_memory_length/tempBlock->size);
	tempBlock->setIsFree(true);
	freeList.at(indexList).insert(tempBlock);

	for(int i = indexList; i >= 0; --i){
		BlockHeader * buddyBlock = (BlockHeader *) getbuddy((char *) tempBlock);
		bool isMyBuddy = arebuddies((char *) tempBlock, (char *) buddyBlock);
		if(!isMyBuddy)
			break;
		tempBlock = (BlockHeader *) merge((char *)tempBlock,(char *) buddyBlock);
	}
  return 0;
}

void BuddyAllocator::debug (){
  //Allocate mem of size 30
  char * mem = alloc(30);
  cout << "size of free list = " << freeList.size() << endl;

  //cout << "mem addr = " << mem <<endl;
  printf("mem addr = %x \n" ,mem);
  printf("real mem addr = %x \n" ,mem - sizeof(BlockHeader));
  BlockHeader * memBlock = (BlockHeader *) (mem - sizeof(BlockHeader));
  
  cout << "size of mem block = " << memBlock->size <<endl;
  if(!memBlock->isFree)
  	cout << "the 128 Byte mem block is being used" <<endl;

  printf("freeList.at(3).getHead() = %x \n" ,freeList.at(3).getHead());

  char * buddyMem = getbuddy((mem - sizeof(BlockHeader)));
  if((char *) freeList.at(3).getHead() == buddyMem)
  	cout << "block inserted correctly" <<endl;
  if(freeList.at(3).getHead()->isFree)
  	cout << "buddy is free" <<endl;
  cout << "size of buddy mem = " << freeList.at(3).getHead()->size <<endl;

  //Allocate mem1 of size 200
  cout <<endl <<endl <<endl;
  char* mem1 = alloc(200);

  printf("mem1 addr = %x \n" ,mem1);
  printf("real mem1 addr = %x \n" ,mem1 - sizeof(BlockHeader));
  BlockHeader * mem1Block = (BlockHeader *) (mem1 - sizeof(BlockHeader));
  
  cout << "size of mem block = " << mem1Block->size <<endl;
  if(!mem1Block->isFree)
  	cout << "the 256 Byte mem1 block is being used" <<endl;

  printf("freeList.at(2).getHead() = %x \n" ,freeList.at(2).getHead());

  //allocate mem2 of size 200
  cout <<endl <<endl <<endl;
  char* mem2 = alloc(200);


  printf("mem2 addr = %x \n" ,mem2);
  printf("real mem2 addr = %x \n" ,mem2 - sizeof(BlockHeader));
  BlockHeader * mem2Block = (BlockHeader *) (mem2 - sizeof(BlockHeader));

  cout << "size of mem2 block = " << mem2Block->size <<endl;
  if(!mem2Block->isFree)
  	cout << "the 256 Byte mem block is being used" <<endl;

  printf("freeList.at(2).getHead() = %x \n" ,freeList.at(2).getHead());

  char * buddyMem2 = getbuddy((mem2 - sizeof(BlockHeader)));
  if((char *) freeList.at(2).getHead() == buddyMem2)
  	cout << "block inserted correctly" <<endl;
  if(freeList.at(2).getHead()->isFree)
  	cout << "buddy2 is free" <<endl;
  cout << "size of buddy2 mem = " << freeList.at(2).getHead()->size <<endl;  

  //free mem1
  cout <<endl <<endl <<endl;
  free(mem1);
  printf("freeList.at(2).getHead() = %x \n" ,freeList.at(2).getHead());
  cout << "size of mem1 = " << freeList.at(2).getHead()->size <<endl; 
  if(freeList.at(2).getHead()->isFree)
  	cout<<"free mem1 block and inserted back to free list correctly" <<endl;

  //free mem
  cout <<endl <<endl <<endl;
  free(mem);
  printf("freeList.at(1).getHead() = %x \n" ,freeList.at(1).getHead());
  cout << "size of mem after merge = " << freeList.at(1).getHead()->size <<endl;
  if(freeList.at(3).getHead() == NULL)
  	cout <<"there is no 128 B." <<endl;
  if(freeList.at(1).getHead()->isFree)
  	cout<<"free mem block and inserted back to free list correctly" <<endl;

  //free mem2
  cout <<endl <<endl <<endl;
  free(mem2);
  for(int i = 0; i < 4; i++)
  {
  	if(i == 0){
  		printf("freeList.at(0).getHead() = %x \n" ,freeList.at(0).getHead());
  		cout << "size of mem2 after merge = " << freeList.at(0).getHead()->size <<endl;
  		if(freeList.at(0).getHead()->isFree)
  			cout << "everything is free correctly" <<endl;
  		continue;
  	}
  	cout << "current index = " << i <<endl;
  	if(freeList.at(i).getHead() == NULL)
  		cout << "remove and merge correctly at this index" <<endl;
  }

}

// given a block address, this function returns the address of its buddy 
char* BuddyAllocator::getbuddy (char * addr){
	BlockHeader *tempBlock = (BlockHeader *) addr;
	return (char*)(((addr - HeadofBlock) ^ tempBlock->size) + HeadofBlock);
}

// Is the memory starting at addr is a valid block
// This is used to verify whether the a computed block address is actually correct 
bool BuddyAllocator::isvalid (char *addr){

}

// checks whether the two blocks are buddies are not
bool BuddyAllocator::arebuddies (char* block1, char* block2){
	BlockHeader *tempBlock1 = (BlockHeader *)block1;
	BlockHeader *tempBlock2 = (BlockHeader *)block2;
	
	if(tempBlock1->size != tempBlock2->size)
		return false;

	if((tempBlock1->isFree != tempBlock2->isFree) && (tempBlock1->isFree != true))
		return false;

	int indexList = log2(_total_memory_length/tempBlock1->size);
	if(!(freeList.at(indexList).findBuddy(tempBlock1) && freeList.at(indexList).findBuddy(tempBlock2)))
		return false;
	return getbuddy(block1) == block2;

}

// this function merges the two blocks returns the beginning address of the merged block
// note that either block1 can be to the left of block2, or the other way around
char* BuddyAllocator::merge (char* block1, char* block2){ 
	BlockHeader *leftBlock = (block1 < block2) ? (BlockHeader *)block1 : (BlockHeader *)block2; 
	BlockHeader *rightBlock = (block1 > block2) ? (BlockHeader *)block1 : (BlockHeader *)block2; 

	//cout << "left block before merge = " <<leftBlock->size <<endl;
	//cout << "right block before merge = " <<rightBlock->size <<endl;
	int indexList = log2(_total_memory_length/leftBlock->size);
	freeList.at(indexList).remove(leftBlock);
	freeList.at(indexList).remove(rightBlock);

	leftBlock->size *= 2;

	indexList = log2(_total_memory_length/leftBlock->size);
	freeList.at(indexList).insert(leftBlock);
	//cout << "left block after merge = " <<leftBlock->size <<endl;
	//cout << "right block after merge = " <<rightBlock->size <<endl;

	return (char *) leftBlock;
}	

// splits the given block by putting a new header halfway through the block
// also, the original header needs to be corrected
char* BuddyAllocator::split (char* block){
	BlockHeader *tempBlock = (BlockHeader *)block;
	int indexList = log2(_total_memory_length/tempBlock->size);
	//cout << "block size before split= " << tempBlock->size <<endl;
	freeList.at(indexList).remove(tempBlock);	//remove current block from the list
	
	//get buddy block address
	BlockHeader *buddyBlock = (BlockHeader *)(block + tempBlock->size/2);
 
	tempBlock->size /= 2;
	tempBlock->isFree = true;
	
	buddyBlock->setBlockHeader(tempBlock->size);
	//cout << "block size after split = " << tempBlock->size <<endl;
	//cout << "buddy block size = " <<buddyBlock->size <<endl;
	indexList = log2(_total_memory_length/tempBlock->size);
	freeList.at(indexList).insert(tempBlock);
	freeList.at(indexList).insert(buddyBlock);

  	return (char*)buddyBlock;
}

