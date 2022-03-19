/*
 * Author: benevolent professor - Prof Victor Norman
 * Completed by: David Yoo (gy24)
 * Date: Mar 17, 2022
 * 
 * SeongChan Cho(sc77) and I visited professor Norman for some advices, and based on those ideas we started the project.
 * And while doing this project, me and SeonChan shared a lot of our ideas.(so some of the algorithms or codes may be similar) 
 * Some of the codes in this projects are built based on the codes on the class slides.
 * Below are the work cited that I used during this project.
 * Work cited:
 * semaphore: https://www.geeksforgeeks.org/use-posix-semaphores-c/ and class slides
 * bounded buffer: class slides
 * rendezvous: class slides
 * intpr_t: https://stackoverflow.com/questions/9042024/error-unknown-type-name-intptr-t
 *          https://stackoverflow.com/questions/35071200/what-is-the-use-of-intptr-t
 * creating thread: https://www.geeksforgeeks.org/multithreading-c-2/
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h> // for intptr_t used in main()

#define NUM_VIALS 30
#define SHOTS_PER_VIAL 6
#define NUM_CLIENTS (NUM_VIALS * SHOTS_PER_VIAL)
#define NUM_NURSES 10
#define NUM_STATIONS NUM_NURSES
#define NUM_REGISTRATIONS_SIMULTANEOUSLY 4


/* global variables */
sem_t NurseGivesShot [NUM_STATIONS];
sem_t ClentReady [NUM_STATIONS];
sem_t register_Semaphore;
sem_t buffer_full;
sem_t buffer_mutex;
sem_t buffer_empty;
sem_t vial;

int num_vials_left =  NUM_VIALS;
int station[NUM_STATIONS];
int in = 0;
int out = 0;



int get_rand_in_range(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

char *time_str;
char *curr_time_s() {
    time_t t;
    time(&t);
    time_str = ctime(&t);
    // replace ending newline with end-of-string.
    time_str[strlen(time_str) - 1] = '\0';
    return time_str;
}


// lower and upper are in seconds.
void walk(int lower, int upper) {
    usleep(get_rand_in_range(lower * 1000000 , upper * 1000000));
}



// arg is the nurses station number.
void *nurse(void *arg) {
    long int id = (long int)arg;
    fprintf(stderr, "%s: nurse %ld started\n", curr_time_s(), id);

    while (1){
        // walk a random amount of time between 1 and 3 seconds to get a vial of vaccine
        fprintf(stderr, "%s: nurse %ld walking to get a vial\n", curr_time_s(), id);
        walk(1,3);

        // if there are no more vials, leave the clinic.
        if (num_vials_left == 0){
            fprintf(stderr, "%s: nurse %ld found all vials are gone: exiting\n", curr_time_s(), id);
            fprintf(stderr, "%s: nurse %ld is done\n", curr_time_s(), id);
            pthread_exit(NULL);
        }
        
        // decrement the number of vials
        sem_wait (&vial);
        num_vials_left--;
        fprintf(stderr, "%s: nurse %ld got vial. Num Vials left = %d.\n", curr_time_s(), id, num_vials_left);

        // walk back (again between 1 and 3 seconds)
        sem_post(&vial);
        walk(1,3);
        fprintf(stderr, "%s: nurse %ld back at station\n", curr_time_s(), id);

        // 1 vial = 6 shots
        int shots_available = 6;
        while (shots_available != 0) {
            // indicate to the queue of clients waiting for a station assignment that you are ready for the next client.
            fprintf(stderr, "%s: nurse %ld tells the waiting queue that she/he is available\n",curr_time_s(),id);
            sem_wait(&buffer_empty);
            sem_wait(&buffer_mutex);
            station[in] = id;
            in = (in + 1) % NUM_STATIONS;
            sem_post(&buffer_mutex);
            sem_post(&buffer_full);

            // wait for the client to indicate that they are ready to be vaccinated
            fprintf(stderr, "%s: nurse %ld waiting for a client to arrive.\n",curr_time_s(),id);
            sem_wait(&ClentReady[id]);

            // give the client a vaccination (which takes 5 seconds to administer)
            fprintf(stderr, "%s: nurse %ld sees client is ready. Giving shot now.\n",curr_time_s(),id);
            usleep(5000000);
            sem_post(&NurseGivesShot[id]);
            fprintf(stderr, "%s: nurse %ld gave the client the shot\n",curr_time_s(),id);
            shots_available --; // decreament the number of shots available
        }      
    }
}


    

void *client(void *arg) {
    long int id = (long int)arg;
    // after arrival (i.e., thread creation), walk a random amount between 3 and 10 seconds to the registration desk.
    fprintf(stderr, "%s: client %ld has arrived and is walking to register\n", curr_time_s(), id);
    walk(3,10);

    // wait for an opening at the registration desk.
    fprintf(stderr, "%s: client %ld waiting to register\n", curr_time_s(), id);
    sem_wait(&register_Semaphore);

    // take a random amount of time between 3 and 10 seconds to register.
    fprintf(stderr, "%s: client %ld is registering\n", curr_time_s(), id);
    usleep(get_rand_in_range(3000000,10000000));
    sem_post(&register_Semaphore);
    
    // walk a random amount of time between 3 and 10 seconds to get to the station-assignment queue.
    fprintf(stderr, "%s: client %ld is done registering, now walking to the next queue.\n", curr_time_s(), id);
    walk(3,10);
    
    // wait for station assignment
    sem_wait(&buffer_full);
    sem_wait(&buffer_mutex);

    int nurseID = station[out];
    out = (out+1)%NUM_STATIONS;
    sem_post(&buffer_mutex);
    sem_post(&buffer_empty);

    // walk a random amount of time between 1 and 2 seconds to get to the assigned nurse's station.
    fprintf(stderr, "%s: client %ld got assigned to station %d: walking there now.\n", curr_time_s(), id, nurseID);
    walk(1,2);
    fprintf(stderr, "%s: client %ld is at station %d.\n", curr_time_s(), id, nurseID);

    // indicate to the nurse that the client is ready for the vaccination
    sem_post(&ClentReady[nurseID]);
    fprintf(stderr, "%s: client %ld is ready for the shot from nurse %d.\n", curr_time_s(), id, nurseID);
    
    // wait for the nurse to complete the vaccination
    sem_wait(&NurseGivesShot[nurseID]);
    fprintf(stderr, "%s: client %ld got the shot!.\n", curr_time_s(), id);

    //leave the clinic
    fprintf(stderr, "%s: client %ld leaves the clinic.\n", curr_time_s(), id);
    pthread_exit(NULL);
        
}


int main() {

    srand(time(0));
    // initialize the semaphore
    sem_init (&register_Semaphore, 0, NUM_REGISTRATIONS_SIMULTANEOUSLY);
    sem_init (&vial, 0,1);
    sem_init (&buffer_full, 0, 0);
    sem_init (&buffer_mutex,0,1);
    sem_init (&buffer_empty, 0, NUM_STATIONS);

    //rendezvous 
    for (int i =0; i < NUM_STATIONS; i++)
    {
        sem_init(&ClentReady[i], 0, 0);
        sem_init(&NurseGivesShot[i], 0, 0);
    }
    
    // Thread for nurses and clients
    pthread_t nurseThread[NUM_NURSES];
    pthread_t clientThread[NUM_CLIENTS];
    // nurse threads
    for (int i = 0 ; i < NUM_NURSES; i++){
        pthread_create(&nurseThread[i], NULL, nurse, (void*)(intptr_t) i);
    }
    // client threads
    for (int i = 0 ; i < NUM_CLIENTS; i++){
        pthread_create(&clientThread[i], NULL, client, (void*)(intptr_t) i);
        usleep(get_rand_in_range(0,1000000));
    }

    pthread_exit(0);
}