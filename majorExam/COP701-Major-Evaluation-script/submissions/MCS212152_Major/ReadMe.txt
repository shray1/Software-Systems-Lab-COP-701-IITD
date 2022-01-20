Explanation :: 

A SET contains number of BLOCKs.  

Now, a memory address bits are divided into three parts -> 

|<---------------------MemoryAddressBits------------------>|

|<--------TagNo--------->|<--Setoffset-->|<--BlockOffset-->|  


Now, each memory address block can be stored into any BLOCK of its mapped SET. 


MemoryBits = 32 (Given)
NumOfSet , NumOfBlocks, BlockSize ,NumOfSets are given to us. 

So we can calucate the following ,

BlockSizeBits = log2(iBlockSize)

SetOffsetBits = log2(iNumOfSets)

TagBits = MemoryBits -  (BlockSizeBits + SetOffsetBits)


And now, we can have a 2D Array of Caches. 
Cache[SET][BLOCK] =  "TagNumber" 

where "TagNumber" is the decimal number formed out of the tag bits of a memory address. 

Case of FIFO :: 
 In this case, we can create a an updateCounterIndex, which will store the index of BLOCK to be updated in the cache. Since in case of FIFO, the replacement policy is cyclic in nature. 

Case of LFU :: 
 In this case, we keep another 2D array to store the frequency related to the BLOCK in a SET of a cache. This helps us in evaluating the minimal used BLOCK to be replaced. 
 (In case of tie, FIFO order resolves the tie).  {We could have used a more complex data structure, a min heap to retrive the index of the minimum updated BLOCK in less time.}

Case of LRU :: 
 In this, we keep a global time counter (initialised to 0). Then for each instruction we update this counter and keep a store for each BLOCK in SET  of cache, an associated used time. Thus by iterating over all BLOCKs in a SET, we find out the least recently used block
 as this block will have the minimum value of time associated to it. 


Note : I have used simple array traversal for searching the tagNumber. But this could have been optimised using a data structure called HashMap. For such optimisation, I would need to create a hashMap data structure and store the tagNumbers associated with the index of the BLOCK in Cache. 






CYCLE COUNTS :: 

To calculate cycle counts, we observe that for each memory instruction, 
we will incur the cache cost of 1 cycle. 
Now, we incur the memory cost of 100 cycles in the following conditions ::

a) In case of a Cache Miss. 
b) In case of a value written to cache, but not updated in memory. (Hence, whenever these
		values are removed from cache we have to incur this special cost of updating our memory.)

