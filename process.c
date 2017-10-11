/*process data structure
 *  * Author: Chun TAO
 *   * student ID: 879010*/
 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "process.h"

/*
** This function reads the input file and outputs an array
** of processes as if they were on disk. We can then treat
** this array is if it were current and future processes on disk.
** 
** Returns pointer to the head of the linked list of disk processes.
** Also updates the num_processes pointer so we know how big we
** need to make the round robin queues when we initialise them.
** 
*/
Process *read_processfile(char *targetfile, int memsize, int *num_processes)
  {
    // Saving the pointer to the first Process created.
    int first = 1;
    Process *head;

    // Keeping track of the most recently created process
    // so that we can point it to the next one we create.
    Process *recent;
    FILE *file;
    if ((file = fopen(targetfile, "r")) == NULL)
      {
        fprintf(stderr, "Error: File does not exist!");
        exit(EXIT_FAILURE);
      }
    //read the process information  
    int val1 = 0, val2 = 0, val3 = 0, val4 = 0;
    while (fscanf(file, "%d %d %d %d", &val1, &val2, &val3, &val4) != EOF )
      {
        if (memsize < val3)
          {
            // If the process is going to take more memory than we have
            // available, we prompt and exit.
            fprintf(stderr, "Error: Process memory usage is more than available memory!");
            exit(EXIT_FAILURE);                
          }
        if (first) 
          {
            head = create_process(val1, val2, val3, val4);
            recent = head;
          } 
        else 
          {
            recent->next = create_process(val1, val2, val3, val4);
            recent->next->prev = recent;
            recent = recent->next;
          }
        first = 0;
        *num_processes += 1;
      }

    fclose(file);
    return head;
  }

/*
** Creates a new process. Should only need to be called from whichever
** function reads in the initial input data, as these processes will
** then be loaded "on to disk", meaning a queue that acts as if the
** swap space on disk.
** 
** Parameters are: time_created, process_id, memory_size, job_time
*/
Process *create_process(int val1, int val2, int val3, int val4)
  {
    Process *p = malloc(sizeof(Process));
    assert(p);

    // These are the inital given data.
    p->time_created = val1;
    p->process_id = val2;
    p->mem_size = val3;
    p->job_time = val4;
    // The following variables are only relevant when the process is in memory.
    // As such, many are set toplaceholders like -1 or NULL.
    p->start = -1;
    p->end = -1;
    p->remaining_time = val4; // Just the job_time to start with.
    p->active = 0;
    p->in_mem = 0;
    p->time_inserted_into_mem = -1; // Will be set when inserted in to memory.
    p->next = NULL;
    p->prev = NULL;
    return p;
  }

/*
** Frees a process. This is called once a process finishes executing entirely.
*/
void free_process(Process *p)
  {
    p->next = NULL;
    p->prev = NULL;
    free(p->next);
    free(p->prev);
  }
  