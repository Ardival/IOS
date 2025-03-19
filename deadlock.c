// IOS projekt c.2
// Juraj Budai - xbudai02

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

long unsigned NZ;   // count of customers
long unsigned NU;   //count of officers
long unsigned TZ;   //usleep
long unsigned TU;   //usleep
long unsigned F;    //usleep

long unsigned *A = NULL;        //count of lines which are printed to file
long unsigned *queue1 = NULL;   // how many customers are in queue1
long unsigned *queue2 = NULL;   // how many customers are in queue2
long unsigned *queue3 = NULL;   // how many customers are in queue3
bool *opened = NULL;            //open or close of post

sem_t *mutex = NULL;     
sem_t *customer1 = NULL;    //customers in queue1
sem_t *customer2 = NULL;    //customers in queue2
sem_t *customer3 = NULL;    //customers in queue3
sem_t *officer1 = NULL;     // semafore for picking from queue
sem_t *officer2 = NULL;     // semafore for picking from queue
sem_t *officer3 = NULL;     // semafore for picking from queue
sem_t *customer_done = NULL;      
sem_t *officer_done = NULL;
sem_t *queue = NULL;

FILE *out = NULL;

int read_args(int argc, char **argv) //validation of arguments and assign of global variables
{
    if (argc != 6)
    {
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }
    for (int i=1; i<argc; i++)
    {
        for (int i = 1; i < 6; i++) {
            char *p;
            strtol(argv[i], &p, 10);
            if (*p != '\0') {
                fprintf(stderr, "Error: Argument %d is not a number\n", i);
                return 1;
            }
        }
        int arg_value = atoi(argv[i]);

        switch (i)
        {
        case 1:
            NZ = arg_value;
            if (arg_value < 1)
            {
                fprintf(stderr, "Error: Negative numbers are not supported\n");
                return 1;
            }
            break;
        case 2:
            NU = arg_value;
            if (arg_value < 1)
            {
                fprintf(stderr, "Error: Negative numbers are not supported\n");
                return 1;
            }
            break;
        case 3:
            TZ = arg_value;
            if (arg_value > 10000 || arg_value < 0)
            {
                fprintf(stderr, "Error: Argument 3 is out of interval\n");
                return 1;
            }
            break;
        case 4:
            TU = arg_value;
            if (arg_value > 100 || arg_value < 0)
            {
                fprintf(stderr, "Error: Argument 4 is out of interval\n");
                return 1;
            }
            break;
        case 5:
            F = arg_value;
            if (arg_value > 10000 || arg_value < 0)
            {
                fprintf(stderr, "Error: Argument 5 is out of interval\n");
                return 1;
            }
            break;
        }
    }
    return 0;
}
// inicialization of shared memory
int init_of_shared_memory () {

    // allocating shared memory for semaphores
    if((mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((customer1 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((customer2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((customer3 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((officer1 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((officer2 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((officer3 = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((customer_done = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((officer_done = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((queue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;

    // allocating shared memory for variables
    if((queue1 = mmap(NULL, sizeof(long unsigned), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1; 
    if((queue2 = mmap(NULL, sizeof(long unsigned), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((queue3 = mmap(NULL, sizeof(long unsigned), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((A = mmap(NULL, sizeof(long unsigned), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;
    if((opened = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) return -1;

    //opening of file
    out = fopen ("proj2.out", "w");
    if(out == NULL){
        fprintf(stderr, "Error: Can not create a proj2.out file.\n");
        return -1;
    }
    
    //inicialization of semaphores
    if(sem_init(mutex, 1, 1) == -1) return -1;
    if(sem_init(customer1, 1, 0) == -1) return -1;
    if(sem_init(customer2, 1, 0) == -1) return -1;
    if(sem_init(customer3, 1, 0) == -1) return -1;
    if(sem_init(officer1, 1, 0) == -1) return -1;
    if(sem_init(officer2, 1, 0) == -1) return -1;
    if(sem_init(officer3, 1, 0) == -1) return -1;
    if(sem_init(customer_done, 1, 1) == -1) return -1;
    if(sem_init(officer_done, 1, 0) == -1) return -1;
    if(sem_init(queue, 1, 1) == -1) return -1;

    return 0;
}

void clean_memory_semaphores (){

    //closign of file
    if(out != NULL){
        fclose(out);
    }

    //destroying semaphores
    sem_destroy(mutex); munmap(mutex, sizeof(sem_t));
    sem_destroy(customer1); munmap(customer1, sizeof(sem_t));
    sem_destroy(customer2); munmap(customer2, sizeof(sem_t));
    sem_destroy(customer3); munmap(customer3, sizeof(sem_t));
    sem_destroy(officer1); munmap(officer1, sizeof(sem_t));
    sem_destroy(officer2); munmap(officer2, sizeof(sem_t));
    sem_destroy(officer3); munmap(officer3, sizeof(sem_t));
    sem_destroy(customer_done); munmap(customer_done, sizeof(sem_t));
    sem_destroy(officer_done); munmap(officer_done, sizeof(sem_t));
    sem_destroy(queue); munmap(queue, sizeof(sem_t));

    //freeing allocated memory
    munmap(queue1, sizeof(long unsigned));
    munmap(queue2, sizeof(long unsigned));
    munmap(queue3, sizeof(long unsigned));
    munmap(A, sizeof(long unsigned));
    munmap(opened, sizeof(bool));
}
void serving_service (int service, int idU){

    //wainting for customer to be served
    if (service == 1){sem_wait(customer1); sem_post(officer1);}
    else if (service == 2){sem_wait(customer2); sem_post(officer2);}
    else if (service == 3){sem_wait(customer3); sem_post(officer3);}
    sem_wait(mutex);
    fprintf (out, "%lu: U %d: serving a service of type %d\n", ++(*A), idU, service);
    fflush (out);
    sem_post(mutex);
    int procesing = (rand () % 11); //waiting, serving customer
    usleep ((procesing)*1000);
    sem_wait(mutex);
    fprintf (out, "%lu: U %d: service finished\n", ++(*A), idU);
    fflush (out);
    sem_post(mutex);
    sem_wait (customer_done);
    sem_post(officer_done);
}

int mail_man (int idU, int TU)
{
    sem_wait(mutex);
    fprintf (out, "%lu: U %d: started\n", ++(*A), idU);
    fflush (out);
    sem_post(mutex);
    while ((*queue1) > 0 ||(*queue2) > 0 || (*queue3) > 0 || *opened) //the post must be open or the queue is not empty
    { 
        int case_num5 = (rand () % 3 + 1); //random case to be served
        sem_wait(queue);
        if (case_num5 == 3){
            if ((*queue3) > 0){
                serving_service (3, idU);
                (*queue3)--;
            }
            else if ((*queue2) > 0) {
                serving_service (2, idU);
                (*queue2)--;
            }
            else if ((*queue1) > 0) {
                serving_service (1, idU);
                (*queue1)--;
            }
        }
        else if (case_num5 == 2){
            if ((*queue2) > 0){
                serving_service (2, idU);
                (*queue2)--;
            }
            else if ((*queue1) > 0) {
                serving_service (1, idU);
                (*queue1)--;
            }
            else if ((*queue3) > 0) {
                serving_service (3, idU);
                (*queue3)--;
            }
        }
        else {
            if ((*queue1) > 0){
                serving_service (1, idU);
                (*queue1)--;
            }
            else if ((*queue2) > 0) {
                serving_service (2, idU);
                (*queue2)--;
            }
            else if ((*queue3) > 0) {
                serving_service (3, idU);
                (*queue3)--;
            }
        }
        sem_post(queue);
        if ((*opened) && ((*queue1) == 0 && (*queue2) == 0 && (*queue3) == 0)){ // if the queue is empty the officer is taking break
            sem_wait(mutex);
            fprintf (out, "%lu: U %d: taking break\n", ++(*A), idU);
            fflush (out);
            sem_post(mutex);
            int resting = (rand () % (TU));
            usleep ((resting)*1000);
            sem_wait(mutex);
            fprintf (out, "%lu: U %d: break finished\n", ++(*A), idU);
            fflush (out);
            sem_post(mutex);
        }
    }

    //when the post is closed and no one is in the queue, officer is going home
    sem_wait(mutex);
    fprintf (out, "%lu: U %d: going home\n", ++(*A), idU);
    fflush (out);
    sem_post(mutex);
    return 0;
}

void *customer_1 (int idZ, int TZ){
    sem_wait(mutex);
    fprintf (out, "%lu: Z %d: started\n", ++(*A), idZ);
    fflush (out);
    sem_post(mutex);
    int sleep_interval = (rand () % TZ);
    usleep ((sleep_interval)*1000);

    if (*opened) { // if the post is opened
        int case_num = (rand () % 3 + 1);

        //new customer will join the queue
        if (case_num == 1){sem_wait(queue); *queue1 += 1; sem_post(queue);} 
        else if (case_num == 2){sem_wait(queue); *queue2 += 1; sem_post(queue);}
        else if (case_num == 3){sem_wait(queue); *queue3 += 1; sem_post(queue);}
        sem_wait(mutex);
        fprintf (out, "%lu: Z %d: entering office for a service %d\n", ++(*A), idZ, case_num);
        fflush (out);
        sem_post(mutex);

        //waiting for officer
        if (case_num == 1){sem_post(customer1); sem_wait (officer1);}
        else if (case_num == 2){sem_post(customer2); sem_wait (officer2);}
        else if (case_num == 3){sem_post(customer3); sem_wait (officer3);}
        sem_wait(mutex);
        fprintf (out, "%lu: Z %d: called by office worker\n", ++(*A), idZ); 
        fflush (out);
        sem_post(mutex);
        sem_post(customer_done);
        sem_wait(officer_done);
        sem_wait(mutex);
        fprintf (out, "%lu: Z %d: going home\n", ++(*A), idZ);
        fflush (out);
        sem_post(mutex);
    }
    else{ // if the post is closed, customer will not be served
        sem_wait(mutex);
        fprintf (out, "%lu: Z %d: going home\n", ++(*A), idZ);
        fflush (out);
        sem_post(mutex);
    }
    return 0;
}

int main (int argc, char *argv[])
{
     int E_rror = 0;
     read_args(argc, argv); 
     if((E_rror = init_of_shared_memory()) != 0){
        fprintf(stderr, "Error: Mapping of shared memory crashed.\n");
        return 1;
    }
    *opened = true; // post is opened
    *queue1 = 0; // inicialization
    *queue2 = 0;
    *queue3 = 0;
    srand(time(NULL));

    int id = fork();
    if (id == 0){
        int num = (rand () % (F - (F/2) + 1)) + (F/2);
        usleep ((num)*1000);
        *opened = false; //post is closeing
        sem_wait(mutex);
        fprintf (out,"%lu: closing\n", ++(*A));
        fflush (out);
        sem_post(mutex);
    }
    else { //creating officer and customer processes
        for(int i = 1; i < (int)NZ+1; i++)
        {
            pid_t pid = fork();
            if(pid == 0) 
            {
                srand(getpid());
                customer_1(i, TZ);
                exit(0);
            }
            else if (pid < 0){
                fprintf(stderr, "Error: Creating process failed\n");
                exit(1);
            }
        }
        for(int j = 1; j < (int)NU+1; j++)
        {
            pid_t pid = fork();
            if(pid == 0) 
            {
                srand(getpid());
                mail_man(j, TU);
                exit(0);
            }
            else if (pid < 0){
                fprintf(stderr, "Error: Creating process failed\n");
                exit(1);
            }
        }
    }
    while (wait(NULL)>0);
    clean_memory_semaphores (); //cleaning the allocated memory and destroying semaphores
    return 0;
}
