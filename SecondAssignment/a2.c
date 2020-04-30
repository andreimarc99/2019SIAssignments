#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>
#define N1 5 // THE NUMBER OF THREADS NEEDED IN THE THIRD TASK (5)
#define N2 45 // THE NUMBER OF THREADS NEEDED IN THE FOURTH TASK (45)

typedef struct arg1 {
    int arg;
} ThArg1;

void ret() {
    exit(1);
}

pthread_t ths[5];
// SEMAPHORES NEEDED FOR TASK 3, INITIALIZED
// IN MAIN FURTHER DOWN
sem_t sem1_t3, sem2_t3;

// THREAD FUNCTION FOR THE THIRD TASK
void* thread_function1(void* arg) {
    ThArg1* args = (ThArg1 *) arg;
    //int secondEnded = 0;

    if (1 == args->arg) {
        ;
    } else {
        sem_wait(&sem1_t3);
    }

    info(BEGIN, 7, args->arg);
    
    for (int i = 0; i < N1 - 1; ++i) {
        sem_post(&sem1_t3);
    }

    if (2 == args->arg) {
        ;
    } else {
        sem_wait(&sem2_t3);
    }
    info(END, 7, args->arg);
    for(int i = 0; i < N1 - 1; ++i) {
        sem_post(&sem2_t3);
    }
    
    return NULL;
}

pthread_t ths4[45];
// SEMAPHORES NEEDED FOR TASK 4, INITIALIZED
// IN MAIN FURTHER DOWN
sem_t sem1_t4, sem2_t4, sem3_t4;
// THREAD FUNCTION FOR THE FOURTH TASK
void* thread_function2(void* arg) {
    ThArg1* args = (ThArg1 *) arg;

    if (10 == args->arg) {
        ;
    } else {
        sem_wait(&sem2_t4);
    }

    sem_wait(&sem1_t4);

    info(BEGIN, 2, args->arg);

    for (int i = 0; i < N2 - 1; ++i) {
        sem_post(&sem2_t4);
    }

    if (10 == args->arg) {
        ;
    }
    else {
        sem_wait(&sem3_t4);
    }

    info(END, 2, args->arg);

    for (int i = 0; i < N2 - 1; ++i) {
        sem_post(&sem3_t4);
    }

    sem_post(&sem1_t4);

    return NULL;
}

int main(){

    init();
    // MAIN PROCESS
    info(BEGIN, 1, 0);
        // TASK 2 - CREATING THE HIERARCHY
        int pid2, pid3, pid4, pid5, pid8, pid7, pid6;
        // EACH PARENT WAITS FOR ITS KID(S)
        pid2 = fork();
        if (pid2 == -1) {
            perror("ERROR");
            return -1;
        } else if (pid2 == 0) {
            info(BEGIN, 2, 0);
            pid4 = fork();
            if (pid4 == -1) {
                perror("ERROR");
                return -1;
            } else if (pid4 == 0) {
                info(BEGIN, 4, 0);
                pid6 = fork();
                if (pid6 == -1) {
                    perror("ERROR");
                    return -1;
                } else if (pid6 == 0) {
                    info(BEGIN, 6, 0);
                    info(END, 6, 0);
                    ret();
                }
                waitpid(pid6, NULL, 0);
                info(END, 4, 0);
                ret();
            }
            
            pid5 = fork();
            if (pid5 == -1) {
                perror("ERROR");
                return -1;
            } else if (pid5 == 0) {
                info(BEGIN, 5, 0);
                info(END, 5, 0);
                ret();
            }

            pid8 = fork();
            if (pid8 == -1) {
                perror("ERROR");
                return -1;
            } else if (pid8 == 0) {
                info(BEGIN, 8, 0);
                info(END, 8, 0);
                ret();
            }
            
            // INITIALIZING THE GLOBAL SEMAPHORES
            sem_init(&sem1_t4, 0, 6);
            sem_init(&sem2_t4, 0, 0);
            sem_init(&sem3_t4, 0, 0);
            ThArg1 args[45];
            for (int i = 0; i < N2; ++i) {
                args[i].arg = i + 1;
            }
            // CREATING 45 THREADS USING THE IMPLEMENTED THREAD FUNCTION
            for (int i = 0; i < N2; ++i) {
                pthread_create(&ths4[i], NULL, thread_function2, &args[i]);
            }
            // WAITING FOR THE THREADS
            for (int i = 0; i < N2; ++i) {
                pthread_join(ths4[i], NULL);
            }
            

            sem_destroy(&sem1_t3);
            sem_destroy(&sem2_t3);

            waitpid(pid4, NULL, 0);
            waitpid(pid5, NULL, 0);
            waitpid(pid8, NULL, 0);
            info(END, 2, 0);
            
            ret();
        }

        pid3 = fork();
        if (pid3 == -1) {
            perror("ERROR");
            return -1;
        } else if (pid3 == 0) {
            info(BEGIN, 3, 0);
            info(END, 3, 0);
            ret();
        }
        
        pid7 = fork();
        if (pid7 == -1) {
            perror("ERROR");
            return -1;
        } else if (pid7 == 0) {
            // PROCESS NUMBER 7
            // THIS IS WHERE WE SOLVE TASK 3
            info(BEGIN, 7, 0);
            // INITIALIZING THE GLOBAL SEMAPHORES
            sem_init(&sem1_t3, 0, 0);
            sem_init(&sem2_t3, 0, 0);
            // WE PLACE THE INDEXES IN A STRUCTURE
            ThArg1 args[5];

            for (int i = 0; i < N1; ++i) {
                args[i].arg = i + 1;
            }
            // CREATING 5 THREADS USING THE IMPLEMENTED THREAD FUNCTION
            for (int i = 0; i < N1; ++i) {
                pthread_create(&ths[i], NULL, thread_function1, &args[i]);
            }
            // WAITING FOR THE THREADS
            for (int i = 0; i < N1; ++i) {
                pthread_join(ths[i], NULL);
            }

            sem_destroy(&sem1_t3);
            sem_destroy(&sem2_t3);

            info(END, 7, 0);
            
            ret();
        }
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
    waitpid(pid7, NULL, 0);

    info(END, 1, 0);
    ret();
    
    return 0;
}

