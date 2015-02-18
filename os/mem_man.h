#ifndef	_MEM_MAN__H_
#define	_MEM_MAN__H_


#include "types.h"
#include "proc.h"


typedef struct {
	u64	_used	:  1;
	u64	_dirty	:  1;
	u64	_pinned	:  1;
	u64		: 13;
	u64	_l1i	: 10;	
	u64		:  6;
	u64	_l2i	: 10;	
	u64		:  6;
	u64	_proc	: 16;
}
mem_manage;



void read_page (u32 x, u16 y);
void write_page (u32 x, u16 y);
void build_mem_avail ();
u16  page_alloc ();
void page_free (u16 x);
int  vas_alloc (u16 v[], u32 size);
void vas_free (u16 v[],	u32 size);
void initialize_free	();
u16 initialize_process	();
u32 page_lookup(u16 pageTable, u32 address);
u16 walk_page_ring ();
u32 get_vas_count();
u32 page_fault(u16 pageTable, u32 address);


#endif
