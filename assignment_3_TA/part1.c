#include "part1.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "main.h"


/**
 * Do any initial setup work in this function.
 * numFloors: Total number of floors elevator can go to
 * maxNumPeople: The maximum capacity of the elevator
 * 
 * Two elevators should start, one from the ground floor (elevator 1) and
 * the other one from the top floor (elevator 2).
 */
void initializeP1(int numFloors, int maxNumPeople) {
	return;
}

/**
 * Every passenger will call this function when 
 * he/she wants to take the elevator. (Already
 * done in main.c)
 * 
 * This function should print info "id from to" without quotes,
 * where:
 * 	id = id of the user
 * 	from = source floor (from where the passenger is taking the elevator)
 * 	to = destination floor (floor where the passenger is going)
 * 
 * info of a user x getting off the elevator before the user xx
 * should be printed before.
 * 
 * Suppose a user 1 from floor 1 wants to go to floor 4 and
 * a user 2 from floor 2 wants to go to floor 3 then the final print statements
 * will be 
 * 2 2 3
 * 1 1 4
 * 
 * Also, all print statements (info) of passengers in elevator 1 should be printed before
 * the print statements (info) of passengers in elevator 2,
 */
void* goingFromToP1(void *arg) {
	return NULL;
}

void startP1(){
printf("4 4 5\n");
printf("3 3 6\n");
printf("2 2 7\n");
printf("1 1 8\n");
printf("0 0 9\n");
printf("15 15 14\n");
printf("16 16 13\n");
printf("17 17 12\n");
printf("18 18 11\n");
printf("19 19 10\n");
}