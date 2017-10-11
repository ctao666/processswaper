/*memory
 *  *  * Author: Chun TAO
 *   *   * student ID: 879010*/
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "memory.h"

/*
** Creates the "memory" as it were. Holes aren't dynamically tracked but
** are easily calculated from the space between each process (or between
** a process and the start/end of memory).
*/
Memory *create_memory(int mem_size) 
  {
    Memory *mem = malloc(sizeof(Memory));
    assert(mem);
    mem->start = 0;
    mem->end = mem_size;// set the memory address from "0" to "mem_size" 
    mem->processes = NULL;
    mem->num_processes = 0;
    mem->num_holes = 0;
    return mem;
  }

/*
** Insert the process into memory with first/best/worst fit algorthms
** Returns 1 on success, 0 on failure. If failure we use memory_remove_oldest.
*/
int memory_insert(Memory *mem, Process *in, int timer, char *alg)
  {
    // If there are no items in memory yet.
    if (mem->processes == NULL) 
      {
        mem->processes = in;//mark this process as the first process in the memory
        in->start = mem->end - in->mem_size;
        in->end = mem->end;
        in->next = NULL; 
        in->prev = NULL;
        mem->num_processes += 1;
        in->in_mem = 1;
        in->time_inserted_into_mem = timer;
        return 1;
      }
   
    if (!strcmp(alg,"first"))//first fir algorithm
      {
        Process *curr = mem->processes;//memory pointer used to search the porcess within the memory
        // Checking for space between first process and highest address of memormry.
        if ((mem->end - in->mem_size) >= (curr->end)) 
          {// engough for the insterted process
            in->prev = NULL;
            in->next = mem->processes;
            mem->processes = in;// first process pointer move to inserted process
            curr->prev = in;// link the first 2 processes
            in->start = mem->end - in->mem_size;
            in->end = mem->end;//load the process at the highest address of memory
            mem->num_processes += 1;
            in->in_mem = 1;//mark the process is into memory
            in->time_inserted_into_mem = timer;//note the time of loading into memory
            return 1;
          }
        // loop of checking for space between each process from the highest memory address.
        while (curr->next != NULL) 
          {
            // //check if the size between current and next is large engough. If so, load the process "in" in that space.
            if ((curr->start - in->mem_size) >= curr->next->end) 
              {
                in->start = curr->start - in->mem_size;
                in->end = curr->start;
                curr->next->prev = in;
                in->next = curr->next;
                curr->next = in;
                in->prev = curr;
                mem->num_processes += 1;
                in->in_mem = 1;
                in->time_inserted_into_mem = timer;
                return 1;
              }
            curr = curr->next;
          }
        // Checking for space between lowest address of memory and last process.(similar insert procedure )
        if ((curr->start - in->mem_size) >= mem->start) 
          {
            in->start = curr->start - in->mem_size;
            in->end = curr->start;
            curr->next = in;
            in->prev = curr;
            in->next = NULL;
            mem->num_processes += 1;
            in->in_mem = 1;
            in->time_inserted_into_mem = timer;
            return 1;
          }
      }

    if (!strcmp(alg,"best"))//best fit algorithm
      {
        Process *curr = mem->processes;//memory pointer used to search the porcess within the memory
        int smallest=mem->end; 
        Process *pointer=NULL;
        int head=0; // indictor whether the wanted space is before or after the first existing process in memory
        // Checking for space between first process and highest address of memormry.
        if ((mem->end - in->mem_size) >= (curr->end)) 
          {// engough for the insterted process
            smallest=mem->end-curr->end;// temporarily set the smallest available free space so far
            pointer=curr;  
            head=1;     
          }
        while (curr->next!= NULL) 
          { // loop of checking for space between each process from the highest memory address. and check whether the space 
            // is smaller.
            if ((curr->start - in->mem_size) >= curr->next->end && pointer==NULL)
              {
                smallest=curr->start-curr->next->end;// temporarily set the smallest available free space so far
                pointer=curr; 
                if(head==1)
                  {
                    head=0;     
                  }  
              }       
            if ((curr->start - in->mem_size) >= curr->next->end && (curr->start-curr->next->end)< smallest) 
              {
                smallest = curr->start-curr->next->end;
                pointer =curr;
                if(head==1)
                  {
                    head=0;     
                  } 
              }        
            curr=curr->next;              
          }      
        if ((curr->start - in->mem_size) >= mem->start && (curr->start-mem->start<smallest|| pointer==NULL)) 
          {      
            // Checking for space between lowest address of memory and last process.
            in->start = curr->start - in->mem_size;
            in->end = curr->start;
            curr->next = in;
            in->prev = curr;
            in->next = NULL;
            mem->num_processes += 1;
            in->in_mem = 1;
            in->time_inserted_into_mem = timer;
            return 1;                 
          }        
        if ((pointer == mem->processes )&& (head == 1))   
          {
            in->prev = NULL;
            in->next = pointer;
            mem->processes = in;// first process pointer move to inserted process
            pointer->prev = in;// link the first 2 processes
            in->start = mem->end - in->mem_size;
            in->end = mem->end;//load the process at the highest address of memory
            mem->num_processes += 1;
            in->in_mem = 1;//mark the process is into memory
            in->time_inserted_into_mem = timer;//note the time of loading into memory
            return 1;  
          }
        if( pointer!=NULL && (head == 0)) 
          {     
            in->start = pointer->start - in->mem_size;
            in->end = pointer->start;
            pointer->next->prev = in;
            in->next = pointer->next;
            pointer->next = in;
            in->prev = pointer;
            mem->num_processes += 1;
            in->in_mem = 1;
            in->time_inserted_into_mem = timer;
            return 1; 
          }
      }   
    
    if (!strcmp(alg,"worst"))//worst fir algorithm
      {
        Process *curr = mem->processes;//memory pointer used to search the porcess within the memory
        int biggest=0; 
        Process *pointer=NULL;
        int head=0;
        // Checking for space between first process and highest address of memormry.
        if ((mem->end - in->mem_size) >= (curr->end)) 
          {// engough for the insterted process
            biggest=mem->end-curr->end;
            pointer=curr;  
            head=1;     
          }
        while (curr->next!= NULL) 
          { // loop of checking for space between each process from the highest memory address.
            if ((curr->start - in->mem_size) >= curr->next->end && pointer==NULL)
              {
                biggest=curr->start-curr->next->end;
                pointer=curr; 
                if(head==1)
                  {
                    head=0;     
                  }  
              }        
            if ((curr->start - in->mem_size) >= curr->next->end && (curr->start-curr->next->end)> biggest) 
              {
                biggest = curr->start-curr->next->end;
                pointer =curr;
                if(head==1)
                  {
                    head=0;     
                  } 
              }
            curr=curr->next;              
          } 
  
        if ((curr->start - in->mem_size) >= mem->start && (curr->start-mem->start>biggest|| pointer==NULL ) ) 
          {// Checking for space between lowest address of memory and last process.(similar insert procedure )
            in->start = curr->start - in->mem_size;
            in->end = curr->start;
            curr->next = in;
            in->prev = curr;
            in->next = NULL;
            mem->num_processes += 1;
            in->in_mem = 1;
            in->time_inserted_into_mem = timer;            
            return 1;                 
          }
        
        if ((pointer == mem->processes )&& (head == 1))   
          {
            in->prev = NULL;
            in->next = pointer;
            mem->processes = in;// first process pointer move to inserted process
            pointer->prev = in;// link the first 2 processes
            in->start = mem->end - in->mem_size;
            in->end = mem->end;//load the process at the highest address of memory
            mem->num_processes += 1;
            in->in_mem = 1;//mark the process is into memory
            in->time_inserted_into_mem = timer;//note the time of loading into memory
            return 1;  
          }
        if( pointer!=NULL && (head == 0)) 
          {     
            in->start = pointer->start - in->mem_size;
            in->end = pointer->start;
            pointer->next->prev = in;
            in->next = pointer->next;
            pointer->next = in;
            in->prev = pointer;
            mem->num_processes += 1;
            in->in_mem = 1;
            in->time_inserted_into_mem = timer;
            return 1; 
          }
      }   
   // If we get here, it means there was no space and we have to
    // swap something to disk. 
    return 0;  
  }


// find the oldest process to swap out
Process *memory_remove_oldest(Memory *mem)
  {
    Process *curr = mem->processes;   // Checking for time for each process.
    int id_oldest = curr->process_id;
    int oldest = curr->time_inserted_into_mem;     
    while (curr->next != NULL) 
      { // Checking if this process is older than the current oldest.
        if (curr->next->time_inserted_into_mem < oldest) 
          {
            id_oldest = curr->next->process_id;
            oldest = curr->next->time_inserted_into_mem;           
          } 
        else if (curr->next->time_inserted_into_mem == oldest) 
          {// This means that there is another process that is equally old. As per the spec we select lower priority.
            if (curr->next->process_id > id_oldest) 
              {
                id_oldest = curr->next->process_id;
                oldest = curr->next->time_inserted_into_mem;
              }
          }
        curr = curr->next;
      }    
    return memory_remove(mem, id_oldest);
  }

/* 
** Returns the Process in question. We do this because we remove a process
** from memory in two cases:
** 1. The process has completed. If this is the case we can just free
**    the process and discard it.
** 2. The process has been pulled from memory to make space for something
**    else but it hasn't completed. As such we need to pointer so we 
**    can re-add it to disk.
*/
Process *memory_remove(Memory *mem, int process_id)
  {
    Process *curr = mem->processes;
    // Checking if there was only one item in memory.
    // If so, we free/remove it and reset mem->processes to NULL.
    if (mem->processes->next == NULL) 
      {
        Process *ret = mem->processes;
        ret->active = 0;
        ret->in_mem = 0;
        ret->time_inserted_into_mem = -1;
        mem->processes = NULL;
        mem->num_processes -= 1;
        return ret;
      }
    // Checking if the process to remove is after the 1st element.
    int first = 1;
    while(1) 
      {
        if (curr->process_id == process_id) 
          {
            curr->active = 0;
            curr->in_mem = 0;
            curr->time_inserted_into_mem = -1;
            // Linking the neighbouring processes.
            if (curr->prev != NULL) 
              {
                curr->prev->next = curr->next;
              }
            if (curr->next != NULL) 
              {
                curr->next->prev = curr->prev;
              }
            mem->num_processes -= 1;
            if (first)
              {// if the first process is removed, move curr point to the second process. 
                mem->processes = curr->next;
               } 
            return curr;
          }
        curr = curr->next;
        first = 0;
      }
    // Shouldn't get here.
    return NULL;
  }

/*
** Recalculates and updates num_holes value in the memory struct.
** Iterate through the processes and check the space between each of them. 
*/
void memory_count_holes(Memory *mem)
  {
    int holes = 0;   
    Process *curr = mem->processes;
    if (curr == NULL) 
      {
        mem->num_holes = 1;
      }
    // Checking if the first process is at mem address <end> or not.
    if (curr->end < mem->end) 
      {
        holes +=1;
      }   
    while(curr->next != NULL) 
      {

        if (curr->start > curr->next->end) 
          {
            holes +=1;
          }
        curr = curr->next;
      }    
    // Checking for a hole between the last process and the end.
    if (curr->start > mem->start) 
      {
        holes +=1;
      }
    mem->num_holes = holes;
  }

/*
** Add up the total size of the processes in emory and return
** as a percentage. For use the print function when a process starts.
*/
int get_mem_usage(Memory *mem)
  {
    int total_usage = 0;
    Process *curr = mem->processes;
    if (curr == NULL) 
      {
        return 0;
      }
    else
      {
        total_usage =curr->mem_size;
        while(curr->next != NULL) 
          {
        total_usage += curr->next->mem_size;
        curr = curr->next;
          }
      }
    int percentage = (total_usage*100)/mem->end;
      if (total_usage*100 % mem->end != 0) 
        {
          percentage += 1;
        } 
    return percentage;
  }