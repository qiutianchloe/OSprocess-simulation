#include<stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>


#define True    1
#define False   0


/*
string function:

1). substring
    char* subst = substring(p,i,2); 

2). chech the similarity of 2 string
    if(strcmp(taga, subst)==0 ){
        return i+2; 
    }
*/

/* type definitions --------------------------------------------- -------*/
typedef struct node node_t;
struct node {
    //time-arrived, process-id, memory-size-req, job-time
	int time_arrived;                    
    int process_id;               
    int memory_size_req;                
    int job_time;   
    int remain_time;     
    int quantum;                                      
	node_t *next;             /* the pointer that point to the next node */
};

typedef struct {
	node_t *head;             /* the start node of this link list */
	node_t *foot;             /* the end node of this link list */
} list_t;
/* type definitions --------------------------------------------- -------*/


/*-----------------------------------function declaration-------------------------------*/
/*read in all the file*/
void read_in_cmd(char *argv[], int argc, char *cmd_arg[]);
void read_file_into_list(char* filename, list_t *process_input, int quantum);

/*only the scheduling method*/
//ff  process scheduling
void run_ff_algorithm(list_t *process_input);
int check_arrival(int simulator_timer, list_t *process_input, list_t *to_do_list, int num_processes_arrived);
void finish_curr_process(int simulator_timer, list_t *process_input, list_t *to_do_list,node_t *curr_process, int num_processes_arrived);
void run_new_process(int simulator_timer, list_t *process_input, list_t *to_do_list,node_t *curr_process, int num_processes_arrived);
void run_rr_algorithm(list_t *process_input, int quantum);

/*node operation*/
node_t *create_a_node(int time_arrived, int process_id, int memory_size_req, int job_time, int remain_time, int quantum); 
/* list operations */
list_t *make_empty_list(void);
int    is_empty_list(list_t*);
void   free_list(list_t*);
list_t *insert_at_head(list_t *list, node_t *new);
list_t *insert_at_foot(list_t *list, node_t *new);
int get_head(list_t *list);
list_t *get_tail(list_t *list);
int get_weight(list_t *list);
void delete_a_node(list_t *list, int del_process_id); 
list_t *copy_n_to_another_lists_head(list_t *list, node_t *old);
list_t *copy_n_to_another_lists_foot(list_t *list, node_t *old);
void print_list(list_t *list);

/*-----------------------------------function declaration-------------------------------*/



int 
main(int argc, char *argv[]){
    // read in commend line, and create a string array cmd_arg to record. 
    char* cmd_arg[5];
    cmd_arg[4] = NULL;
    read_in_cmd(argv, argc, cmd_arg);

    //read in the file
    list_t *process_input = make_empty_list(); 
    if(strcmp(cmd_arg[1], "rr")==0){
        int quantum=atoi(cmd_arg[4]);
        read_file_into_list(cmd_arg[0], process_input, quantum);
    }else{
        read_file_into_list(cmd_arg[0], process_input, -1);
    }

    if(strcmp(cmd_arg[1], "ff")==0){
        run_ff_algorithm(process_input);
    }else if(strcmp(cmd_arg[1], "rr")==0){
        run_rr_algorithm(process_input, atoi(cmd_arg[4]));
    }
    return 0;
}

void read_in_cmd(char *argv[], int argc,char *cmd_arg[]){
    int count;
    //cmd_arg[0]=>(-f): filename(string)
    //cmd_arg[1]=>(-a): scheduling_algorithm(string)
    //cmd_arg[2]=>(-m): memery_allocation(string)
    //cmd_arg[3]=>(-s): memory_size(int->string)
    //cmd_arg[4]=>(-q): quantum(int->string) 
    int cmd_counter = 0; 
    
    //printf("The command line has %d arguments :\n",argc-1);
    for (count = 1; count < argc; ++count) {
        if(argv[count][0]=='-'){
            switch(argv[count][1]){
                case 'f' :
                    //printf("input the filenmae\n" );
                    cmd_arg[0] = argv[count+1];
                    break;
                case 'a' :
                    //printf("input the scheduling algorithm \n" );
                    cmd_arg[1] = argv[count+1];
                    break;
                case 'm' :
                    //printf("input the memory allocation\n" );
                    cmd_arg[2] = argv[count+1];
                    break;
                case 's' :
                    //printf("input the memory size\n" );
                    cmd_arg[3] = argv[count+1];
                    break;
                case 'q' :
                    //printf("quantum\n" );
                    cmd_arg[4] = argv[count+1];
                    break;
                default :
                    printf("There are somthing wrong with the input\n" );
                }       
        }
    } 
}

void read_file_into_list(char* filename, list_t *process_input, int quantum){
    //open the file
    FILE *fp=fopen(filename,"r"); 

    //check whether the file open successfully
    if(!fp){
        //exit the program if the file open failed
       printf("fail to open the file！\n");
       exit(-1);
    }

    //declare the variable to record 
    int time_arrived;                    
    int process_id;               
    int memory_size_req;                
    int job_time;  

    //read the file according to its format and record the value into the list
    while(!feof(fp)){
        //read in the file
        fscanf(fp,"%d %d %d %d\n",&time_arrived, &process_id, &memory_size_req, &job_time); 
        //create a new node
        node_t *new_process = create_a_node(time_arrived, process_id, memory_size_req, job_time,job_time,quantum);
        //put into the list
        process_input = insert_at_foot(process_input, new_process);
    }

}

void run_ff_algorithm(list_t *process_input){
    int simulator_timer = 0; 

    //information about the arriving job  
    int num_processes_arrived = 0; 
    list_t *to_do_list = make_empty_list(); 

    node_t *curr_process=NULL; 

    while(!is_empty_list(process_input)){
        /*accept the arriving process*/
        num_processes_arrived = check_arrival(simulator_timer, process_input, to_do_list, num_processes_arrived);

        /*executing the process*/
        if(curr_process!=NULL && curr_process->remain_time!=0){
            /*continue dealing with current process*/
            curr_process->remain_time--;  
        }else{
            if(curr_process!=NULL){
                /*this process has already finished*/
                num_processes_arrived--;
                finish_curr_process(simulator_timer, process_input, to_do_list, curr_process,num_processes_arrived);
            }
            /*execute the next process*/
            //find the right one 
            if(!is_empty_list(to_do_list)){
                //there are still process need to execute in to-do-list
                curr_process = to_do_list->head;
                printf("%d, RUNNING, id=%d, remaining-time=%d\n", simulator_timer, curr_process->process_id, curr_process->remain_time);
                curr_process->remain_time--; 
            }else if(!is_empty_list(process_input)){
                //there are still process need to execute, but they just haven't arrived yet
                //so do nothing just wait 
                curr_process = NULL; 
            }else{
                //there is nothing to do here, so break out
                break;
            }
        }
        //printf("Happended at %d: \n",simulator_timer);
        simulator_timer++; 
    }
}

int 
check_arrival(int simulator_timer, list_t *process_input, list_t *to_do_list, int num_processes_arrived){
    int old_num = num_processes_arrived;
    //checking the arrival
    node_t *following_process= process_input->head; 
    while(following_process!=NULL){
        //go through the whole list to find the process that arrived at this time 
        if(following_process->time_arrived==simulator_timer){
            //if the to_do_list is empty
            if(to_do_list->head==NULL){
                //insert into it any way
                to_do_list=copy_n_to_another_lists_foot(to_do_list, following_process);
            }else{
                node_t *prev = to_do_list->head; 
                node_t *curr = prev->next; 

                //if the head process is the process that arrived at the same time as the current one
                //and the process id is larger than the current one
                if(prev->time_arrived==following_process->time_arrived && following_process->process_id<prev->process_id){
                    //insert it in the head of the list
                    to_do_list=copy_n_to_another_lists_head(to_do_list, following_process); 
                }else{  
                    //let we go throught the 
                    int find_same_arr_process = False; 
                    while(curr!=NULL){
                        if(curr->time_arrived==following_process->time_arrived){
                            find_same_arr_process = True; 
                            if(curr->process_id>following_process->process_id){
                                node_t *process = create_a_node(following_process->time_arrived, following_process->process_id, following_process->memory_size_req, following_process->job_time,following_process->remain_time,following_process->quantum);
                                prev->next = process;
                                process->next = curr;  
                            }else{
                                to_do_list=copy_n_to_another_lists_foot(to_do_list, following_process);
                            }
                        }
                        prev= prev->next; 
                        curr = curr->next; 
                    }
                    if(!find_same_arr_process){
                        to_do_list=copy_n_to_another_lists_foot(to_do_list, following_process);
                    }
                }
                
            }
            num_processes_arrived++; 
        }
        following_process=following_process->next;
    }
    //printf("there are %d processes arrived in this round\n",num_processes_arrived-old_num);
    return num_processes_arrived;
}

void 
finish_curr_process(int simulator_timer, list_t *process_input, list_t *to_do_list,node_t *curr_process, int num_processes_arrived){
    //finish the current process
    printf("%d, FINISHED, id=%d, proc-remaining=%d\n", simulator_timer,curr_process->process_id, num_processes_arrived);
    delete_a_node(process_input,curr_process->process_id);
    delete_a_node(to_do_list,curr_process->process_id);
}
void 
run_new_process(int simulator_timer, list_t *process_input, list_t *to_do_list,node_t *curr_process, int num_processes_arrived){
    printf("%d, RUNNING, id=%d, remaining-time=%d\n", simulator_timer, curr_process->process_id, curr_process->remain_time);
    curr_process->remain_time--; 
    num_processes_arrived--; 
}

void 
run_rr_algorithm(list_t *process_input, int quantum){

    int simulator_timer = 0; 

    //information about the arriving job  
    int num_processes_arrived = 0; 
    list_t *to_do_list = make_empty_list(); 

    node_t *curr_process=NULL; 

    while(!is_empty_list(process_input)){
        //accept the arriving process
        num_processes_arrived = check_arrival(simulator_timer, process_input, to_do_list, num_processes_arrived);
        /*-----------------------------------------------there is no need to change anything until here-------------------------------------------------------*/

        //executing the process
        if(curr_process!=NULL && curr_process->remain_time!=0 && curr_process->quantum!=0){
            //continue dealing with current process
            curr_process->remain_time--;  
            curr_process->quantum--;  
        }else{
            if(curr_process!=NULL){
                //this process has already finished
                if(curr_process->remain_time==0){
                    num_processes_arrived--;
                    printf("%d, FINISHED, id=%d, proc-remaining=%d\n", simulator_timer,curr_process->process_id, num_processes_arrived);
                    delete_a_node(process_input,curr_process->process_id);
                    delete_a_node(to_do_list,curr_process->process_id);
                }else{
                    //delete from the head of the to-do-list, put at the end of the queue
                    //delete from head 
                    //printf("%d, ELASPED, id=%d, proc-remaining=%d, and the project still remain %d time\n", simulator_timer,curr_process->process_id, num_processes_arrived,curr_process->remain_time);
                    node_t *next_round_process = create_a_node(curr_process->time_arrived, curr_process->process_id,curr_process->memory_size_req,curr_process->job_time, curr_process->remain_time,quantum);
                    delete_a_node(to_do_list,curr_process->process_id);
                    //insert at the end of the 
                    to_do_list = insert_at_foot(to_do_list, next_round_process);
                }
            }
            //execute the next process
            //find the right one 
            if(!is_empty_list(to_do_list)){
                //there are still process need to execute in to-do-list
                curr_process = to_do_list->head;
                printf("%d, RUNNING, id=%d, remaining-time=%d\n", simulator_timer, curr_process->process_id, curr_process->remain_time);
                curr_process->remain_time--; 
                curr_process->quantum--;  
            }else if(!is_empty_list(process_input)){
                //there are still process need to execute, but they just haven't arrived yet
                //so do nothing just wait 
                curr_process = NULL; 
            }else{
                //there is nothing to do here, so break out
                break;
            }
        }
        simulator_timer++; 
    }
}


/*---------------------------------------------------------------------------
   Code that follows is written by Alistair Moffat, as an example for the book
   I changed a little bit based on his original code.
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.

   See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
   information.

   Prepared December 2012 for the Revised Edition.
----------------------------------------------------------------------------*/
node_t
*create_a_node(int time_arrived, int process_id, int memory_size_req, int job_time,int remain_time, int quantum){
    node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(new!=NULL);
	new->time_arrived = time_arrived;
    new->process_id = process_id;
    new->memory_size_req = memory_size_req;
    new->job_time = job_time;
    new->remain_time = remain_time;
    new->quantum = quantum;
    new->next = NULL; 
	return new;
}

list_t
*make_empty_list(void) {
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

int
is_empty_list(list_t *list) {
	assert(list!=NULL);
	return list->head==NULL;
}

void
free_list(list_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

list_t
*insert_at_head(list_t *list, node_t *new) {
	assert(list!=NULL && new!=NULL);
	new->next = list->head;
	list->head = new;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->foot = new;
	}
	return list;
}

list_t
*copy_n_to_another_lists_head(list_t *list, node_t *old){
    assert(list!=NULL && old!=NULL);
    //create a new node here; 

    node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(new!=NULL);
	new->time_arrived = old->time_arrived;
    new->process_id = old->process_id;
    new->memory_size_req= old->memory_size_req;
    new->job_time = old->job_time;
    new->remain_time=old->remain_time;
    new->quantum=old->quantum;
	new->next = list->head;
	list->head = new;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->foot = new;
	}
	return list;
}


void delete_a_node(list_t *list, int del_process_id){    
    node_t *curr, *prev;
	assert(list!=NULL); 
    prev = list->head; 
    curr = prev->next; 
    //that means there is at least one node in the list
    assert(prev!=NULL);

    //deal with the case that there is only one node in the list
    if(list->head==list->foot){
        if(prev->process_id == del_process_id){
            list->head=NULL; 
            list->foot=NULL; 
            free(prev);
            //printf("deleted node %d\n", del_process_id);
        }else{
            printf("there is no such node in the list\n");
        }
    }else{
        //check if the node that we need to delete is the head
        if(prev->process_id==del_process_id){
            //printf("deleted node %d\n", del_process_id);
            list->head=curr; 
            free(prev);
        }else{
            //if it is not the head of the list
            //then iterate throught the list
            while(curr!=NULL){
                //we find that node
                if(curr->process_id==del_process_id){
                    //check whether this is the foot node
                    if(curr->next==NULL){
                        //this is the node at the foot
                        list->foot=prev; 
                        prev->next = curr->next; 
                        free(curr);
                        //that will NULL
                        //printf("deleted node %d\n", del_process_id);
                        break; 
                    }else{
                        //not the foot node
                        prev->next = curr->next; 
                        //printf("deleted node %d\n", del_process_id);
                        free(curr);
                        break;
                    }
                }else{
                    prev = curr; 
                    curr=curr->next; 
                }
            }
        }
    }
}

list_t
*insert_at_foot(list_t *list, node_t *new) {
	assert(list!=NULL && new!=NULL);
	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}

list_t
*copy_n_to_another_lists_foot(list_t *list, node_t *old){
    assert(list!=NULL && old!=NULL);
    //create a new node here; 

    node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(new!=NULL);
	new->time_arrived = old->time_arrived;
    new->process_id = old->process_id;
    new->memory_size_req= old->memory_size_req;
    new->job_time = old->job_time;
    new->remain_time=old->remain_time;
    new->quantum=old->quantum;
	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}


int
get_head(list_t *list) {
	assert(list!=NULL && list->head!=NULL);
	return list->head->process_id;
}

int 
get_weight(list_t *list){
    assert(list!=NULL && list->head!=NULL);
    return list->head->process_id;
}

list_t
*get_tail(list_t *list) {
	node_t *oldhead;
	assert(list!=NULL && list->head!=NULL);
	oldhead = list->head;
	list->head = list->head->next;
	if (list->head==NULL) {
		/* the only list node just got deleted */
		list->foot = NULL;
	}
	free(oldhead);
	return list;
}
void 
print_list(list_t *list){
    assert(list!=NULL);
    node_t *curr=list->head;
    printf("start of the list:");
    while(curr!=NULL){
        printf("|process_id=%d, remain time=%d|->", curr->process_id, curr->remain_time);
        curr=curr->next; 
    }
    printf("\n");
}

/* end of of code written by Moffat -------------------------------------- */