#include	<stdio.h>
#include	"types.h"
#include	"proc.h"
#include	"mem_man.h"
#include	"util.h"



#define	PAGE_COUNT		65536
#define	PAGE_MASK		(PAGE_COUNT - 1)
#define	VAS_VEC_SIZE		4096
#define	VAS_VEC_SIZE_MASK	(VAS_VEC_SIZE - 1)



static	page		mem[PAGE_COUNT]	;
static	u16		page_avail 		= 0;
static	mem_manage	mem_man[PAGE_COUNT] 	= {0};
static	u16		mem_offset 		= 1;
static	u64		vas_vec[VAS_VEC_SIZE] 	= {0};
static	u32		vas_offset 		= 0;
static	u32		vas_count 		= VAS_VEC_SIZE;


// returns vas_couunt
/*
u32 get_vas_count(){
    return vas_count;
}
*/

// reads a page
void read_page (u32 vasOff, u16 memOff)
{
	u32	i;
	for (i = 0; i < 512; ++i) vas_vec[vasOff+i] = mem[memOff]._u64[i];
}

// writes a page 
void write_page (u32 vasOff, u16 memOff)
{
	u32	i;
	for (i = 0; i < 512; ++i) mem[memOff]._u64[i] = vas_vec[vasOff + i];
}

// allocates a page from the free list
u16 page_alloc()
{
	u16 t = page_avail;
	if (page_avail)	page_avail = mem[page_avail]._u16[0];
	mem[t]._u16[0]=0;
	mem_man[t]._used = 1;
	return t;
}

// Free's the page at the address given
void page_free(u16 x)
{
	mem[x]._u16[0]	= page_avail;
	page_avail = x;
}

// Creates initial free list when OS starts
void initialize_free()
{
    int i;
    for(i = 0; i < PAGE_COUNT; i++){ 
      page_free(i);
    }
}

// fills in the passed in array with unique mapping 
// Notes
/*
// size represents number of 4 meg blocks
// fill array with indices of chunks from swap space
// need to create a unique mapping between the swap space and array
// shift 6 or with bit   vas_vec[6] |= 1<<11;  11 comes back from lsb  6 is index of array,  put (6<<6)| 11 in array
// send lsb function a 64 bit number that isn't all 1's
*/
/*
int vas_alloc (u16 v[], u32 size) {
	printf("size to remove = %d\n", size);
	u32 firstZero;
	if(size > vas_count)
		return 0;
	else{
		u32 i;
		for(i = 0; i < size; i++){
			while(!(~vas_vec[vas_offset])){ 
 				vas_offset++;
				//printf("vas offset: %d\n", vas_offset);
				if(vas_offset >= 64) vas_offset = 1;
			}
			firstZero = lsb64(vas_vec[vas_offset]);
			v[i] = (vas_offset<<6) | firstZero;		// number more sequential with 5 then 6
			vas_vec[vas_offset] = 1ul << firstZero | vas_vec[vas_offset];  // 1ul instead of 1
			vas_count--;
		}
	
	return 1;
	}
}

// Send an array with it's size and it free's the addresses in the array
void vas_free (u16 v[],	u32 size) {
	printf("size to add = %d\n", size);
	u32 i;
	u32 offset = 0;
	for(i = 0; i < size; i++){
		offset = v[i] >> 6;
		vas_vec[offset] = vas_vec[offset] ^ (1ul << (v[i] & 63));
		//printf("offset: %d\n", offset);
		printf("vas_vec value: %lu\n", ~vas_vec[offset]);
		vas_count++;
	}

}
*/

u16 initialize_process(){		//not done yet, needs to walk ring if page_alloc returns 0
    u16 l1 = page_alloc();
    if(!l1){
     l1 = walk_page_ring(); 
    }
    mem_man[l1]._used = 1;
    mem_man[l1]._pinned = 1;
    return l1;
}

//  Process gives OS it's L1 table and address for lookup
u32 page_lookup(u16 pageTable, u32 address){
  
  // Breakdown Address
    u16 l1_offset = address >> 22;
    u16 l2_offset = (address >> 12) & 1024;
    u16 page_offset = address & 4096;
    
   // Find the l2 Table Address
    page l1 = mem[pageTable];
    u32 l2_address = l1._u32[l1_offset];
    //if page isn't there already
    if(!l2_address){
      //printf("no l2 page table\n");
      return 0;
      //l2_address = page_alloc();
      //mem_man[l2_address]._pinned = 1;
    }
    /* if free table was empty
    if(!l2_address){
      l2_address = walk_page_ring();
      mem_man[l2_address]._pinned = 1;
    }
    */
    
    
    // Find the data page Address
    page l2 = mem[l2_address];
    u32 data_page_address = l2._u32[l2_offset];
    // if page isn't already there
    if(!data_page_address){
      //printf("l2 page table but no data page\n");
      return 0;
     // data_page_address = page_alloc();
      //mem_man[data_page_address]._proc = pageTable;
      //mem_man[data_page_address]._l1i = l1_offset;
      //mem_man[data_page_address]._l2i = l2_offset;
    }
    /*if free table was empty
    if(!data_page_address){
      data_page_address = walk_page_ring();
    }
    */
    return 1;
    
    
}


u16 walk_page_ring(){
  while(mem_man[mem_offset]._pinned || mem_man[mem_offset]._used){
    mem_man[mem_offset]._used = 0; 
    mem_offset++;
    if(mem_offset >= 65536){
	mem_offset = 1;
    }
  }
  u16 process_l1 = mem_man[mem_offset]._proc;
  u16 process_l2 = mem[process_l1]._u32[mem_man[mem_offset]._l1i];
  
  //write to disk if dirty hereish
  
  mem[process_l2]._u32[mem_man[mem_offset]._l2i] = 0;
  mem_man[mem_offset]._used = 1;
  return mem_offset;

}

u32 page_fault(u16 pageTable, u32 address){
   // Breakdown Address
    u16 l1_offset = address >> 22;
    u16 l2_offset = (address >> 12) & 1024;
    u16 page_offset = address & 4096;
    
    // Find the l2 Table Address
    page l1 = mem[pageTable];
    u32 l2_address = l1._u32[l1_offset];
    //if page isn't there already
    if(!l2_address){
      l2_address = page_alloc();
      // if free table was empty
      if(!l2_address){
	l2_address = walk_page_ring();
      }
    mem[pageTable]._u32[l1_offset] = (l2_address | 0ul);
    mem_man[l2_address]._pinned = 1;
    }
    
    // Find the data page Address
    page l2 = mem[l2_address];
    u32 data_page_address = l2._u32[l2_offset];
    // if page isn't already there
    if(!data_page_address){
      data_page_address = page_alloc();
      //if free table was empty
      if(!data_page_address){
	data_page_address = walk_page_ring();
      }
      mem[l2_address]._u32[l2_offset] = (data_page_address | 0ul);
      mem_man[data_page_address]._proc = pageTable;
      mem_man[data_page_address]._l1i = l1_offset;
      mem_man[data_page_address]._l2i = l2_offset;
     // printf("Disk Read\n");
      disk_read(address ,data_page_address);			// don't think the address is right...
    }
    
    return 1;
  
}


#ifdef _MEM_TEST_
//only runs when compiled with flag -D _MEM_TEST_ ex. gcc -D _MEM_TEST_ mem_man.c
int main(){
  
	u32 i, j, k;
	initialize_free();
	u16 proc1, proc2, proc3;
	proc1 = initialize_process();
	printf("proc1: %d\n", proc1);
	u32 memAddress = 1 + (1 << 12) + (1 << 22);
	i = page_lookup(proc1, memAddress);
	page_fault(proc1, memAddress);
	i = page_lookup(proc1, memAddress);
	//j = page_lookup(proc1, 4198402);
	//k = page_lookup(proc1, 4198403);
	printf("page lookup: %d\n", i);
	//printf("page lookup: %d\n", j);
	//printf("page lookup: %d\n", k);
	for(i = 65535; i >0; i--){
	 if(mem_man[i]._pinned == 1){
	  printf("pinned: %d\n", i); 
	 }
	 if(mem_man[i]._used == 1){
	  printf("used: %d\n", i); 
	 }
	}
	

	
	//test page free
	/*
	printf("Page Count: %d\n", PAGE_COUNT);
	for(i = 1; i < 10; i++){
		page_free(i);
	}
	*/
	
	// test page alloc
	/*
	u16 a[40];
	u16 b[15];
	for(i = 40; i > 0; i--){
	  a[i] = page_alloc();
	  printf("allocated pages: %d\n", a[i]);
	}
	for(i = 10; i < 20; i++){
	  page_free(a[i]);
	}
	for(i = 15; i > 0; i--){
	  b[i] = page_alloc();
	  printf("allocated pages 2: %d\n", b[i]);
	}
	*/
	
	//test vas_alloc
	/*
	int success, i;
	u32 vasSizeOne = 84;
	u32 vasSizeTwo = 69;
	u16 vasOne[84] = {0};
	u16 vasTwo[69] = {0};
	success = vas_alloc(vasOne, vasSizeOne);

	success = vas_alloc(vasTwo, vasSizeTwo);

	for(i=0; i < vasSizeOne; i++){
	printf("Vas One Values: %d\n", vasOne[i]);
	}
	
	vas_free(vasOne, vasSizeOne);
	
	success = vas_alloc(vasTwo, vasSizeTwo);

	printf("\n Vas Two \n");

	for(i=0; i < vasSizeTwo; i++){
	printf("Vas Two Values: %d\n", vasTwo[i]);
	}
  */
	

	return 1;
}

#endif





