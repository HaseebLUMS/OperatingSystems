#include <stdio.h>
#include <semaphore.h>
#include "part1.h"
#include "main.h"

/*
	- DO NOT USE SLEEP FUNCTION
	- Define every helper function in .h file
	- Use Semaphores for synchronization purposes
 */

const int MAX_NUM_FLOORS = 20;

/**
 * Do any initial setup work in this function.
 * numFloors: Total number of floors elevator can go to | will be smaller or equal to MAX_NUM_FLOORS
 * maxNumPeople: The maximum capacity of the elevator
 * 
 * Two elevators should start, one from the ground floor (elevator 1) and
 * the other one from the top floor (elevator 2).
 *
 * Note that: Elevator 1 starts from ground floor and goes up
 * Elevator 2 starts from Top floor and goes down.
 */
void initializeP1(int numFloors, int maxNumPeople) {
	return;
}

/**
 * Every passenger will call this function when 
 * he/she wants to take the elevator. (Already
 * called in main.c)
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
 * the print statements (info) of passengers in elevator 2.
 */
void* goingFromToP1(void *arg) {
	return NULL;
}

/*If you see the main file, you will get to 
know that this function is called after setting every
passenger.

So use this function for starting your elevators. In 
this way, you will be sure that all passengers are already
waiting for elevators.
*/
void startP1(){
}