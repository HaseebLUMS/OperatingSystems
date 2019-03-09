#include "part1.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "main.h"



#define UP 1
#define DOWN 2



struct floors_data {
	sem_t source_sema;
	sem_t destination_sema;

	/*Need to protect (i.e lock) these variables as well*/
	int waits_on_source_sema;
	int waits_on_destination_sema;
};



int persons_in_elevator_at_the_moment = 0;
struct floors_data * floors_arr;
pthread_mutex_t mutex_persons_in_elevator_at_the_moment;
pthread_mutex_t mutex_waits_on_destination_sema;
pthread_mutex_t mutex_waits_on_source_sema;


/**
 * Do any initial setup work in this function.
 * numFloors: Total number of floors elevator can go to
 * maxNumPeople: The maximum capacity of the elevator
 */
void initializeP1(int numFloors, int maxNumPeople) {
	int total_floors = numFloors;
	int cap_of_elevator = maxNumPeople;

	/*printf("Total Floors are : %d\n", total_floors);*/
	floors_arr = (struct floors_data *)malloc(sizeof(struct floors_data) * numFloors);


	int i;
	for(i = 0; i < numFloors; i++) {
		floors_arr[i].waits_on_source_sema = 0;
		floors_arr[i].waits_on_destination_sema = 0;
	}


	pthread_t elevator_thread;

	int * data = malloc(sizeof(int) * 2);
	data[0] = total_floors;
	data[1] = cap_of_elevator;
	/*printf("Total floors told to thread are: %d\n", data[0]);*/

	int status = pthread_create(&elevator_thread, NULL, elevator_f, (void *) data);
	/*if(status == -1) {printf("%s\n", "Failed: elevator thread");}else{printf("%s\n", "Creadted: elevator thread");}*/


	/*printf("%s\n", "Back to initializeP1");*/

	
	return;
}

/**
 * This program should print data in the specific format
 * described below:
 * If there is a passenger (id 5) going from floor 2 to 4, followed
 * by a passenger (id 6) from 2 to 5, the output should be:
 * 5 2 4
 * 6 2 5
 * i.e. there should be a single space, and each journey should be 
 * on a new line. Incorrectly formatted output will get 0 marks
 */
void* goingFromToP1(void *arg) {
   	/*printf("%d %d %d\n", ((int *)user_data)[0], ((int *)user_data)[1], ((int *)user_data)[2]);*/

	user_f(arg);

	return NULL;
}

void startP1(){}


void * elevator_f (void* elevator_data) {
	int total_floors = ((int *)elevator_data)[0];
	int cap_of_elevator = ((int *)elevator_data)[1];
	int current_floor = 0; 
	int direction_of_elevator = UP;
	

	sleep(2);/*so that all users can reach the elevator*/


	while(1) {
		/*printf("On floor : %d | going to : %d | total floors : %d\n", current_floor, direction_of_elevator, total_floors);*/


		pthread_mutex_lock(& mutex_waits_on_destination_sema);
		pthread_mutex_lock(& mutex_waits_on_source_sema);
		pthread_mutex_lock(& mutex_persons_in_elevator_at_the_moment);

		int person_allowed_to_enter = cap_of_elevator - persons_in_elevator_at_the_moment;
		person_allowed_to_enter += floors_arr[current_floor].waits_on_destination_sema;


		if(person_allowed_to_enter > floors_arr[current_floor].waits_on_source_sema) {
			person_allowed_to_enter = floors_arr[current_floor].waits_on_source_sema;
		}
		
		while(person_allowed_to_enter >= 1) {
			/*printf("Signalling SOURCE sema of floor %d\n", current_floor);*/
			sem_post(& floors_arr[current_floor].source_sema);


			persons_in_elevator_at_the_moment++;
			person_allowed_to_enter--;
			floors_arr[current_floor].waits_on_source_sema = floors_arr[current_floor].waits_on_source_sema - 1;
		}

		while(floors_arr[current_floor].waits_on_destination_sema >= 1) {
			/*printf("Signalling DESTINATION sema of floor %d\n", current_floor);*/
			sem_post(& floors_arr[current_floor].destination_sema);

			
			persons_in_elevator_at_the_moment--;		
			floors_arr[current_floor].waits_on_destination_sema = floors_arr[current_floor].waits_on_destination_sema - 1;
		}

		pthread_mutex_unlock(& mutex_waits_on_source_sema);
		pthread_mutex_unlock(& mutex_waits_on_destination_sema);
		pthread_mutex_unlock(& mutex_persons_in_elevator_at_the_moment);
		/*---------------------------------------------*/
		if(direction_of_elevator == UP) {
			current_floor++;
			if(current_floor >= total_floors) {
				current_floor = total_floors - 2;
				direction_of_elevator = DOWN;
			}
		}
		else {
			current_floor--;
			if(current_floor < 0) {
				current_floor = 1;
				direction_of_elevator = UP;
			}
		}

		sleep(1);
	}
}


void * user_f(void * user_data) {
	/*int my_destination = ((int *)user_data)[2];
	int my_source = ((int *)user_data)[1];
	int user_id = ((int *)user_data)[0];*/

	int my_destination = ((struct argument*)user_data)->to;
	int my_source = ((struct argument*)user_data)->from;
	int user_id = ((struct argument*)user_data)->id;


	/*printf("Calling wait on source | cf: %d\n", my_source);*/


	pthread_mutex_lock(& mutex_waits_on_source_sema);
	floors_arr[my_source].waits_on_source_sema++;
	pthread_mutex_unlock(& mutex_waits_on_source_sema);


	sem_wait(& (floors_arr[my_source].source_sema));
	/*printf("PICKED up %d | from floor %d\n", user_id, my_source);*/





	pthread_mutex_lock(& mutex_waits_on_destination_sema);
	floors_arr[my_destination].waits_on_destination_sema++;
	pthread_mutex_unlock(& mutex_waits_on_destination_sema);


	/*printf("Calling wait on destination floor %d | cf %d\n", my_destination, my_source);*/
	sem_wait(& (floors_arr[my_destination].destination_sema));
	/*printf("Dropped : %d | cf: %d\n", user_id, my_destination);
	printf("\n\n\n");*/
	fflush(stdout);
	printf("%d %d %d\n",user_id, my_source, my_destination);fflush(stdout);

	return NULL;
}
