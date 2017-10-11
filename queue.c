/*Round Robin Queue
 *  * Author: Chun TAO
 *   * student ID: 879010*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "queue.h"

/*
** Creates a new queue with the given quantum.
** Note the comment about mallocing size for a struct with a
** flexible (dynamic) array member.
** 
** Sets the queue size to the maximum number of items it could hold
** being the number of processes that were given initially.
*/
Queue *create_queue(int quantum, int size)
  {
    Queue *q;
    /* 
    ** This odd malloc is due to how flexible/dynamic array members work
    ** inside a struct. You don't malloc the array separately like this:
    ** q->queue = malloc(sizeof(Process*) * BASE_QUEUE_SIZE);
    ** but instead just malloc the Queue the size of both the size of the
    ** struct AND the size of the array.
    */
    q = malloc(sizeof(Queue) + sizeof(Process*) * size);
    q->quantum = quantum;
    q->start = 0;
    q->num_items = 0;
    q->max_size = size;
    return q;
  }

/*
**Remove the process from queue( When a process is swapped out of memory and placed on disk,
**it must also be removed from the Round Robin queue.) 
*/
void queue_remove(Queue *q, Process *in)
  {
    int i = q->start;
    while(q->queue[i]!= in)
      {     
     	  i=(i+1)% q->max_size;
      }   
      //find the queue index of the process
    if(i==q->start)
      {    		
        q->queue[q->start] = NULL;         
        q->start = (q->start + 1) % q->max_size;          
      }
    else if (i ==( q->start + q->num_items-1)% q->max_size)
      {   	     
          q->queue[i]= NULL;               
      }		    		 
    else
      {	
    	int j=i;   	
    	while (j!=(q->start + q->num_items-1)% q->max_size)
        {
    	q->queue[j]=q->queue[(j+1)% q->max_size];
    	j=(j+1)% q->max_size;     
        }    	
      }
    q->num_items -= 1;
  }

/*
** Pops an process from the front of the queue and adjusts the start
** accordingly. This is either incrementing by 1 or looping back
** to the start of the array if we're at the end.
*/
Process *queue_pop(Queue *q)
  {
    Process *ret = q->queue[q->start];
    q->queue[q->start] = NULL;
    q->start = (q->start + 1) % q->max_size;//adust the start pointer    
    q->num_items -= 1;    
    return ret;
  }

/*
** Calculates where to put the next item. This calculation takes into
** account the cyclical nature of the queue array using modulo.
*/
void queue_insert(Queue *q, Process *in)
  {   	  
    int next_index = (q->start + q->num_items) % q->max_size;
    q->queue[next_index] = in;
    q->num_items += 1;
  }



