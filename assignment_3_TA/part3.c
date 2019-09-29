#include "part3.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "main.h"

#define RED 0
#define GREEN 1

struct signal {
	sem_t left_semaphore;
	sem_t right_semaphore;
	sem_t turn_semaphore;
	int waits_on_left_semaphore;
	int waits_on_right_semaphore;
	int waits_on_turn_semaphore;
	int light; /*0 for RED, 1 for GREEN*/
};

sem_t north_road_semaphore;
sem_t south_road_semaphore;
sem_t east_road_semaphore;
sem_t west_road_semaphore;

struct signal * traffic_signal;
/*
	0 = NORTH
	1 = SOUTH
	2 = EAST
	3 = WEST
*/


pthread_mutex_t waits_mutex;


void initializeP3() {
	traffic_signal = malloc(sizeof(struct signal) * 4);
	int i;
	for(i=0; i<  4; i++) {
		traffic_signal[i].light = RED;
	}

	sem_post(& north_road_semaphore);
	sem_post(& south_road_semaphore);
	sem_post(& east_road_semaphore);
	sem_post(& west_road_semaphore);

	pthread_t north_signal;
	pthread_t east_signal;
	pthread_t south_signal;
	pthread_t west_signal;


	pthread_create(&north_signal, NULL, north_signal_f, NULL);
	pthread_create(&east_signal, NULL, east_signal_f, NULL);
	pthread_create(&south_signal, NULL, south_signal_f, NULL);
	pthread_create(&west_signal, NULL, west_signal_f, NULL);

	/*pthread_join(north_signal, NULL);
	pthread_join(east_signal, NULL);
	pthread_join(south_signal, NULL);
	pthread_join(west_signal, NULL);*/
	/*koi sa signal green krna he*/


}


void * goingFromToP3(void *argu){
	struct argumentP3 * car_data = (struct argumentP3 *)argu;


	int from = car_data->from;
	int to = car_data->to;
	int lane = car_data->lane;
	int user_id = car_data->user_id;

	if(lane == 0) {
		if(from == 0){sem_wait(& north_road_semaphore);}	
		if(from == 1){sem_wait(& south_road_semaphore);}	
		if(from == 2){sem_wait(& east_road_semaphore);}	
		if(from == 3){sem_wait(& west_road_semaphore);}	
	}


	if(lane == 1) {
		pthread_mutex_lock(& waits_mutex);
		traffic_signal[from].waits_on_right_semaphore ++;
		pthread_mutex_unlock(& waits_mutex);

		sem_wait(& traffic_signal[from].right_semaphore);
	}
	if(lane == 0 && !is_left_turn(from, to)) {
		pthread_mutex_lock(& waits_mutex);
		traffic_signal[from].waits_on_left_semaphore ++;
		pthread_mutex_unlock(& waits_mutex);

		sem_wait(& traffic_signal[from].left_semaphore);
	}


	printf("%s %s %s\n", directions(from), directions(to), lanes(lane));


	if(lane == 0) {
		if(from == 0){sem_post(& north_road_semaphore);}	
		if(from == 1){sem_post(& south_road_semaphore);}	
		if(from == 2){sem_post(& east_road_semaphore);}	
		if(from == 3){sem_post(& west_road_semaphore);}
	}



}

void startP3(){
	sem_post(& traffic_signal[0].turn_semaphore);
}

void * north_signal_f() {

	while(1) {
		sem_wait(& (traffic_signal[0].turn_semaphore));
		//printf("NORTH\n");


		//printf("Before\n");
		//printf("After\n");
		pthread_mutex_lock(& waits_mutex);
		int t_waits = traffic_signal[0].waits_on_left_semaphore;
		int tr_waits = 0; //rights waits to signal
		pthread_mutex_unlock(& waits_mutex);


		if(t_waits >= 5){t_waits = 5;}
		else{tr_waits = 5 - t_waits;}

		int r_waits = traffic_signal[0].waits_on_right_semaphore;
		if(r_waits < tr_waits) {tr_waits = r_waits;}

		while(t_waits) {
			sem_post(& traffic_signal[0].left_semaphore);
			sleep(1);
			t_waits--;

		}
		while(tr_waits) {
			sem_post(& traffic_signal[0].right_semaphore);
			sleep(1);
			tr_waits--;
		}

		sem_post(& traffic_signal[2].turn_semaphore);
		
	}
}


void * east_signal_f() {


	while(1) {
		sem_wait(& (traffic_signal[2].turn_semaphore));
		//printf("EAST\n");

		pthread_mutex_lock(& waits_mutex);
		int t_waits = traffic_signal[2].waits_on_left_semaphore;
		int tr_waits = 0; //rights waits to signal

		if(t_waits >= 5){t_waits = 5;}
		else{tr_waits = 5 - t_waits;}

		int r_waits = traffic_signal[2].waits_on_right_semaphore;
		pthread_mutex_unlock(& waits_mutex);


		if(r_waits < tr_waits) {tr_waits = r_waits;}

		while(t_waits) {
			sem_post(& traffic_signal[2].left_semaphore);
			sleep(1);
			t_waits--;
		}
		while(tr_waits) {
			sem_post(& traffic_signal[2].right_semaphore);
			sleep(1);
			tr_waits--;
		}

		sem_post(& traffic_signal[1].turn_semaphore);
	}
}


void * south_signal_f() {


	while(1) {
		sem_wait(& (traffic_signal[1].turn_semaphore));
		//printf("SOUTH\n");

		pthread_mutex_lock(& waits_mutex);
		int t_waits = traffic_signal[1].waits_on_left_semaphore;
		int tr_waits = 0; //rights waits to signal

		if(t_waits >= 5){t_waits = 5;}
		else{tr_waits = 5 - t_waits;}

		int r_waits = traffic_signal[1].waits_on_right_semaphore;
		if(r_waits < tr_waits) {tr_waits = r_waits;}
		pthread_mutex_unlock(& waits_mutex);

		while(t_waits) {
			sem_post(& traffic_signal[1].left_semaphore);
			sleep(1);
			t_waits--;
		}
		while(tr_waits) {
			sem_post(& traffic_signal[1].right_semaphore);
			sleep(1);
			tr_waits--;
		}

		sem_post(& traffic_signal[3].turn_semaphore);
	}
}


void * west_signal_f() {


	while(1) {
		sem_wait(& (traffic_signal[3].turn_semaphore));
		//printf("WEST\n");


		pthread_mutex_lock(& waits_mutex);
		int t_waits = traffic_signal[3].waits_on_left_semaphore;
		int tr_waits = 0; //rights waits to signal

		if(t_waits >= 5){t_waits = 5;}
		else{tr_waits = 5 - t_waits;}

		int r_waits = traffic_signal[3].waits_on_right_semaphore;
		if(r_waits < tr_waits) {tr_waits = r_waits;}
		pthread_mutex_unlock(& waits_mutex);

		while(t_waits) {
			sem_post(& traffic_signal[3].left_semaphore);
			sleep(1);
			t_waits--;
		}
		while(tr_waits) {
			sem_post(& traffic_signal[3].right_semaphore);
			sleep(1);
			tr_waits--;
		}

		sem_post(& traffic_signal[0].turn_semaphore);
	}
}

int is_left_turn(int from, int to) {
	if(from == 0 && to == 2){return 1;}
	if(from == 1 && to == 3){return 1;}
	if(from == 2 && to == 1){return 1;}
	if(from == 3 && to == 0){return 1;}
	return 0;
}


char* directions(int i){
	char* dir[4];
	dir[0] = "NORTH";
	dir[1] = "SOUTH";
	dir[2] = "EAST";
	dir[3] = "WEST";
	return dir[i];
}


char* lanes(int i){
	char* lane[2];
	lane[0] = "LEFT";
	lane[1] = "RIGHT";
	return lane[i];
}
