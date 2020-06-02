#include<stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <math.h>


#define True    1
#define False   0
#define MAX 100000


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
	long long int time_arrived;                    
    long long int process_id;               
    long long int memory_size_req;                
    long long int job_time;   
    long long int remain_time;     
    long long int quantum; 
    long long int load_time;//load time = page_num/2 = memory_size/2
    long long int page_num;//page_num = memory_size/4
    long long int *take_up_page;                                       
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
long long int check_arrival(long long int simulator_timer, list_t *process_input, list_t *to_do_list, long long int num_processes_arrived);

/*node operation*/
node_t *create_a_node(long long int time_arrived, long long int process_id, long long int memory_size_req, long long int job_time, long long int remain_time, long long int quantum,long long int page_num, long long int load_time, long long int* take_up_page); 
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

    }

    /*do the performance statistics*/
    perform_statistics(statistics, finish_at);
    free(process_input);
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
    free(to_do_list);
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
                                                                quantum, 0, 0, NULL);
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
    free(to_do_list);
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
                curr_process->load_time=curr_process->memory_size_req/2; 
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
                    memory_take_up_per = ((double)page_need/memory_page)*100; 
                }else{
                    printf("The memory is empty but there is still no space for the process!\n");
                } 

                printf("%lld, RUNNING, id=%lld, remaining-time=%lld, load-time=%lld, mem-usage=%d%%, mem-addresses=", simulator_timer, curr_process->process_id, curr_process->remain_time,curr_process->load_time,memory_take_up_per);
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
    free(to_do_list);
    return simulator_timer;
}

long long int 
run_p_on_rr_algorithm(list_t *process_input,long long int memory_size, long long int quantum, statistic_t *statistics){
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
                                                                quantum, 0, 0, NULL);
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
    free(to_do_list);
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
                                                                following_process->take_up_page);
                to_do_list = insert_at_head(to_do_list,process2);
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
                                                                following_process->take_up_page);
                    to_do_list = insert_at_head(to_do_list,process1);
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
                                                                following_process->take_up_page);
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
                                                                following_process->take_up_page);
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
                                                                following_process->take_up_page);
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
                                            load_time,num_page, NULL);
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
*create_a_node(long long int time_arrived, long long int process_id, long long int memory_size_req, long long int job_time,long long int remain_time, long long int quantum, long long int load_time, long long int num_page, long long int*take_up_page){
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
    long long int *take_up_p = (long long int *)malloc(sizeof(long long int)*num_page);
    assert(take_up_p!=NULL);
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
        printf("|process_id=%lld, remain time=%lld|->", curr->process_id, curr->remain_time);
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
    printf("[");
    printf("%lld",take_up_page[0]);
    int counter = 0; 
    for(counter = 1; counter<num_of_page;counter++){
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