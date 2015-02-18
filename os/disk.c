#include	<stdio.h>
#include	"types.h"
#include	"util.h"
#include	"proc.h"



#define	SWAP_SIZE		((1 << 6))
#define	SWAP_SIZE_MASK	(SWAP_SIZE - 1)



static	u64		avail[SWAP_SIZE]		= {1, 0};
static	u32		offset				= 0;
static	u32		count				= SWAP_SIZE << 6;

static	u32		read_latency			= 8 << 20;
static	u32		write_latency			= 8 << 20;
static	u64		disk_time			= 0ul;



u32 get_swap_count(){
 return count; 
}

u64 get_disk_time(){
 return disk_time; 
}

int swap_alloc (u16 v[], u32 size){
	u32	i;
	u32	t;

	if (count < size) return 0;
	for (i = 0; i < size; ++i) {
		if (~(avail[offset])) {
			t				 = lsb64(avail[offset]);
			avail[offset]	|= 1ul << t;
			v[i]			 = (offset << 6) | t;
		}
		else	offset	= (offset + 1) & SWAP_SIZE_MASK;
	}
	count	-= size;
	return 1;
}




void swap_free (u16 v[], u32 size){
	u32	i;

	for (i = 0; i < size; ++i)	avail[v[i] >> 6] &= ~(1ul << (v[i] & 63));
	count	+= size;
}



//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////



u64 disk_read (u32 block, u16 addr){
	u32		delta	= rand();
	//printf("Disk Time before: %lu\n", get_disk_time());
	if (get_time() > disk_time)	disk_time	 = get_time();
	if (delta & 1)		disk_time	= (disk_time + read_latency + (delta & 0x3FFFFF)) >> 5;
	else				disk_time	= (disk_time + read_latency - (delta & 0x1FFFFF)) >> 5;
	if (block < SWAP_SIZE) {
		write_page(block, addr);
	}
	//printf("Disk Time after: %lu\n", get_disk_time());
	return disk_time;
}




u64 disk_write (u32 block, u16 addr){
	u32		delta	= rand();
	if (get_time() > disk_time)	disk_time	 = get_time();
	if (delta & 1)		disk_time	+= write_latency + (delta & 0x3FFFFF);
	else				disk_time	+= write_latency - (delta & 0x1FFFFF);
	if (block < SWAP_SIZE) {
		read_page(block, addr);
	}
	return disk_time;
}

#ifdef _DISK_TEST_
//only runs when compiled with flag -D _MEM_TEST_ ex. gcc -D _MEM_TEST_ mem_man.c
int main(){
 printf("disk time: %lu\n", get_disk_time());
 u64 time = disk_read(20, 5);
 printf("Disk Time: %lu\n", time);
 time = disk_read(20, 5);
 printf("Disk Time: %lu\n", time);
 time = disk_write(20, 5);
 printf("Disk Time: %lu\n", time);
 return 1;
}

#endif


