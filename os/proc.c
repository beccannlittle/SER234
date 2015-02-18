#include	<stdio.h>
#include	<stdlib.h>
#include	"types.h"
#include	"proc.h"
#include	"mem_man.h"



static	process*	blocked_list	= 0;;
static  process*	blocked_end	= 0;;
static	process*	ready_list_1	= 0;;
static	process*	ready_1_end	= 0;;
static	process*	ready_list_2	= 0;;
static	process*	ready_2_end	= 0;;
static	process*	ready_list_3	= 0;;
static	process*	ready_3_end	= 0;;
static	u32		proc_init_offset = 0;;
static	u64		time		= 0;;
static 	u32		finished_processes = 0;

struct	{
	u64			_start_time;
	u32			_run_time;
	u32			_code_size;
	u32			_data_size;
	u32			_priority;
} proc_init[100]	= {
					{	            10ul,	5100000,		20000000,		30000000,		1	},
					{	   10000000001ul,	6200000,		40000000,		20000000,		2	},
					{	   10000000200ul,	7300000,		60000000,		10000000,		3	},
					{	   80000000013ul,	8400000,		80000000,		90000000,		1	},
					{	   80000000014ul,	9500000,		10000000,		80000000,		2	},
					{	   80000000015ul,	5600000,		20000000,		70000000,		3	},
					{	  200000000016ul,	6700000,		30000000,		60000000,		1	},
					{	  200000000107ul,	7800000,		40000000,		50000000,		2	},
					{	  200000000801ul,	8900000,		50000000,		40000000,		3	},
					{	  200000009001ul,	9600000,		60000000,		30000000,		1	},
					{	  800000000000ul,	5500000,		70000000,		20000000,		2	},
					{	  800000000001ul,	6400000,		80000000,		10000000,		3	},
					{	  800000000002ul,	7300000,		90000000,		50000000,		1	},
					{	  800000000003ul,	8200000,		10000000,		40000000,		2	},
					{	  800000000004ul,	9000000,		20000000,		30000000,		3	},
					{	 1200000000025ul,	5400000,		30000000,		20000000,		1	},
					{	 1200000000026ul,	6000000,		40000000,		10000000,		2	},
					{	 1200000000027ul,	7666660,		50000000,		60000000,		3	},
					{	 1200000000028ul,	8400000,		60000000,		30000000,		1	},
					{	 1200000000029ul,	9200000,		70000000,		10000000,		2	},
					{	 1200000000010ul,	5000000,		20000000,		30000000,		1	},
					{	 2000000000011ul,	6100000,		40000000,		20000000,		2	},
					{	 2000000000102ul,	7200000,		60000000,		10000000,		3	},
					{	 2000000000301ul,	8300000,		80000000,		90000000,		1	},
					{	 2000000000401ul,	9400000,		10000000,		80000000,		2	},
					{	 2000000050001ul,	5500000,		20000000,		70000000,		3	},
					{	 2000000060001ul,	6600000,		30000000,		60000000,		1	},
					{	 2000000070001ul,	7700000,		40000000,		50000000,		2	},
					{	 3000000000018ul,	8800000,		50000000,		40000000,		3	},
					{	 3000000000019ul,	9900000,		60000000,		30000000,		1	},
					{	 3000000000020ul,	5000000,		70000000,		20000000,		2	},
					{	 3000000000021ul,	6900000,		80000000,		10000000,		3	},
					{	 3000000000022ul,	7800000,		90000000,		50000000,		1	},
					{	 3000000000023ul,	8700000,		10000000,		40000000,		2	},
					{	 3000000000024ul,	9600000,		20000000,		30000000,		3	},
					{	 3000000000025ul,	5500000,		30000000,		20000000,		1	},
					{	 5000000001026ul,	6400000,		40000000,		10000000,		2	},
					{	 5000000002027ul,	7300000,		50000000,		60000000,		3	},
					{	 5000000003028ul,	8200000,		60000000,		30000000,		1	},
					{	 5000000004029ul,	9100000,		70000000,		10000000,		2	},
					{	 5000000005010ul,	5000000,		20000000,		30000000,		1	},
					{	 5000000006011ul,	6600000,		40000000,		20000000,		2	},
					{	 5000000007012ul,	7500000,		60000000,		10000000,		3	},
					{	 5000000008013ul,	8400000,		80000000,		90000000,		1	},
					{	 5000000009014ul,	9300000,		10000000,		80000000,		2	},
					{	 7000000000005ul,	5200000,		20000000,		70000000,		3	},
					{	 7000000000016ul,	6100000,		30000000,		60000000,		1	},
					{	 7000000000118ul,	8000000,		50000000,		40000000,		3	},
					{	 7000000001019ul,	9600000,		60000000,		30000000,		1	},
					{	 7000000010020ul,	5500000,		70000000,		20000000,		2	},
					{	 7000000020021ul,	6400000,		80000000,		10000000,		3	},
					{	 7000000030022ul,	7300000,		90000000,		50000000,		1	},
					{	 7000000040023ul,	8200000,		10000000,		40000000,		2	},
					{	 7000000050017ul,	7100000,		40000000,		50000000,		2	},
					{	 7000000060024ul,	9000000,		20000000,		30000000,		3	},
					{	 9000000000025ul,	5600000,		30000000,		20000000,		1	},
					{	 9000000000026ul,	6500000,		40000000,		10000000,		2	},
					{	 9000000000027ul,	7400000,		50000000,		60000000,		3	},
					{	 9000000000028ul,	8300000,		60000000,		30000000,		1	},
					{	 9000000000029ul,	9200000,		70000000,		10000000,		2	},
					{	 9000000000110ul,	5100000,		20000000,		30000000,		1	},
					{	 9000000000111ul,	6000000,		40000000,		20000000,		2	},
					{	 9000000000112ul,	7600000,		60000000,		10000000,		3	},
					{	 9000000000113ul,	8500000,		80000000,		90000000,		1	},
					{	 9000000000114ul,	9400000,		10000000,		80000000,		2	},
					{	 9000000000115ul,	5300000,		20000000,		70000000,		3	},
					{	11000000001016ul,	6200000,		30000000,		60000000,		1	},
					{	11000000002017ul,	7100000,		40000000,		50000000,		2	},
					{	11000000003018ul,	8000000,		50000000,		40000000,		3	},
					{	11000000010019ul,	9300000,		60000000,		30000000,		1	},
					{	11000000020020ul,	5000000,		70000000,		20000000,		2	},
					{	11000000030021ul,	6600000,		80000000,		10000000,		3	},
					{	11000000040022ul,	7200000,		90000000,		50000000,		1	},
					{	11000000050023ul,	8400000,		10000000,		40000000,		2	},
					{	11000000060024ul,	9100000,		20000000,		30000000,		3	},
					{	11000000070025ul,	5000000,		30000000,		20000000,		1	},
					{	11000000080026ul,	6000000,		40000000,		10000000,		2	},
					{	11000000090027ul,	7000000,		50000000,		60000000,		3	},
					{	14000000003028ul,	8000000,		60000000,		30000000,		1	},
					{	14000000004029ul,	9900000,		70000000,		10000000,		2	},
					{	14000000005010ul,	5800000,		20000000,		30000000,		1	},
					{	14000000006011ul,	6600000,		40000000,		20000000,		2	},
					{	14000000010012ul,	7300000,		60000000,		10000000,		3	},
					{	14000000020013ul,	8000000,		80000000,		90000000,		1	},
					{	14000000030014ul,	9000000,		10000000,		80000000,		2	},
					{	14000000040015ul,	5000000,		20000000,		70000000,		3	},
					{	14000000050016ul,	6000000,		30000000,		60000000,		1	},
					{	14000000060017ul,	7600000,		40000000,		50000000,		2	},
					{	14000000070018ul,	8300000,		50000000,		40000000,		3	},
					{	14000000080019ul,	9300000,		60000000,		30000000,		1	},
					{	14000000090020ul,	5600000,		70000000,		20000000,		2	},
					{	15000000010021ul,	6300000,		80000000,		10000000,		3	},
					{	15000000020022ul,	7000000,		90000000,		50000000,		1	},
					{	15000000030023ul,	8000000,		10000000,		40000000,		2	},
					{	15000000040024ul,	9000000,		20000000,		30000000,		3	},
					{	15000000050025ul,	5100000,		30000000,		20000000,		1	},
					{	15000000060026ul,	6300000,		40000000,		10000000,		2	},
					{	15000000070027ul,	7500000,		50000000,		60000000,		3	},
					{	15000000080028ul,	8300000,		60000000,		30000000,		1	},
					{	15000000090029ul,	9400000,		70000000,		10000000,		2	}
				};
				
				
/*
struct	{
	u64			_start_time;
	u32			_run_time;
	u32			_code_size;
	u32			_data_size;
	u32			_priority;
} proc_init[20]	= {
			{	10ul,	500000,		20000000,	30000000,	1	},
			{	11ul,	600000,		40000000,	20000000,	2	},
			{	12ul,	700000,		60000000,	10000000,	3	},
			{	13ul,	800000,		80000000,	90000000,	1	},
			{	14ul,	900000,		10000000,	80000000,	2	},
			{	15ul,	500000,		20000000,	70000000,	3	},
			{	16ul,	600000,		30000000,	60000000,	1	},
			{	17ul,	700000,		40000000,	50000000,	2	},
			{	18ul,	800000,		50000000,	40000000,	3	},
			{	19ul,	900000,		60000000,	30000000,	1	},
			{	20ul,	500000,		70000000,	20000000,	2	},
			{	21ul,	600000,		80000000,	10000000,	3	},
			{	22ul,	700000,		90000000,	50000000,	1	},
			{	23ul,	800000,		10000000,	40000000,	2	},
			{	24ul,	900000,		20000000,	30000000,	3	},
			{	25ul,	500000,		30000000,	20000000,	1	},
			{	26ul,	600000,		40000000,	10000000,	2	},
			{	27ul,	700000,		50000000,	60000000,	3	},
			{	28ul,	800000,		60000000,	30000000,	1	},
			{	29ul,	900000,		70000000,	10000000,	2	}
		};
		*/

// Enqueue in blocked list
void blocked_enq (process* proc, u64 _time){
  proc->_time = _time;
  process* current;
  
  if(blocked_list == 0){
   // printf("Empty list, store at front\n");
    blocked_list = proc;
    blocked_end = proc;    
  }
  else{
    if(proc->_time < blocked_list->_time)
    {
      //printf("smaller then first in list, put at front\n");
      proc->_next = blocked_list;
      blocked_list = proc;
      //printf("%lu\n", blocked_end->_time);
      
    }
    else if(proc->_time >= blocked_end->_time)
    {
     //printf("larger then last in list, put at end\n");
     blocked_end->_next = proc;
     blocked_end = proc;
    }
    else{
     current = blocked_list;
     //printf("somewhere in middle, find it\n");
     while(proc->_time > current->_next->_time){
       current = current->_next;
     }
     proc->_next = current->_next;
     current->_next = proc;
    }
    
  }
  
}

// Dequeue in blocked list
process* blocked_deq (){
  process* temp;
  if(blocked_list != 0 && blocked_list != blocked_end){
    temp = blocked_list;
    blocked_list = blocked_list->_next;
  }
  else if(blocked_list == blocked_end){
    temp = blocked_list;
    blocked_list = 0;
  }
  else
    temp = 0;
  return temp;
  
}

//Enqueue for priority one processes
void priority_one_enq (process* proc){
  if(ready_list_1 == 0){
  ready_list_1 = proc;
  ready_1_end = proc;    
  }
  else{
   ready_1_end->_next = proc;
   ready_1_end = proc;
  }
  
}

//Enqueue for priority two processes
void priority_two_enq (process* proc){
  if(ready_list_2 == 0){
  ready_list_2 = proc;
  ready_2_end = proc;    
  }
  else{
   ready_2_end->_next = proc;
   ready_2_end = proc;
  }
  
}

//Enqueue for priority three processes
void priority_three_enq (process* proc){
  if(ready_list_3 == 0){
  ready_list_3 = proc;
  ready_3_end = proc;    
  }
  else{
   ready_3_end->_next = proc;
   ready_3_end = proc;
  }
  
}

//Enqueue in ready list by built in priority
void ready_enq (process* proc){
  switch(proc->_priority){
   case 1:
      priority_one_enq(proc);
   break;
   
   case 2:
      priority_two_enq(proc);
   break;
   
   case 3:
     priority_three_enq(proc);
   break;
   
   default:
     printf("Priority not set\n");
   break; 
    
  }
}

//Dequeue for priority one process
process* priority_one_deq(){
  process* temp;
    if(ready_list_1 != 0 && ready_list_1 != ready_1_end){
    temp = ready_list_1;
    ready_list_1 = ready_list_1->_next;
   }
  else if(ready_list_1 == ready_1_end){
    temp = ready_list_1;
    ready_list_1 = 0;
  }
  else
    temp = 0;

   return temp; 
}

//Dequeue for priority two process
process* priority_two_deq(){
  process* temp;
    if(ready_list_2 != 0 && ready_list_2 != ready_2_end){
    temp = ready_list_2;
    ready_list_2 = ready_list_2->_next;
   }
  else if(ready_list_2 == ready_2_end){
    temp = ready_list_2;
    ready_list_2 = 0;
  }
  else
    temp = 0;

   return temp; 
}

//Dequeue for priority three process
process* priority_three_deq(){
  process* temp;
    if(ready_list_3 != 0 && ready_list_3 != ready_3_end){
    temp = ready_list_3;
    ready_list_3 = ready_list_3->_next;
   }
  else if(ready_list_3 == ready_3_end){
    temp = ready_list_3;
    ready_list_3 = 0;
  }
  else
    temp = 0;

   return temp; 
}

//Dequeue from the ready list based on priority
process* ready_deq (u8 priority){
  process* temp;
  switch(priority){
   case 1:
      temp = priority_one_deq();
   break;
   
   case 2:
      temp = priority_two_deq();
   break;
   
   case 3:
      temp = priority_three_deq();
   break;
   
   default:
     printf("Not a valid priority\n");
     temp = 0;
   break; 
  }
   
  return temp;
}

// Clear processes that are dont based on time
void clear_blocked(){
    while(blocked_list && blocked_list->_time <= get_time()){
    ready_enq(blocked_deq()); 
    }
}

// run four high priority processes
void execute_high(){
  u32 i = 0;
  u64 time_till_ready;
  u64 time_started;
  u64 time_to_run;
  process* p;
  while(ready_list_1 != 0 && i <4){
   time_to_run = get_time() + 20000;
   p = priority_one_deq();
   time_started = get_time();
   time_till_ready = process_exec(time_to_run, p); //->_ca, p->_ct, p->_cs, p->_da, p->_dt, p->_ds, p->_l1);
   //printf("run time remaining: %d\n", p->_rt);
   p->_rt = p->_rt - (get_time()- time_started);
   //printf("run time remaining: %d\n", p->_rt);
   if(p->_rt <= 0){
    printf("Process %d with Priority %d Finished at %lu\n", p->_pid, p->_priority,  get_time());
    swap_free(p->_vas_vector, (p->_cs + p->_ds)>>22);	
    finished_processes++;
   }
   else if(time_till_ready == time_to_run){
    set_time(time_till_ready);
    //printf("process back in ready: %lu\n", time_till_ready);
    priority_one_enq(p); 
   }
   else if(time_till_ready > 0){
    blocked_enq(p, time_till_ready); 
   }
    i++;
  }
}

// run two medium priority processes
void execute_medium(){
  u32 i = 0;
  u64 time_till_ready;
  u64 time_started;
  u64 time_to_run;
  process* p;
  while(ready_list_2 != 0 && i <2){
    time_to_run = get_time() + 20000;
   p = priority_two_deq();
   time_started = get_time();
   time_till_ready = process_exec(time_to_run, p);
    p->_rt = p->_rt - (get_time()- time_started);
   
   if(p->_rt <= 0){
    printf("Process %d with Priority %d Finished at %lu\n", p->_pid, p->_priority,  get_time());
    swap_free(p->_vas_vector, (p->_cs + p->_ds)>>22);
    finished_processes++;
   }
   else if(time_till_ready == time_to_run){
    set_time(time_till_ready);
    priority_two_enq(p); 
   }
   else if(time_till_ready > 0){
    blocked_enq(p, time_till_ready); 
   }
    i++;
  }  
}

// run one low priority processes
void execute_low(){
  u32 i = 0;
  u64 time_till_ready;
  u64 time_started;
  u64 time_to_run;
  process* p;
  while(ready_list_3 != 0 && i <1){
    time_to_run = get_time() + 20000;
   time_started = get_time();
   p = priority_three_deq();
   time_till_ready = process_exec(time_to_run, p);
   p->_rt = p->_rt - (get_time()- time_started);
   
   if(p->_rt <= 0){
    printf("Process %d with Priority %d Finished at %lu\n", p->_pid, p->_priority,  get_time()); 
    swap_free(p->_vas_vector, (p->_cs + p->_ds)>>22);
    finished_processes++;
   }
   else if(time_till_ready == time_to_run){
    set_time(time_till_ready);
    priority_three_enq(p); 
   }
   else if(time_till_ready > 0){
    blocked_enq(p, time_till_ready); 
   }
    i++;
  }  
}

// return a new code time
u64 new_code_time (){
	return	50 + (rand() & 0xfff);
}

// return a new data time
u64 new_data_time (){
	return	100 + (rand() & 0x1fff);
}

// return a new code address
u32 new_code_addr (u32 addr, u32 limit){
	static u32	x[32]	= {	 1,	 1,	 1,	 1,	 1,	 1,	 1,	 1, 1,	 1,	 1,	 1,	 1,	 1,	 1,	 1, 
							 2,	 2,	 2,	 2,	 2,	 2,	 2,	 2, 3,	 3,	 3,	 3,	 4,	 4,	 8,	16};

	u32	r = rand();
	addr	=  (r & 64) ? addr + x[r & 31] : addr - x[r & 31];
	return (addr > limit) ? addr = r % limit : addr;
}

// return a new data address
u32 new_data_addr (u32 addr, u32 base, u32 limit){
	static u32	x[32]	= {	 1,	 1,	 1,	 1,	 2,	 2,	 2,	 2,	 3,	 3,	 3,	 3,	 4,	 4,	 4,	 4, 
							 5,	 5,	 6,	 6,	 7,	 7,	 8,	 8,	 9,	10,	11,	12,	16,	20,	28,	40};

	u32	r = rand();
	addr	=  (r & 64) ? addr + x[r & 31] : addr - x[r & 31];
	return ((base < addr) || (addr > limit)) ? addr = base + (r % (limit - base)) : addr;
}

//initialize new processes
void initialize_processes(){
  process* p = malloc(100 * sizeof(process));
 while(proc_init[proc_init_offset]._start_time < get_time() && (proc_init[proc_init_offset]._code_size + proc_init[proc_init_offset]._data_size) >>22 < get_swap_count()){
  printf("Process Starting: %d\n", proc_init_offset+1);
   swap_alloc(p[proc_init_offset]._vas_vector ,(proc_init[proc_init_offset]._code_size + proc_init[proc_init_offset]._data_size) >> 22);
  p[proc_init_offset]._rt = proc_init[proc_init_offset]._run_time;
  p[proc_init_offset]._cs = proc_init[proc_init_offset]._code_size;
  p[proc_init_offset]._ds = proc_init[proc_init_offset]._data_size;
  p[proc_init_offset]._priority = proc_init[proc_init_offset]._priority;
  p[proc_init_offset]._l1 = initialize_process();
  p[proc_init_offset]._ct = new_code_time();
  p[proc_init_offset]._dt = new_data_time();
  p[proc_init_offset]._ca = new_code_addr(0, proc_init[proc_init_offset]._code_size);	
  p[proc_init_offset]._da = new_data_addr(0, proc_init[proc_init_offset]._code_size, proc_init[proc_init_offset]._data_size);
  p[proc_init_offset]._pid = proc_init_offset+1;
  if(p[proc_init_offset]._priority == 1){
   priority_one_enq(&p[proc_init_offset]); 
  }
  else if(p[proc_init_offset]._priority == 2){
   priority_two_enq(&p[proc_init_offset]); 
  }
  else if(p[proc_init_offset]._priority == 3){
   priority_three_enq(&p[proc_init_offset]); 
  }
  else{
   printf("Error enqueuing into correct priority queue\n"); 
  }
  proc_init_offset++;
 }
}

void scheduler (){
  while(1){
    //printf("Initialize\n");
    initialize_processes();
    //printf("Clear Blocked\n");
    clear_blocked();
    //printf("Execute High\n");
    execute_high();
    //printf("Execute Medium\n");
    execute_medium();
    //printf("Execute Low\n");
    execute_low();
    //printf("Advance Time\n");
    if(finished_processes >= 100){
      printf("All Processes Finished after: %lu\n", get_time());
      break;
    }
    time_adv(1000000);
  } 
}

// gets the current system time
u64 get_time(){
  return time;
}

// advances the system time by input
void time_adv (u32 delta){
  time = time + delta;
}

// sets the system time
void set_time(u64 new_time){
  time = new_time;
}


// execute the process
u64 process_exec(u64 t, process* p){
	u32	code_addr	=	p->_ca;
	u32	code_time	= 	p->_ct;
	u32	code_limit	=	p->_cs;
	u32	data_addr	=	p->_da;
	u32	data_time	=	p->_dt;
	u32	data_limit	=	p->_ds;
	u16	l1		=	p->_l1;
	u32	i;
	u32	code_trans	= page_lookup(l1, code_addr);
	u32	data_trans	= page_lookup(l1, data_addr);
	u32 	check = 0;
	
	if (!code_trans) {
		page_fault(l1, code_addr);
	}
	

	if (!data_trans) {
		page_fault(l1, data_addr);
		check++;
	}
	

	while (1) {
		u32	t_t_t	= t - get_time(); //time_till_timer
		if (code_time < data_time) {
			if (code_time > t_t_t) {
				code_time -= t_t_t;
				data_time -= t_t_t;
				return t;	//returns the value of how long it ran
			}
			set_time (get_time() + code_time);
			data_time  -= code_time;
			code_addr	= new_code_addr(code_addr, code_limit);
			code_time	= new_code_time();
			code_trans	= page_lookup(l1, code_addr);
			if (!code_trans) {
				page_fault(l1, code_addr);
				return get_disk_time();
			}
		}
		else if (data_time > t_t_t) {
			code_time -= t_t_t;
			data_time -= t_t_t;
			return t;
		}
		else {
			set_time (get_time() + data_time);
			code_time  -= data_time;
			data_addr	= new_data_addr(data_addr, code_limit, data_limit); 
			data_time	= new_data_time();
			data_trans	= page_lookup(l1, data_addr); 
			if (!data_trans) {
				page_fault(l1, data_addr);
				return get_disk_time();
			}
		}
	}
}


#ifdef _PROC_TEST_
//only runs when compiled with flag -D _MEM_TEST_ ex. gcc -D _MEM_TEST_ mem_man.c
int main(){
 initialize_free();
 printf("Run Scheduler\n");
 scheduler();
  
  
  /*
 initialize_processes();
 printf("%d\n", ready_deq(1)->_rt);
 printf("%d\n", ready_deq(2)->_rt);
 printf("%d\n", ready_deq(3)->_rt);
 printf("%d\n", ready_deq(1)->_rt);
 printf("%d\n", ready_deq(2)->_rt);
 printf("%d\n", ready_deq(3)->_rt);
 printf("%d\n", ready_deq(1)->_rt);
 printf("%d\n", ready_deq(2)->_rt);
 printf("%d\n", ready_deq(3)->_rt);
 printf("%d\n", ready_deq(1)->_rt);
 printf("%d\n", ready_deq(2)->_rt);
 printf("%d\n", ready_deq(3)->_rt);
 printf("%d\n", ready_deq(1)->_rt);
 printf("%d\n", ready_deq(2)->_rt);
 printf("%d\n", ready_deq(3)->_rt);
 printf("%d\n", ready_deq(1)->_rt);
 printf("%d\n", ready_deq(2)->_rt);
 printf("%d\n", ready_deq(3)->_rt);
 printf("%d\n", ready_deq(1)->_rt);
 printf("%d\n", ready_deq(2)->_rt);
*/
  
  
  
  /*
 process p1, p2, p3, p4, p5, p6;
 p1._priority = 1;
 p2._priority = 1;
 p3._priority = 1;
 p4._priority = 1;
 p5._priority = 1;
 p6._priority = 1;
 
 //Blocked Queue test
 
 blocked_enq(&p1, 5);
 blocked_enq(&p2, 10);
 blocked_enq(&p3, 4);
 blocked_enq(&p4, 3);
 blocked_enq(&p5, 8);
 blocked_enq(&p6, 2);
 
 time_adv(6);
 
 clear_blocked();
 
 
 printf("%lu\n", blocked_deq()->_time);
 printf("%lu\n", blocked_deq()->_time);
 printf("%lu\n", ready_deq(1)->_time);
 printf("%lu\n", ready_deq(1)->_time);
 printf("%lu\n", ready_deq(1)->_time);
 printf("%lu\n", ready_deq(1)->_time);
*/
  
  
 return 1; 
}
#endif



