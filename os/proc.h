#ifndef _PROC__H_
#define _PROC__H_


/*
typedef 
struct process {
	u16		_pid;
	u32		_vas;		// Virtual Adddress Size
	dev_type	_wait_on;
	u32		_cs;		// Code Size
	u32		_ds;		// Data Size
	u32		_fs;		// File Size
	u32		_ca;		// Code Address
	u32		_da;		// Data Address
	u32		_fa;		// File Address
	u32		_ct;		// Code Time
	u32		_dt;		// Data Time
	u32		_ft;		// File Time
	u32		_nt;		// Net Time
	u32		_kt;		// kbd Time
	void		(*_cp)(u32 m, u32 t, u32* l, u32* d);	// Code Profile
	void		(*_dp)(u32 m, u32 t, u32* l, u32* d);	// Data Profile
	void		(*_fp)(u32 m, u32 t, u32* l, u32* d);	// File Profile
	void		(*_np)(u32 m, u32 t, u32* l, u32* d);	// Net Profile
	void		(*_kp)(u32 m, u32 t, u32* l, u32* d);	// kbd Profile
	u16		_fpi;		// File Pointer Index
	u64		_time;
	u32		_priority;
	struct process*	_next;
	u16		_pti;		// Page Table Index
	u16		_sbt	[1024];	// Swap Block Table
} 
process;
*/

typedef
struct process{
	struct process*	_next;			// pointer to next process
	u64		_time;			// time until ready to be moved back into ready que
	u8		_priority;		// priority of the process, 1, 2, or 3
	u16		_l1;			// Index in memory of the l1 page table of the process?
	u16		_vas_vector[128];	// not sure what this is for
	u32		_addr;  		// address of something
	u32		_ca;			// code address
	u32		_da;			// data address
	u32		_ct;			// code time
	u32		_dt;			// data time
	u32		_cs;			// code size
	u32		_ds;			// data size
	s32		_rt;			// run time
	u32		_pid;			// process ID
} process;




void		blocked_enq	(process* proc, u64 _time);			// put process in blocked que
process*	blocked_deq	();					// take process off blocked que
void		ready_enq	(process* proc);			// put process on ready que
process*	ready_deq	(u8 priority);				// take process off ready que
u64		get_time	();					// get current time
void		time_adv	(u32 delta);				// advance time by delta
void 		set_time	(u64 new_time);				// sets new time
u64 		process_exec	(u64 t, process* p);



#endif
