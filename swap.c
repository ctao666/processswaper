/*Swap Simulation main program
 *  *  * Author: Chun TAO
 *   *   * student ID: 879010*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "queue.h"
#include "memory.h"

/* Extern declarations: */
extern char *optarg;

void print_usage(char *program_name);
void simulate(Process *disk_processes, int num_processes, Memory *memory, char *alg, int q);

int main(int argc, char **argv)
  {
    char input;
    char *target;
    char *alg;
    int memsize;
    int q;
    // Getting the options for program execution.
	  while ((input = getopt(argc, argv, "f:a:m:q:")) != EOF)
      {
		    switch ( input )
		      {
     			  case 'f':
    				  // get the file to read the processes.
      				target = optarg;
      				break;
      			case 'a':
      				// get the memory algorithm to use.
      				if(strcmp(optarg,"first") == 0) 
      					alg = optarg;
      				else if(strcmp(optarg, "best") == 0) 
      					alg = optarg;
              else if(strcmp(optarg, "worst") == 0) 
      					alg = optarg;
      				else 
                {
      					  // Exit if optarg unknown.
      					  fprintf(stderr, "Invalid option %s\n", optarg);
      					  print_usage(argv[0]);
      					  exit(1);
      				  }
       				break;
      			case 'm':
      				// get total memory size.
      				memsize = atoi(optarg);
      				break;
            case 'q'://get quantum size.
              q = atoi(optarg);   
              break;
      			default:
      				// Should not get here.
      				print_usage(argv[0]);
      				return -1;
      		}
      }
   
    /*
  	** Here we read processes into disk. Throughout the life of the
  	** simulation, this disk linked list will hold both processes that
  	** haven't yet been started (future processes whose start times are
  	** greater than the current simulation time) as well as processes
  	** that have been swapped back to disk because memory was too full.
  	*/
  	int num_processes = 0;
  	Process *disk_head = read_processfile(target, memsize, &num_processes);
    if (disk_head == NULL) 
      {
  		fprintf(stderr, "%s couldn't be read properly, exiting...\n", target);
  		return -1;
  	  }  
  	Memory *memory = create_memory(memsize);  
  	simulate(disk_head, num_processes, memory, alg, q);  	
  	return 0;
  }

/*
** Print usage on incorrect command line usage.
*/
void print_usage(char *program_name)
  {
  	fprintf(stderr, "Usage(warning): %s -f [target file name] -a [algorithm] -m [memsize] -q [quantum size]\n", program_name);
  	fprintf(stderr, "-f Name of input text file\n");
  	fprintf(stderr, "-a {first, best, worst}\n");
  	fprintf(stderr, "-m Integer in mb (greater than 0)\n");
    fprintf(stderr, "-q An integer( quantum size in seconds)\n");
  }

/*
** This is the main swap function that controls the simulation in the program.
** After we create the appropriate queues, the main steps of this function are: 
** - If there isn't an active process,check for processes on disk which haven't 
**   been loaded into a queue yet based on the time stay in the disk.
** - We try to load this process into memory based on first/best/worst algorithm If 
**   there isn't enough space, we keep removing the oldest from memory until 
**   there is enough space.
** - we pop one off a procees from the queue.
** - Count the holes and print the statement when a process is swaped in to     
**   memory.
** - Decrement the time remaining for the active process and then check if it
**   is done. If so, we remove the process from memory and free it.
** - If a process just finished, we then check if there are any processes
**   left in the queues or on disk. If so, we're done!
** - Otherwise, we just incrememnt the timer and decrement the remaining
**   time for the given quantum q.
*/
void simulate(Process *disk_processes, int num_processes, Memory *memory, char *alg, int q)
  {
    int timer =0;       
    Process *check= disk_processes;       
    Queue *rrq = create_queue(q, num_processes);       
	  int remaining_quantum= rrq->quantum;
	  Process *active = NULL;      
   
    while(1)
      {        
	      Process *expired = NULL;	//store the porcess whose quantum is just expired
     		if (remaining_quantum == 0) 
          {
			      if (active != NULL) 
              {
        				// current active process is running out of its quantum, push it to the last of    
                //queue, and deactivate it    			   				   				    				
        				expired=active;
        				active = NULL;
    			    }
            //refresh a full quantum for next process
    			  remaining_quantum = rrq->quantum;
   	      }                     
		    if (active == NULL) 
          {		   	  
 		   	    check = disk_processes;
              int keep = 1;    //indicate wheather need to reload the deactived process into queue(if it is swapout then no need to reload)     
            if (check!=NULL) 
              {  /*
	               ** Deciding which process to load in memory. 
	               ** thenpop the front of the queue
	               */   
			           if (check->time_created <= timer) 
                   { 				      
      				       Process *curr = check;
                     Process *pointer=curr->next;             
                     while (pointer!=NULL)
                       {                
                         if (pointer->time_created<curr->time_created)
                           {
                             curr=pointer;
                           }
                         if (pointer->time_created == curr->time_created && pointer->process_id < curr->process_id)
                           {
                             curr=pointer;
                           }
                         pointer=pointer->next;                
                       } 
      				       // Checking if the relevant process is at the head of the
       				       // disk linked list.
        					   // If there is more than one item, link the head to the
      	  				   // next process in the linked list.
      		  			   if (curr->next != NULL) 
                       {
                         if(curr->prev==NULL)
                           {
      						           disk_processes = curr->next;
      						           disk_processes->prev = NULL;
                           }
                         else 
                           {
                             curr->prev->next=curr->next;
                             curr->next->prev=curr->prev;
                           }                      
      			  		    //  Otherwise just set disk processes to NULL. 
					             } 
                     else 
                       {
                         if(curr->prev==NULL)
                           {
      						           disk_processes = NULL;
                           }
                         else
                           {
                             curr->prev->next=NULL;
                           }                   
     					        }
   			            
                 	  /* 
            				** Trying to insert the process into memory.
            				** If this fails, it calls the function to remove the
            				** oldest process currently memory.
            				** It will keep doing these two functions until the 
            				** process is inserted successfully.
            				*/		
        
                    while(!memory_insert(memory, curr, timer,alg)) 
                      { // Remove the oldest from memory and queue and put it back on disk.
   		  			          if (disk_processes == NULL) 
                          {
              						  disk_processes = memory_remove_oldest(memory);
              						  disk_processes->prev = NULL;
              						  disk_processes->next = NULL;
            						    disk_processes->time_created = timer;// record the time swap out                       
              						  if (disk_processes!=expired)
                              {
                                queue_remove(rrq, disk_processes);}
                            else
                              {
                                keep=0;
                              }						
              				  	} 
                        else 
                          {               
                						Process *h = disk_processes;             
                						Process *swapout=memory_remove_oldest(memory);                                                          
                            swapout->time_created = timer;   
                                                            
                					  while (h->time_created<timer ) 
                              {
                							  if (h->next==NULL)
                                  {
                                    break;
                                  }
                                h = h->next;
                					    }          
                             
                						if (h->time_created==timer && h->process_id < swapout->process_id)
                              {
                							  if (h->next!=NULL)
                                  {
                							      swapout->next=h->next;
                      							h->next->prev=swapout;
                      							h->next=swapout;
                      							swapout->prev=h;				  	
                					        }
                							  else
                                  {
                      						  swapout->next=h->next;			
                      							h->next=swapout;
                      							swapout->prev=h;	
            							        }							
                						  }
                						else
                              { 
                							  if (h->prev!=NULL)
                                  { 
                							      swapout->prev=h->prev;
                      							h->prev->next=swapout;
                      							h->prev=swapout;
                      							swapout->next=h;
                                                             
                					  	    }
                					      else
                                  {
                      					  	swapout->prev=h->prev;							
                      							h->prev=swapout;
                      							swapout->next=h;	
                      							disk_processes=swapout;				  	
                					  	    }					
                					     }
                                             
                            if (swapout!=expired)
                              { 
                					      queue_remove(rrq, swapout);	                                         
                					    }
                            else
                              {
                                keep=0;
                              } 
                              
                          }          
                	}
                // Count holes and print the appropriate message for when
        		    // a process load into memory.                         
        		  	memory_count_holes(memory);
        		  	printf("time %d, %d loaded, numprocesses=%d, numholes=%d, memusage=%d%%\n", timer, curr->process_id, memory->num_processes,
                memory->num_holes, get_mem_usage(memory));             
                queue_insert(rrq, curr);
                
                }	
                
                              	              
              }    
             // reload the just deactived process (unfinished) into queue
            if (expired!=NULL && keep)
              {
                queue_insert(rrq, expired);      
              }      
          	if (rrq->num_items == 0) 
              { 
       				  timer += 1;
         				remaining_quantum = 0;
         				continue;
          	  }       
            active = queue_pop(rrq);// Pop the item from the current queue and mark it as active.
            active->active = 1;                  
          }  
        active->remaining_time -= 1;

				/*
				** If the active process finishes executing, we call the
				** function that finds it in memory, removes it and relinks the
				** neighbouring processes. We then free it and set active to NULL.
				*/            
				if (active->remaining_time == 0) 
          {
  					// Discard the pointer, we don't need it as the process is done.
  					
            memory_remove(memory, active->process_id);
                                  
  					free_process(active);
  					free(active);
  					active = NULL;
  					remaining_quantum = 1;
					
  					// Check if there's anything left in the queue or on disk.
  					// If not, we're done!
  					if (!rrq->num_items && disk_processes == NULL) 
              {
  						  timer += 1;                    
  						  break;
  			      }
		      }            
		    remaining_quantum -= 1;
		    timer += 1;                  
      }
	  printf("time %d, simulation finished.\n", timer);//end of sim    
  }
