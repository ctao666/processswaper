/*process data structure header
 *  * Author: Chun TAO
 *   * student ID: 879010*/
#pragma once
 
typedef struct Process_s
  {
    // initial process data.
    int time_created;//the time of last in the disk(create or swap into disck)
    int process_id;
    int mem_size;
    int job_time;
    
    // status of process
    int start; // The start address of occupied memory
    int end; // The end address of occupied memory
    int remaining_time; // remaining time need to finish the job.
    int active; // if the Process is running.
    int in_mem; // if the Process is loaded into memory.
    int time_inserted_into_mem; // Value of timer when put into mem. -1 if out.
    /*
    ** We need to track both the next and previous process 
    ** e.g. when a process us deleted, we need to link the previous to the next
    ** process to each other.
    */
    struct Process_s *next;
    struct Process_s *prev;    
  } Process;


Process *create_process(int val1, int val2, int val3, int val4);
Process *read_processfile(char *targetfile, int memsize, int *num_processes);
void free_process(Process *p);


    





