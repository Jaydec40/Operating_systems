/*    
Jayden Decena
cs-301-001
Project 4: office hours:locks and condition variables


 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "mytime.h"
//#include <sephamore.h>

#define MAX_STUDENTS 100
//initialize mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t teacher_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t student_cond = PTHREAD_COND_INITIALIZER;

int waiting = 0; // set students waiting to 0
int total_chairs; 
int student_total; //total students waiting
int left_time, right_time;

int *buffer; // Added buffer variable

// Function to generate random time within a given interval
int mytime(int left, int right) {
    int time = 0; 
    time = left + rand() % (right - left);
    printf("random time is %d sec\n", time);
    return time; 
}

// Function for student threads
void *student_thread(void *arg) {
    int id = *((int *) arg);
    int help_count = 0;
    
    while (help_count < 2) { // Each student seeks help twice
        printf("Student %d to sleep\n", id);
        int sleep_time = mytime(left_time, right_time); // Generate random sleep time
        sleep(sleep_time);
        printf("Student %d wake up\n", id);
        
        pthread_mutex_lock(&mutex); // Lock mutex before accessing shared resources
        printf("Student %d will call mutex lock\n", id);
        
        if (waiting < total_chairs) { // If there are chairs available
            waiting++;
            printf("Student %d sits on a chair. Total chairs occupied: %d\n", id, waiting);
            pthread_cond_signal(&teacher_cond); // Signal teacher that student is waiting
            pthread_mutex_unlock(&mutex); // Unlock mutex after signaling
            
            pthread_mutex_lock(&mutex); // Lock mutex again before unlocking
            printf("Student %d will call mutex unlock\n", id);
            pthread_mutex_unlock(&mutex); // Unlock mutex after relocking
        } else {
            printf("Student %d goes back to study\n", id);
            pthread_mutex_unlock(&mutex); // Unlock mutex if no chairs available
        }
        
        help_count++;
    }
    
    printf("Student %d finished getting help\n", id);
    pthread_exit(NULL); // Exit thread after getting help twice
}

// Function for teacher thread
void *teacher_thread(void *arg) {
    while (1) { // Teacher keeps running indefinitely
        pthread_mutex_lock(&mutex); // Lock mutex before accessing shared resources
        printf("Teacher will call mutex lock\n");
        
        if (waiting == 0) { // If no students are waiting
            printf("No students waiting, teacher goes back to own work\n");
            pthread_cond_wait(&teacher_cond, &mutex); // Wait for signal from students
        }
        
        printf("Teacher helps student\n");
        waiting--;
        pthread_mutex_unlock(&mutex); // Unlock mutex after helping student
        printf("Teacher will call mutex unlock\n");
        
        sleep(mytime(left_time, right_time)); // Simulate teacher helping a student
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) { // Check if correct number of command-line arguments provided
        printf("Usage: %s <student_total> <total_chairs> <left_time> <right_time>\n", argv[0]);
        return 1;
    }
    
    // Parse command-line arguments
    student_total = atoi(argv[1]); // Changed num_students to student_total
    total_chairs = atoi(argv[2]); // Changed chairs_available to total_chairs
    left_time = atoi(argv[3]);
    right_time = atoi(argv[4]);
    
    // Validate input arguments
    if (student_total <= 0 || student_total > MAX_STUDENTS || total_chairs <= 0 || left_time >= right_time) {
        printf("Invalid input. Please try again with correct format.\n");
        return 1;
    }
    
    srand(time(NULL)); // Seed the random number generator
    
    pthread_t teacher;
    pthread_t students[student_total]; // Changed num_students to student_total
    int student_ids[student_total]; // Changed num_students to student_total
    
    // Create teacher thread
    pthread_create(&teacher, NULL, teacher_thread, NULL);
    
    // Create student threads
    for (int i = 0; i < student_total; i++) { // Changed num_students to student_total
        student_ids[i] = i + 1;
        pthread_create(&students[i], NULL, student_thread, &student_ids[i]);
    }
    
    // Wait for all student threads to finish
    for (int i = 0; i < student_total; i++) { // Changed num_students to student_total
        pthread_join(students[i], NULL);
    }
    
    // Cancel teacher thread and wait for it to finish
    pthread_cancel(teacher);
    pthread_join(teacher, NULL);
    
    return 0;
}

