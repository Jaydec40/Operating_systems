/*    
Jayden Decena
cs-301-001
Project 4: office hours:sephamore


 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "mytime.h"

#define MAX_STUDENTS 100

sem_t teacher_sem;
sem_t student_sem;
int waiting = 0;
int total_chairs;
int student_total;
int left_time, right_time;

int *buffer;

/*
int mytime(int left, int right) {
    int time = 0;
    time = left + rand() % (right - left);
    printf("random time is %d sec\n", time);
    return time;
}
*/
// Function for student threads
void *student_thread(void *arg) {
    int id = *((int *) arg);
    int help_count = 0;

    while (help_count < 2) { //seek help twice
        printf("Student %d to sleep\n", id);
        int sleep_time = mytime(left_time, right_time); // generate random sleep time
        sleep(sleep_time);
        printf("Student %d wake up\n", id);

        sem_wait(&student_sem); // Wait on student semaphore
        if (waiting < total_chairs) { 
            waiting++;
            printf("Student %d sits on a chair. Total chairs occupied: %d\n", id, waiting);
            sem_post(&teacher_sem); // signal teacher that student is waiting
        } else {
            printf("Student %d goes back to study\n", id);
        }
        sem_post(&student_sem); // Release student semaphore

        help_count++;
    }

    printf("Student %d finished getting help\n", id);
    pthread_exit(NULL); // leave thread after getting help twice
}

// Function for teacher thread
void *teacher_thread(void *arg) {
    while (1) { // Teacher keeps running 
        sem_wait(&teacher_sem); // Wait on teacher semaphore
        if (waiting == 0) { // If no students are waiting
            printf("No students waiting, teacher goes back to own work\n");
        } else {
            printf("Teacher helps student\n");
            waiting--;
        }
        sem_post(&teacher_sem); // Release teacher semaphore

        sleep(mytime(left_time, right_time)); // teacher is helping a student
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) { 
        printf("Usage: %s <student_total> <total_chairs> <left_time> <right_time>\n", argv[0]);
        return 1;
    }

    //command-line arguments
    student_total = atoi(argv[1]); 
    total_chairs = atoi(argv[2]); 
    left_time = atoi(argv[3]);
    right_time = atoi(argv[4]);

    // Validate input arguments
    if (student_total <= 0 || student_total > MAX_STUDENTS || total_chairs <= 0 || left_time >= right_time) {
        printf("Invalid input. Please try again with correct format.\n");
        return 1;
    }

    srand(time(NULL)); // random number generator

    pthread_t teacher;
    pthread_t students[student_total]; 
    int student_ids[student_total]; 

    // Initialize semaphores
    sem_init(&teacher_sem, 0, 0);
    sem_init(&student_sem, 0, 1);

    // Create teacher thread
    pthread_create(&teacher, NULL, teacher_thread, NULL);

    // Create student threads
    for (int i = 0; i < student_total; i++) { 
        student_ids[i] = i + 1;
        pthread_create(&students[i], NULL, student_thread, &student_ids[i]);
    }

    // Wait for all student threads to finish
    for (int i = 0; i < student_total; i++) { 
        pthread_join(students[i], NULL);
    }

    // Cancel teacher thread and wait for it to finish
    pthread_cancel(teacher);
    pthread_join(teacher, NULL);

    // Destroy semaphores
    sem_destroy(&teacher_sem);
    sem_destroy(&student_sem);

    return 0;
}

