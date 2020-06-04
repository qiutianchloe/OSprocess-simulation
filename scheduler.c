#include<stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <math.h>


#define True    1
#define False   0
#define MAX 1000

/* type definitions --------------------------------------------- -------*/
typedef struct node node_t;
struct node {
    //time-arrived, process-id, memory-size-req, job-time
	long long int time_arrived;                    
    long long int process_id;               
    long long int memory_size_req;                
    long long int job_time;   
    long long int remain_time;     
    long long int quantum; 
    long long int load_time;//load time = page_num/2 = memory_size/2
    long long int page_num;//page_num = memory_size/4
    long long int last_stop_time; 
    long long int *take_up_page;
    long long int allocated_page;                                         
	node_t *next;             /* the pointer that point to the next node */
};

typedef struct sta sta_t;
struct sta {
    //time-arrived, process-id, memory-size-req, job-time
	long long int time_arrived;                    
    long long int process_id;                              
    long long int job_time;   
    long long int time_complete;     
    long long int interval;                                  
	sta_t *next;             /* the pointer that point to the next node */
};

typedef struct {
	node_t *head;             /* the start node of this link list */
	node_t *foot;             /* the end node of this link list */
} list_t;

typedef struct {
	sta_t *head;             /* the start node of this link list */
	sta_t *foot;             /* the end node of this link list */
} statistic_t;
/* type definitions --------------------------------------------- -------*/

/*-----------------------------------function declaration-------------------------------*/
/*read in all the file*/
void read_in_cmd(char *argv[], int argc, char *cmd_arg[]);
void read_file_into_list(char* filename, list_t *process_input, long long int quantum);

/*only the scheduling method*/
long long int run_ff_algorithm(list_t *process_input,statistic_t *statistics);
long long int run_rr_algorithm(list_t *process_input, long long int quantum, statistic_t *statistics);
long long int run_p_on_ff_algorithm(list_t *process_input,long long int memory_size, statistic_t *statistics);
long long int run_p_on_rr_algorithm(list_t *process_input,long long int memory_size, long long int quantum, statistic_t *statistics);
long long int run_v_on_rr_algorithm(list_t *process_input,long long int memory_size, long long int quantum, statistic_t *statistics);
long long int check_arrival(long long int simulator_timer, list_t *process_input, list_t *to_do_list, long long int num_processes_arrived);
node_t *find_least_rec_pro(list_t *to_do_list);
node_t *find_least_rec_pro_for_v(list_t *to_do_list);
void assigned_value_to_empty_page(long long int* pages,long long int num_of_page,long long int value); 
long long int find_none_empty_space(long long int* pages,long long int num_of_page); 

/*node operation*/
node_t *create_a_node(long long int time_arrived, long long int process_id, long long int memory_size_req, long long int job_time, long long int remain_time, long long int quantum,long long int page_num, long long int load_time, long long int* take_up_page, long long int last_stop_time, long long allocated_page); 
/* list operations */
list_t *make_empty_list(void);
long long int is_empty_list(list_t*);
void  free_list(list_t*);
list_t *insert_at_head(list_t *list, node_t *new);
list_t *insert_at_foot(list_t *list, node_t *new);
long long int get_head(list_t *list);
list_t *get_tail(list_t *list);
long long int get_weight(list_t *list);
void delete_a_node(list_t *list, long long int del_process_id); 
void print_list(list_t *list);
long long int length_of_list(list_t *list);
char* itoa(int num,char* str,int radix);
void print_take_up_page(long long int* take_up_page, long long int num_of_page);

//function for sort the list 
int comp(const void*a, const void*b){
    return *(long long int*)a-*(long long int*)b; 
}

sta_t *create_sta_node(long long int time_arrived, long long int process_id, long long int job_time, long long int time_complete, long long int interval);
statistic_t *insert_a_sta(statistic_t *list, sta_t *new);
statistic_t *make_a_statisitics(void);
void perform_statistics(statistic_t *statistics, long long int finish_at);
/*-----------------------------------function declaration-------------------------------*/

int 
main(int argc, char *argv[]){
    // read in commend line, and create a string array cmd_arg to record. 
    char* cmd_arg[5];
    cmd_arg[4] = NULL;
    read_in_cmd(argv, argc, cmd_arg);

    /*read in the file*/
    list_t *process_input = make_empty_list(); 
    if(strcmp(cmd_arg[1], "rr")==0){
        long long int quantum=atoi(cmd_arg[4]);
        read_file_into_list(cmd_arg[0], process_input, quantum);
    }else{
        read_file_into_list(cmd_arg[0], process_input, -1);
    }

    statistic_t *statistics = make_a_statisitics();
    long long int finish_at = 0; 
    if(strcmp(cmd_arg[1], "ff")==0 && strcmp(cmd_arg[2], "u")==0){

        finish_at=run_ff_algorithm(process_input,statistics);

    }else if(strcmp(cmd_arg[1], "rr")==0 && strcmp(cmd_arg[2], "u")==0){

        finish_at=run_rr_algorithm(process_input, atoi(cmd_arg[4]),statistics);

    }else if(strcmp(cmd_arg[1], "ff")==0 && strcmp(cmd_arg[2], "p")==0){

        finish_at=run_p_on_ff_algorithm(process_input, atoi(cmd_arg[3]), statistics);

    }else if(strcmp(cmd_arg[1], "rr")==0 && strcmp(cmd_arg[2], "p")==0){

        finish_at=run_p_on_rr_algorithm(process_input,atoi(cmd_arg[3]),atoi(cmd_arg[4]),statistics);

    }else if(strcmp(cmd_arg[1], "ff")==0 && strcmp(cmd_arg[2], "v")==0){
        //they should run with the same as the algorithm p
        finish_at=run_p_on_ff_algorithm(process_input, atoi(cmd_arg[3]), statistics);

    }else if(strcmp(cmd_arg[1], "rr")==0 && strcmp(cmd_arg[2], "v")==0){

        finish_at=run_v_on_rr_algorithm(process_input,atoi(cmd_arg[3]),atoi(cmd_arg[4]),statistics);

    }else{

        printf("Sorry, I didn't implement this function so fat\n");
        exit(0);
    }

    /*do the performance statistics*/
    perform_statistics(statistics, finish_at);
    free_list(process_input);
    return 0;
}

long long int 
run_ff_algorithm(list_t *process_input,statistic_t *statistics){
    long long int interval = 0; 
    long long int simulator_timer = 0; 

    //information about the arriving job  
    long long int num_processes_arrived = 0; 
    list_t *to_do_list = make_empty_list(); 

    node_t *curr_process=NULL; 

    while(!is_empty_list(process_input)){
        //set the interval for statisics
        if((simulator_timer-1)%60==0&&simulator_timer!=1&&simulator_timer!=0){
            interval++; 
        }
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
                printf("%lld, FINISHED, id=%lld, proc-remaining=%lld\n", simulator_timer,curr_process->process_id, num_processes_arrived);
                //record the statistics; 
                sta_t *this_process = create_sta_node(curr_process->time_arrived, curr_process->process_id, curr_process->job_time, simulator_timer,interval);
                statistics = insert_a_sta(statistics, this_process);
                delete_a_node(process_input,curr_process->process_id);
                delete_a_node(to_do_list,curr_process->process_id);
            }
            /*execute the next process*/
            //find the right one 
            if(!is_empty_list(to_do_list)){
                //there are still process need to execute in to-do-list
                curr_process = to_do_list->head;
                printf("%lld, RUNNING, id=%lld, remaining-time=%lld\n", simulator_timer, curr_process->process_id, curr_process->remain_time);
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
        simulator_timer++; 
    }
    free_list(to_do_list);
    return simulator_timer;
}

long long int
run_rr_algorithm(list_t *process_input, long long int quantum,statistic_t *statistics){
    long long int interval = 0; 
    long long int simulator_timer = 0; 

    //information about the arriving job  
    long long int num_processes_arrived = 0; 
    list_t *to_do_list = make_empty_list(); 

    node_t *curr_process=NULL; 

    while(!is_empty_list(process_input)){
        //set the interval for statisics
        if((simulator_timer-1)%60==0&&simulator_timer!=1&&simulator_timer!=0){
            interval++; 
        }
        //accept the arriving process
        num_processes_arrived = check_arrival(simulator_timer, process_input, to_do_list, num_processes_arrived);

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
                    printf("%lld, FINISHED, id=%lld, proc-remaining=%lld\n", simulator_timer,curr_process->process_id, num_processes_arrived);
                    sta_t *this_process = create_sta_node(curr_process->time_arrived, curr_process->process_id, curr_process->job_time, simulator_timer,interval);
                    statistics = insert_a_sta(statistics, this_process);
                    delete_a_node(process_input,curr_process->process_id);
                    delete_a_node(to_do_list,curr_process->process_id);
                }else{
                    //delete from the head of the to-do-list, put at the end of the queue
                    //delete from head 
                    //printf("%lld, ELASPED, id=%lld, proc-remaining=%lld, and the project still remain %lld time\n", simulator_timer,curr_process->process_id, num_processes_arrived,curr_process->remain_time);
                    node_t *next_round_process = create_a_node(curr_process->time_arrived, 
                                                                curr_process->process_id,
                                                                curr_process->memory_size_req,
                                                                curr_process->job_time, 
                                                                curr_process->remain_time,
                                                                quantum, 0, 0, NULL, simulator_timer, 
                                                                curr_process->allocated_page);
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
                printf("%lld, RUNNING, id=%lld, remaining-time=%lld\n", simulator_timer, curr_process->process_id, curr_process->remain_time);
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
    free_list(to_do_list);
    return simulator_timer; 
}

long long int
run_p_on_ff_algorithm(list_t *process_input, long long int memory_size, statistic_t *statistics){

    long long int interval = 0; 
    long long int simulator_timer = 0; 
    //remain page also equal total memory page minus empty_space_start. 
    //or total memory=empty_space_start+remain_page
    long long int memory_page = memory_size/4; //const
    long long int ram[MAX]; 
    long long int remain_page = memory_page; 
    long long int empyt_space_start = 0; 

    // initialize the all the memory page as -1
    long long int counter=0; 
    for(counter = 0; counter<MAX; counter++){
        ram[counter] = -1;  
    }

    //information about the arriving job  
    long long int num_processes_arrived = 0; 
    list_t *to_do_list = make_empty_list(); 

    node_t *curr_process=NULL; 

    while(!is_empty_list(process_input)){
        //set the interval for statisics
        if((simulator_timer-1)%60==0&&simulator_timer!=1&&simulator_timer!=0){
            interval++; 
        }
        /*accept the arriving process*/
        num_processes_arrived = check_arrival(simulator_timer, process_input, to_do_list, num_processes_arrived);

        /*executing the process*/
        if(curr_process!=NULL && curr_process->remain_time!=0 && curr_process->load_time!=0){
            /*continue dealing with load time*/
            curr_process->load_time--;  
        }else if(curr_process!=NULL && curr_process->remain_time!=0 && curr_process->load_time==0){
            /*continue dealing with current process*/
            curr_process->remain_time--;  
        }else{
            //FINISH A PROGRESS
            if(curr_process!=NULL){
                /*this process has already finished*/
                num_processes_arrived--;
                empyt_space_start = curr_process->take_up_page[0]; 
                remain_page = remain_page+curr_process->page_num;
                printf("%lld, EVICTED, mem-addresses=", simulator_timer);
                print_take_up_page(curr_process->take_up_page, curr_process->page_num);
                int i=0; 
                for(i=0; i<curr_process->page_num; i++){
                    int assign_page=curr_process->take_up_page[i];
                    ram[assign_page] = -1;
                }
                printf("%lld, FINISHED, id=%lld, proc-remaining=%lld\n", simulator_timer,curr_process->process_id, num_processes_arrived);
                //record the statistics; 
                sta_t *this_process = create_sta_node(curr_process->time_arrived, curr_process->process_id, curr_process->job_time, simulator_timer,interval);
                statistics = insert_a_sta(statistics, this_process);
                delete_a_node(process_input,curr_process->process_id);
                delete_a_node(to_do_list,curr_process->process_id);
            }
            /*START A NEW PROGRASS*/
            if(!is_empty_list(to_do_list)){
                //there are still process need to execute in to-do-list
                //choose a process to do.
                curr_process = to_do_list->head;
                //assign the memory to process
                //There must be enough space for p mode, so we just assign the space to the process directly
                long long int page_need= curr_process->page_num;
                int memory_take_up_per;
                if(remain_page>=page_need){
                    int i=0; 
                    for(i=0; i<page_need; i++){
                        long long int assign_page=empyt_space_start+i;
                        curr_process->take_up_page[i]=assign_page;
                        ram[assign_page] = curr_process->process_id;
                    }
                    //change the empty_space_start
                    empyt_space_start = empyt_space_start+page_need; 
                    remain_page = remain_page-page_need; 
                    //calculate the memory take up percentage
                    //memory_take_up_per = ((double)page_need/memory_page)*100; 
                    memory_take_up_per = ceil(((double)(memory_page-remain_page)/memory_page)*100); //this is (2)
                }else{
                    printf("The memory is empty but there is still no space for the process!\n");
                } 

                printf("%lld, RUNNING, id=%lld, remaining-time=%lld, load-time=%lld, mem-usage=%d%%, mem-addresses=", simulator_timer, curr_process->process_id, curr_process->remain_time, curr_process->load_time,memory_take_up_per);
                print_take_up_page(curr_process->take_up_page, curr_process->page_num);
                curr_process->load_time--; 
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
    free_list(to_do_list);
    return simulator_timer;
}

long long int 
run_p_on_rr_algorithm(list_t *process_input,long long int memory_size, long long int quantum, statistic_t *statistics){
    long long int interval = 0; 
    long long int simulator_timer = 0; 
    //remain page also equal total memory page minus empty_space_start. 
    //or total memory=empty_space_start+remain_page
    long long int memory_page = memory_size/4; //const
    long long int ram[MAX]; 
    long long int remain_page = memory_page; 
    long long int empyt_space_start = 0;  
    // initialize the all the memory page as -1
    long long int counter=0; 
    for(counter = 0; counter<MAX; counter++){
        ram[counter] = -1;  
    }
    //information about the arriving job  
    long long int num_processes_arrived = 0; 
    list_t *to_do_list = make_empty_list(); 

    node_t *curr_process=NULL; 

    while(!is_empty_list(process_input)){
        //printf("time %lld :\n", simulator_timer);
        //set the interval for statisics
        if((simulator_timer-1)%60==0&&simulator_timer!=1&&simulator_timer!=0){
            interval++; 
        }
        //keep eye on the arriving process
        num_processes_arrived = check_arrival(simulator_timer, process_input, to_do_list, num_processes_arrived);

        if(curr_process!=NULL && curr_process->remain_time!=0 && curr_process->quantum!=0 && curr_process->load_time!=0){
            /*continue dealing with load time*/
            //printf("loading process %lld, there are %lld time need to load\n", curr_process->process_id,curr_process->load_time);
            curr_process->load_time--;  
        }else if(curr_process!=NULL && curr_process->remain_time!=0 && curr_process->quantum!=0 && curr_process->load_time==0){
            /*continue dealing with current process*/
            //printf("executing process %lld, there are %lld time need to execute,  and %lld time in quantum \n", curr_process->process_id,curr_process->remain_time, curr_process->quantum);
            curr_process->remain_time--;  
            curr_process->quantum--;  
        }else{
            //this is not the first process, then we need to finish the current process first
            if(curr_process!=NULL){
                //printf("current process %lld need to finish\n", curr_process->process_id);
                //printf("another round assignment happened at %lld time", simulator_timer);
                //this process has already finished
                if(curr_process->remain_time==0){
                    remain_page = remain_page+curr_process->page_num;
                    empyt_space_start = curr_process->take_up_page[0];
                    printf("%lld, EVICTED, mem-addresses=", simulator_timer);
                    print_take_up_page(curr_process->take_up_page, curr_process->page_num);
                    int i=0; 
                    for(i=0; i<curr_process->page_num; i++){
                        int assign_page=curr_process->take_up_page[i];
                        ram[assign_page] = -1;
                    }
                    num_processes_arrived--;
                    printf("%lld, FINISHED, id=%lld, proc-remaining=%lld\n", simulator_timer,curr_process->process_id, num_processes_arrived);
                    //record for the statistics
                    sta_t *this_process = create_sta_node(curr_process->time_arrived, curr_process->process_id, curr_process->job_time, simulator_timer,interval);
                    statistics = insert_a_sta(statistics, this_process);
                    //delete from the process input list
                    delete_a_node(process_input,curr_process->process_id);
                    //delete from the todo list 
                    delete_a_node(to_do_list,curr_process->process_id);
                }else{
                    // 3. make it load time as 0, because it has already being loaded inside the ram
                    node_t *next_round_process = create_a_node(curr_process->time_arrived, 
                                                                            curr_process->process_id,
                                                                            curr_process->memory_size_req,
                                                                            curr_process->job_time, 
                                                                            curr_process->remain_time,
                                                                            quantum, 0, 
                                                                            curr_process->page_num, 
                                                                            curr_process->take_up_page, 
                                                                            simulator_timer, 
                                                                            curr_process->allocated_page);

                    //printf("the evict process: ");
                    //print_take_up_page(next_round_process->take_up_page, next_round_process->page_num);                   
                    //2. mark the process with its stop time
                    next_round_process->last_stop_time = simulator_timer;
                    //1. delete it from the head of the list
                    delete_a_node(to_do_list,curr_process->process_id);
                    //4. insert at the end of the 
                    to_do_list = insert_at_foot(to_do_list, next_round_process);
                }
            
            }
            //execute the next process
            //then load a new one 
            //printf("check whether there is process on the to do list\n");
            //print_list(to_do_list);
            if(!is_empty_list(to_do_list)){
                //printf("there are process need to process\n");
                //printf("I start to find a new process in the to do list to load\n");
                //there are still process need to execute in to-do-list
                //choose a process to do.
                curr_process = to_do_list->head;
                //assign the memory to process
                //If there is no 
                long long int page_need= curr_process->page_num;
                if(memory_page<page_need){
                    printf("there is not enought space, even the memory is empty!\n"); 
                }
                //case 1: if there are enough pages for the new process, then we just assign memory to the processes directly
                //case 2: otherwise, we need to evict other process according to when they finish(least recentely executed). 
                // there is another condition, that is the process has already being loaded already. no need to assign extra space
                if(curr_process->load_time==0){
                    //I guess do nothing
                }else if(remain_page>=page_need){
                    long long int found_page=0; 
                    while(found_page!=page_need){
                        if(empyt_space_start==memory_page){
                            empyt_space_start=0; 
                        }
                        if(ram[empyt_space_start]==-1){
                            ram[empyt_space_start]=curr_process->process_id;
                            curr_process->take_up_page[found_page]=empyt_space_start;
                            found_page++;
                        }
                        empyt_space_start++; 
                    }  
                    remain_page = remain_page-page_need; 
                }else{
                    long long int found_page=0; 
                    while(found_page!=remain_page){
                        if(empyt_space_start==memory_page){
                            //that's end of the pages
                            empyt_space_start=0; 
                        }
                        if(ram[empyt_space_start]==-1){
                            ram[empyt_space_start]=curr_process->process_id;
                            curr_process->take_up_page[found_page]=empyt_space_start;
                            found_page++; 
                        }
                        empyt_space_start++;
                    }  
                    //change the remain page value
                    remain_page = 0; 
                    //then we need to start evict some other loaded processes. 
                    while(found_page<page_need){
                        //printf("I sucked here last night to find new space for the new processes\n");
                        //OK, start the find the process 
                        node_t *least_rec_pro = find_least_rec_pro(to_do_list);
                        //we already find out this process, let's evict it
                        //firstly, we make its load time back to origin
                        least_rec_pro->load_time=least_rec_pro->memory_size_req/2; 
                        long long int this_pro_page = least_rec_pro->page_num;
                        printf("%lld, EVICTED, mem-addresses=", simulator_timer);
                        print_take_up_page(least_rec_pro->take_up_page, least_rec_pro->page_num);
                                                     //the page still need to find
                        //printf("I guess it means we have already find the right process to evict\n");
                        long long int diff=this_pro_page-(page_need-found_page); 
                        long long int i=0; 
                        if(diff==0||diff<0){
                            for(i=0;i<this_pro_page;i++){
                                // printf("i=%lld  ", i);
                                // printf("found_page=%lld\n", found_page);
                                // printf("the old take up ram page is %lld\n",least_rec_pro->take_up_page[i]);
                                // printf("the ram %lld used to be take up by process %lld\n",ram[least_rec_pro->take_up_page[i],least_rec_pro->process_id);
                                curr_process->take_up_page[found_page]=least_rec_pro->take_up_page[i];
                                ram[least_rec_pro->take_up_page[i]]=curr_process->process_id; 
                                least_rec_pro->take_up_page[i]=-1;
                                found_page++; 
                            }
                        }else if(diff>0){
                            for(i=0;i<this_pro_page;i++){
                                if(i<=this_pro_page-page_need){
                                    curr_process->take_up_page[found_page]=least_rec_pro->take_up_page[i];
                                    ram[least_rec_pro->take_up_page[i]]=curr_process->process_id; 
                                    least_rec_pro->take_up_page[i]=-1;
                                    found_page++;      
                                }else{
                                    ram[least_rec_pro->take_up_page[i]]=-1; 
                                    least_rec_pro->take_up_page[i]=-1;
                                    remain_page++; 
                                }
                            }
                        }
                    }
                } 
                //calculate the memory take up percentage
                int memory_take_up_per;
                // I am not sure the precentage is of the process take how much of the whole memory(1)
                // or of how much taked up memory out of the whole memory(2), take (1) at here, can change later
                //memory_take_up_per = ((double)page_need/memory_page)*100; //this is (1)
                memory_take_up_per =  ceil(((double)(memory_page-remain_page)/memory_page)*100); //this is (2)
                printf("%lld, RUNNING, id=%lld, remaining-time=%lld, load-time=%lld, mem-usage=%d%%, mem-addresses=", simulator_timer, curr_process->process_id, curr_process->remain_time, curr_process->load_time, memory_take_up_per);
                //printf("something wrong happened here!!!!\n"); 
                print_take_up_page(curr_process->take_up_page, curr_process->page_num);
                //printf("something wrong happened here!!!!\n"); 
                if(curr_process->load_time==0){
                    curr_process->remain_time--; 
                    curr_process->quantum--; 
                }else{
                    curr_process->load_time--; 
                }
                //printf("Nothing wrong happened here!!!!\n"); 
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
        //printf("time+1 is %lld :\n", simulator_timer);
    }
    free_list(to_do_list);
    return simulator_timer; 
}

long long int 
run_v_on_rr_algorithm(list_t *process_input,long long int memory_size, long long int quantum, statistic_t *statistics){
    long long int interval = 0; 
    long long int simulator_timer = 0; 
    //remain page also equal total memory page minus empty_space_start. 
    //or total memory=empty_space_start+remain_page
    long long int memory_page = memory_size/4; //const
    long long int ram[MAX]; 
    long long int remain_page = memory_page; 
    long long int empyt_space_start = 0;  
    // initialize the all the memory page as -1
    long long int counter=0; 
    for(counter = 0; counter<MAX; counter++){
        ram[counter] = -1;  
    }
    //information about the arriving job  
    long long int num_processes_arrived = 0; 
    list_t *to_do_list = make_empty_list(); 
    node_t *curr_process=NULL; 

    while(!is_empty_list(process_input)){
        //printf("time %lld: \n", simulator_timer);
        //this part is for the statisics 
        if((simulator_timer-1)%60==0&&simulator_timer!=1&&simulator_timer!=0){
            interval++; 
        }
        //keep eye on the arriving process
        num_processes_arrived = check_arrival(simulator_timer, process_input, to_do_list, num_processes_arrived);

        if(curr_process!=NULL && curr_process->remain_time!=0 && curr_process->quantum!=0 && curr_process->load_time!=0){
            /*continue dealing with load time*/
            curr_process->load_time--;  
        }else if(curr_process!=NULL && curr_process->remain_time!=0 && curr_process->quantum!=0 && curr_process->load_time==0){
            /*continue dealing with current process*/
            curr_process->remain_time--;  
            curr_process->quantum--;  
        }else{
            //this is not the first process, then we need to finish the current process
            if(curr_process!=NULL){
                //this process has already finished
                if(curr_process->remain_time==0){
                    //printf("This is the finish part\n");
                    remain_page = remain_page+curr_process->allocated_page;
                    //doesn't change this part
                    empyt_space_start = curr_process->take_up_page[0];
                    printf("%lld, EVICTED, mem-addresses=", simulator_timer);
                    print_take_up_page(curr_process->take_up_page, curr_process->page_num);
                    long long int i=0; 
                    for(i=0; i<curr_process->page_num; i++){
                        long long int assign_page=curr_process->take_up_page[i];
                        if(assign_page!=-1){
                            ram[assign_page] = -1;
                        }
                    }
                    num_processes_arrived--;
                    printf("%lld, FINISHED, id=%lld, proc-remaining=%lld\n", simulator_timer,curr_process->process_id, num_processes_arrived);
                    //record for the statistics
                    sta_t *this_process = create_sta_node(curr_process->time_arrived, curr_process->process_id, curr_process->job_time, simulator_timer,interval);
                    statistics = insert_a_sta(statistics, this_process);
                    //delete from the process input list
                    delete_a_node(process_input,curr_process->process_id);
                    //delete from the todo list 
                    delete_a_node(to_do_list,curr_process->process_id);
                }else{
                    // make it load time as 0, because it has already being loaded inside the ram
                    node_t *next_round_process = create_a_node(curr_process->time_arrived, 
                                                                            curr_process->process_id,
                                                                            curr_process->memory_size_req,
                                                                            curr_process->job_time, 
                                                                            curr_process->remain_time,
                                                                            quantum, 0, 
                                                                            curr_process->page_num, 
                                                                            curr_process->take_up_page, 
                                                                            simulator_timer, 
                                                                            curr_process->allocated_page);
                    //1. delete it from the head of the list
                    delete_a_node(to_do_list,curr_process->process_id);
                    //4. insert at the end of the 
                    to_do_list = insert_at_foot(to_do_list, next_round_process);
                }
            }
            //then load a new one to execute
            if(!is_empty_list(to_do_list)){
                //there are still process need to execute in to-do-list
                //choose a process to do.
                curr_process = to_do_list->head;
                /*assign the memory to process*/
                long long int page_need= curr_process->page_num;
                if(memory_page<page_need){
                    printf("there is not enought space, even the memory is empty!\n"); 
                }
                /*OK let's assign the page for the new process*/
                //create the varible to record the minimum that we need for this process
                long long int min_require=4;//default as 4 pages
                long long int actual_require = curr_process->page_num-curr_process->allocated_page;
                long long int old_page_allocate = curr_process->allocated_page; 
                if(page_need<=4){
                    min_require=page_need-curr_process->allocated_page; 
                }else{
                    min_require=4-curr_process->allocated_page; 
                }
                /* let's start the party */
                //the most exciting condition is that the all the pages the process need has already being allocated 
                if(actual_require==0){
                    //we can do nothing for this section. let's continue
                }else if(remain_page>=min_require){
                    /*assign the empty to this process */
                    long long int ass_num_page=min_require;//default as equal to the min_require 
                    //if there are more empty space, assign to it. 
                    if(actual_require<=remain_page){
                        ass_num_page=actual_require;
                    }else{
                        ass_num_page=remain_page; 
                    }
                    long long int assigned_page=0; 
                    while(assigned_page!=ass_num_page){
                        if(empyt_space_start==memory_page){
                            empyt_space_start=0; 
                        }
                        if(ram[empyt_space_start]==-1){
                            ram[empyt_space_start]=curr_process->process_id;
                            assigned_value_to_empty_page(curr_process->take_up_page,curr_process->page_num,empyt_space_start); 
                            curr_process->take_up_page[assigned_page]=empyt_space_start;
                            curr_process->allocated_page++; 
                            assigned_page++;
                        }
                        empyt_space_start++; 
                    }  
                    remain_page = remain_page-assigned_page; 
                }else{
                    /*which is need evict the process to allocate the current one*/
                    //because the remain page in the memory is must smaller than the mininum require pages 
                    //so assign all the remain page in the memory to the 
                    long long int assigned_page=0;
                    while(assigned_page!=remain_page){
                        if(empyt_space_start==memory_page){
                            //that's end of the pages
                            empyt_space_start=0; 
                        }
                        if(ram[empyt_space_start]==-1){
                            ram[empyt_space_start]=curr_process->process_id;
                            curr_process->take_up_page[assigned_page]=empyt_space_start;
                            curr_process->allocated_page++; 
                            assigned_page++; 
                        }
                        empyt_space_start++;
                    }  
                    //start need to find    
                    long long int* converted_pages_this_time; 
                    converted_pages_this_time = (long long int *)malloc(sizeof(long long int)*min_require);
                    assert(converted_pages_this_time!=NULL);    
                    long long int num_of_page_remove_this_round=0;
                    long long int gate=0;              
                    while(assigned_page<min_require&&gate<3){
                        gate++; 
                        node_t *least_rec_pro = find_least_rec_pro_for_v(to_do_list);
                        /*
                        printf("----------------------------------------------------------------------------------------\n");
                        printf("time at:%lld \n", simulator_timer);
                        print_list(to_do_list);
                        printf("Before converting:\n");
                        printf("the process %lld's page will give to %lld\n", least_rec_pro->process_id, curr_process->process_id);
                        printf("its old page allocation situation is like this:");
                        print_take_up_page(least_rec_pro->take_up_page, least_rec_pro->page_num);
                        printf("its process that need to sign's page allocation situation is like this:");
                        print_take_up_page(curr_process->take_up_page, curr_process->page_num);
                        */
                        long long int num_of_ass = 0; 
                        long long int require_now = min_require-assigned_page;
                        /*printf("the min_require is %lld\n", min_require);
                        printf("the assigned page value now is %lld\n", assigned_page);
                        printf("the page that we want the lea_rec_pro to lect is %lld\n", require_now);
                        */
                        if(least_rec_pro->allocated_page>=require_now){
                            num_of_ass=require_now;
                        }else{
                            num_of_ass=least_rec_pro->allocated_page;
                        }
                        //printf("number of ass need to assign is %lld\n",num_of_ass);
                        long long int counter =0;
                        for(counter=0;counter<num_of_ass;counter++){
                            //printf("counter is %lld\n",counter);
                            long long int remove_page_num=find_none_empty_space(least_rec_pro->take_up_page,least_rec_pro->page_num);
                            assert(remove_page_num!=-1);
                            //printf("the next place's ram space %lld will be converted, ", remove_page_num);
                            //assign to the new process first 
                            assigned_value_to_empty_page(curr_process->take_up_page,curr_process->page_num,least_rec_pro->take_up_page[remove_page_num]);
                            curr_process->allocated_page++; 

                            //record the removed pages
                            converted_pages_this_time[num_of_page_remove_this_round]=least_rec_pro->take_up_page[remove_page_num];
                            num_of_page_remove_this_round++; 
                            //remove from the old procee
                            ram[least_rec_pro->take_up_page[remove_page_num]]=curr_process->process_id;
                            least_rec_pro->take_up_page[remove_page_num]=-1; 
                            least_rec_pro->allocated_page--; 
                            least_rec_pro->load_time=least_rec_pro->load_time+2; 
                            assigned_page++;
                        }
                        /*
                        printf("After converting:\n");
                        printf("the process %lld's page will give to %lld\n", least_rec_pro->process_id, curr_process->process_id);
                        printf("its old page allocation situation is like this:");
                        print_take_up_page(least_rec_pro->take_up_page, least_rec_pro->page_num);
                        printf("the new page page number is %lld\n",curr_process->page_num);
                        print_take_up_page(curr_process->take_up_page, curr_process->page_num);
                        printf("its process that need to sign's page allocation situation is like this:");
                        //printf("----------------------------------------------------------------------------------------\n");
                        //print out the page that being ejected 
                        printf("the converted pages are:\n");
                        printf("%lld number of page is converted\n", num_of_page_remove_this_round);
                        */
                        //printf("----------------------------------------------------------------------------------------\n");
                        
                    } 
                    if(num_of_page_remove_this_round>0){
                        printf("%lld, EVICTED, mem-addresses=", simulator_timer);
                        qsort(converted_pages_this_time, num_of_page_remove_this_round, sizeof(long long int), comp);
                        print_take_up_page(converted_pages_this_time, num_of_page_remove_this_round);
                    }
                    //change the remain page value
                    remain_page = 0; 
                }
                //I need to  dealing with the extra executing time and loading time 
                //calculate the memory take up percentage
                int memory_take_up_per;
                memory_take_up_per = ceil(((double)(memory_page-remain_page)/memory_page)*100);
                long long int page_fault = curr_process->page_num-curr_process->allocated_page;
                curr_process->remain_time=curr_process->remain_time+page_fault;
                long long int new_page_allocate = curr_process->allocated_page;
                long long int new_load_time = llabs(new_page_allocate-old_page_allocate)*2;
                curr_process->load_time=new_load_time;
                qsort(curr_process->take_up_page, curr_process->page_num, sizeof(long long int), comp);
                printf("%lld, RUNNING, id=%lld, remaining-time=%lld, load-time=%lld, mem-usage=%d%%, mem-addresses=", simulator_timer, curr_process->process_id, curr_process->remain_time, curr_process->load_time, memory_take_up_per);
                print_take_up_page(curr_process->take_up_page, curr_process->page_num);
                if(curr_process->load_time==0){
                    curr_process->remain_time--; 
                    curr_process->quantum--; 
                }else{
                    curr_process->load_time--; 
                }
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
    free_list(to_do_list);
    return simulator_timer; 
}
long long int 
check_arrival(long long int simulator_timer, list_t *process_input, list_t *to_do_list, long long int num_processes_arrived){
    long long int old_num = num_processes_arrived;
    //checking the arrival
    node_t *following_process= process_input->head; 
    while(following_process!=NULL){
        //go through the whole list to find the process that arrived at this time 
        if(following_process->time_arrived==simulator_timer){
            //printf("there are processes arrived, its id is %lld\n", following_process->process_id);
            //if the to_do_list is empty
            if(to_do_list->head==NULL){
                //insert into it any way
                node_t *process2 = create_a_node(following_process->time_arrived, 
                                                                following_process->process_id, 
                                                                following_process->memory_size_req, 
                                                                following_process->job_time,
                                                                following_process->remain_time,
                                                                following_process->quantum,
                                                                following_process->load_time,
                                                                following_process->page_num, 
                                                                following_process->take_up_page, 
                                                                following_process->last_stop_time, 
                                                                following_process->allocated_page);
                to_do_list = insert_at_head(to_do_list,process2);
                //printf("check the last stop time is %lld\n", process2->last_stop_time);
            }else{
                node_t *prev = to_do_list->head; 
                node_t *curr = prev->next; 

                //if the head process is the process that arrived at the same time as the current one
                //and the process id is larger than the current one
                if(prev->time_arrived==following_process->time_arrived && following_process->process_id<prev->process_id){
                    //insert it in the head of the list
                    node_t *process1 = create_a_node(following_process->time_arrived, 
                                                                following_process->process_id, 
                                                                following_process->memory_size_req, 
                                                                following_process->job_time,
                                                                following_process->remain_time,
                                                                following_process->quantum,
                                                                following_process->load_time,
                                                                following_process->page_num, 
                                                                following_process->take_up_page, 
                                                                following_process->last_stop_time, 
                                                                following_process->allocated_page);
                    to_do_list = insert_at_head(to_do_list,process1);
                    //printf("check the last stop time is %lld\n", process1->last_stop_time);
                }else{  
                    //let we go throught the 
                    int find_same_arr_process = False; 
                    while(curr!=NULL){
                        if(curr->time_arrived==following_process->time_arrived){
                            find_same_arr_process = True; 
                            if(curr->process_id>following_process->process_id){
                                //insert into the center of the array
                                node_t *process = create_a_node(following_process->time_arrived, 
                                                                following_process->process_id, 
                                                                following_process->memory_size_req, 
                                                                following_process->job_time,
                                                                following_process->remain_time,
                                                                following_process->quantum,
                                                                following_process->load_time,
                                                                following_process->page_num, 
                                                                following_process->take_up_page, 
                                                                following_process->last_stop_time,
                                                                following_process->allocated_page);
                                prev->next = process;
                                process->next = curr;  
                            }else{
                                node_t *process3 = create_a_node(following_process->time_arrived, 
                                                                following_process->process_id, 
                                                                following_process->memory_size_req, 
                                                                following_process->job_time,
                                                                following_process->remain_time,
                                                                following_process->quantum,
                                                                following_process->load_time,
                                                                following_process->page_num, 
                                                                following_process->take_up_page, 
                                                                following_process->last_stop_time, 
                                                                following_process->allocated_page);
                                to_do_list = insert_at_foot(to_do_list,process3);
                            }
                        }
                        prev= prev->next; 
                        curr = curr->next; 
                    }
                    if(!find_same_arr_process){
                        node_t *process4 = create_a_node(following_process->time_arrived, 
                                                                following_process->process_id, 
                                                                following_process->memory_size_req, 
                                                                following_process->job_time,
                                                                following_process->remain_time,
                                                                following_process->quantum,
                                                                following_process->load_time,
                                                                following_process->page_num, 
                                                                following_process->take_up_page, 
                                                                following_process->last_stop_time, 
                                                                following_process->allocated_page);
                        to_do_list = insert_at_foot(to_do_list,process4);
                    }
                }
                
            }
            num_processes_arrived++; 
        }
        following_process=following_process->next;
    }
    //printf("there are %lld processes arrived in this round\n",num_processes_arrived-old_num);
    return num_processes_arrived;
}

node_t *
find_least_rec_pro(list_t *to_do_list){
    int need_replace_head=False; 
    node_t *least_rec_pro = to_do_list->head->next; 
    if(least_rec_pro->last_stop_time==-1||least_rec_pro->load_time!=0){
        need_replace_head = True;
    }
    int earliest_time = least_rec_pro->last_stop_time;
    node_t *search_node = to_do_list->head; 
    while(search_node!=NULL){
        //printf("I was wondering whether is process 1 runed here, process_id =%lld", search_node->process_id);
        if(search_node->last_stop_time!=-1&&search_node->load_time==0){
            if(need_replace_head){
                least_rec_pro=search_node; 
                earliest_time=search_node->last_stop_time;
                need_replace_head=False; 
            }else{
                if(search_node->last_stop_time<earliest_time){
                    least_rec_pro=search_node; 
                    earliest_time=search_node->last_stop_time;
                }
            }
        }
        search_node=search_node->next;
    }
    //printf("the process id: %lld\n", least_rec_pro->process_id);
    //print_take_up_page(least_rec_pro->take_up_page, least_rec_pro->page_num);
    return least_rec_pro; 
}

node_t *
find_least_rec_pro_for_v(list_t *to_do_list){
    int need_replace_head=False; 
    node_t *least_rec_pro = to_do_list->head->next; 
    //to make sure that the evicted page is loaded before, in the other word. it allocated some pages
    if(least_rec_pro->last_stop_time==-1||least_rec_pro->allocated_page==0){
        need_replace_head = True;
    }
    int earliest_time = least_rec_pro->last_stop_time;
    node_t *search_node = to_do_list->head->next; 
    while(search_node!=NULL){
        //printf("I was wondering whether is process 1 runed here, process_id =%lld", search_node->process_id);
        if(search_node->last_stop_time!=-1&&search_node->allocated_page>0){
            if(need_replace_head){
                least_rec_pro=search_node; 
                earliest_time=search_node->last_stop_time;
                need_replace_head=False; 
            }else{
                if(search_node->last_stop_time<earliest_time){
                    least_rec_pro=search_node; 
                    earliest_time=search_node->last_stop_time;
                }
            }
        }
        search_node=search_node->next;
    }
    //printf("the process id: %lld\n", least_rec_pro->process_id);
    //print_take_up_page(least_rec_pro->take_up_page, least_rec_pro->page_num);
    return least_rec_pro; 
}
void assigned_value_to_empty_page(long long int* pages,long long int num_of_page,long long int value){
    long long int page_num =0; 
    for(page_num = 0; page_num<num_of_page; page_num++){
        if(pages[page_num]==-1){
            pages[page_num]=value; 
            return; 
        }
    }
}
long long int find_none_empty_space(long long int* pages,long long int num_of_page){
    long long int page_num=0; 
    for(page_num = 0; page_num<num_of_page; page_num++){
        if(pages[page_num]!=-1){
            return page_num; 
        }
    }
    return -1; 
}
/*functions to read in all the data-----------------------------------------------------*/
void read_in_cmd(char *argv[], int argc,char *cmd_arg[]){
    int count;
    //cmd_arg[0]=>(-f): filename(string)
    //cmd_arg[1]=>(-a): scheduling_algorithm(string)
    //cmd_arg[2]=>(-m): memery_allocation(string)
    //cmd_arg[3]=>(-s): memory_size(int->string)
    //cmd_arg[4]=>(-q): quantum(int->string) 
    int cmd_counter = 0; 
    
    for (count = 1; count < argc; ++count) {
        if(argv[count][0]=='-'){
            switch(argv[count][1]){
                case 'f' :
                    cmd_arg[0] = argv[count+1];
                    break;
                case 'a' :
                    cmd_arg[1] = argv[count+1];
                    break;
                case 'm' :
                    cmd_arg[2] = argv[count+1];
                    break;
                case 's' :
                    cmd_arg[3] = argv[count+1];
                    break;
                case 'q' :
                    cmd_arg[4] = argv[count+1];
                    break;
                default :
                    printf("There are somthing wrong with the input\n" );
                }       
        }
    } 
}

void read_file_into_list(char* filename, list_t *process_input, long long int quantum){
    //open the file
    FILE *fp=fopen(filename,"r"); 

    //check whether the file open successfully
    if(!fp){
        //exit the program if the file open failed
       printf("fail to open the file！\n");
       exit(-1);
    }

    //declare the variable to record 
    long long int time_arrived;                    
    long long int process_id;               
    long long int memory_size_req;                
    long long int job_time;  

    //read the file according to its format and record the value into the list
    while(!feof(fp)){
        //read in the file
        fscanf(fp,"%lld %lld %lld %lld\n",&time_arrived, &process_id, &memory_size_req, &job_time); 
        //create a new node
        long long int load_time=memory_size_req/2;
        long long int num_page = memory_size_req/4;
        node_t *new_process = create_a_node(time_arrived, process_id,
                                            memory_size_req, job_time,
                                            job_time,quantum,
                                            load_time,num_page, NULL, -1,0);
        //put into the list
        process_input = insert_at_foot(process_input, new_process);
    }

}

/*functions to read in all the data-----------------------------------------------------*/
/*functions to performance statistics---------------------------------------------------*/
void perform_statistics(statistic_t *statistics, long long int finish_at){
    /*do the statistic*/

    // long long int time_arrived;                    
    // long long int process_id;                              
    // long long int job_time;   
    // long long int time_complete;  

    long long int interval_frequency[MAX]={0};
    //let's calculate the size of the interval_frequency that we need to use
    long long int total_interval = ((finish_at-1)/60)+1; 
    //initialize all the interval_frequency array and calculate the turnaround time 
    double turnaround_total = 0; 
    double max_individual_turnaround=0; 
    double total_individual_turnaround=0; 
    sta_t *curr = statistics->head; 
    while(curr!=NULL){
        interval_frequency[curr->interval]++; 
        double time_complete_here = curr->time_complete; 
        double time_arrived_here = curr->time_arrived;
        double job_time_here = curr->job_time;
        double current_turnaround = (time_complete_here-time_arrived_here)/(job_time_here); 
        if(current_turnaround>max_individual_turnaround){
            max_individual_turnaround = current_turnaround; 
        }
        total_individual_turnaround = total_individual_turnaround+ current_turnaround; 
        turnaround_total = turnaround_total+(time_complete_here-time_arrived_here);

        curr=curr->next; 
    }
    long long int i = 0; 
    long long int max = interval_frequency[0];
    long long int min = interval_frequency[0];
    long long int total_num_processes = 0; 
    for (i=0; i<total_interval; i++){
        if(interval_frequency[i]<min){
            min = interval_frequency[i];
        }
        if(interval_frequency[i]>max){
            max = interval_frequency[i]; 
        }
        total_num_processes = total_num_processes+interval_frequency[i]; 
    }
    long long int average = ceil((double)total_num_processes/total_interval); 
    long long int average_turnaround_time = ceil(turnaround_total/total_num_processes);
    double average_individual_turnaround_time=total_individual_turnaround/total_num_processes; 
    printf("Throughput %lld, %lld, %lld\n", average, min, max);
    printf("Turnaround time %lld\n", average_turnaround_time);
    printf("Time overhead %.2f %.2f\n",max_individual_turnaround,average_individual_turnaround_time);
    printf("Makespan %lld\n", finish_at);
}
/*functions to performance statistics---------------------------------------------------*/

/*---------------------------------------------------------------------------
   Code that follows is written by Alistair Moffat, as an example for the book
   I changed some code for this project based on his original code.
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.
   See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
   information.
   Prepared December 2012 for the Revised Edition.
----------------------------------------------------------------------------*/
node_t
*create_a_node(long long int time_arrived, long long int process_id, long long int memory_size_req, long long int job_time,long long int remain_time, long long int quantum, long long int load_time, long long int num_page, long long int *take_up_page, long long int last_stop_time, long long int allocated_page){
    node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(new!=NULL);
	new->time_arrived = time_arrived;
    new->process_id = process_id;
    new->memory_size_req = memory_size_req;
    new->job_time = job_time;
    new->remain_time = remain_time;
    new->quantum = quantum;
    new->load_time = load_time;
    new->page_num = num_page; 
    new->allocated_page = allocated_page;
    //if the last stop time equals to zero, which means this process has never being executed before
    new->last_stop_time = last_stop_time; 
    long long int *take_up_p;
    if(take_up_page==NULL){
        take_up_p = (long long int *)malloc(sizeof(long long int)*num_page);
        assert(take_up_p!=NULL);
        long long int i=0;
        for(i=0; i<num_page; i++){
            take_up_p[i]=-1; 
        }
    }else{
        take_up_p=take_up_page; 
    }
    new->take_up_page =take_up_p;
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

long long int
is_empty_list(list_t *list) {
	assert(list!=NULL);
	return list->head==NULL;
}

void
free_list(list_t *list){
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

long long int
get_head(list_t *list) {
	assert(list!=NULL && list->head!=NULL);
	return list->head->process_id;
}

long long int 
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

/* end of of code written by Moffat ------------------------------------------------- */

/*functions for the algorithm datastructure-------------------------------------------*/

void delete_a_node(list_t *list, long long int del_process_id){    
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
            //printf("deleted node %lld\n", del_process_id);
        }else{
            printf("there is no such node in the list\n");
        }
    }else{
        //check if the node that we need to delete is the head
        if(prev->process_id==del_process_id){
            //printf("deleted node %lld\n", del_process_id);
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
                        //printf("deleted node %lld\n", del_process_id);
                        break; 
                    }else{
                        //not the foot node
                        prev->next = curr->next; 
                        //printf("deleted node %lld\n", del_process_id);
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

void 
print_list(list_t *list){
    assert(list!=NULL);
    node_t *curr=list->head;
    printf("start of the list:");
    while(curr!=NULL){
        printf("|process_id=%lld, allocated_num=%lld, last stop time=%lld|->", curr->process_id, curr->allocated_page, curr->last_stop_time);
        curr=curr->next; 
    }
    printf("\n");
}

long long int
length_of_list(list_t *list){
    long long int length = 0; 
    assert(list!=NULL);
    node_t *curr=list->head; 
    while(curr!=NULL){
        length++; 
        curr=curr->next; 
    }
    return length; 
}

void print_take_up_page(long long int* take_up_page, long long int num_of_page){
    //Ok, we need to find out the non -1 start position
    long long int i; 
    for(i=0; i<num_of_page; i++){
        if(take_up_page[i]!=-1){
            break; 
        }
    }
    printf("[");
    printf("%lld",take_up_page[i]);
    long long int counter; 
    for(counter = i+1; counter<num_of_page;counter++){
        printf(",");
        printf("%lld",take_up_page[counter]);
    }
    printf("]\n");
}
/*copy the function from the internet*/
char* itoa(int num,char* str,int radix){
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned unum;
    int i=0,j,k;
    if(radix==10&&num<0)
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;
    do
    {
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
 
    }while(unum);

    str[i]='\0';
    if(str[0]=='-') k=1;
    else k=0;
    char temp;
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}
/*functions for the algorithm datastructure-------------------------------------------*/
/*Functions for preformance statistics data structure---------------------------------*/
sta_t 
*create_sta_node(long long int time_arrived, long long int process_id, long long int job_time, long long int time_complete, long long int interval){
    sta_t *new;
	new = (sta_t*)malloc(sizeof(*new));
	assert(new!=NULL);
	new->time_arrived = time_arrived;
    new->process_id = process_id;
    new->job_time = job_time;
    new->time_complete = time_complete;
    new->interval = interval;
    new->next = NULL; 
	return new; 
}

statistic_t
*insert_a_sta(statistic_t *list, sta_t *new) {
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
statistic_t
*make_a_statisitics(void){
    statistic_t *list;
	list = (statistic_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}
/*Functions for preformance statistics data structure---------------------------------*/