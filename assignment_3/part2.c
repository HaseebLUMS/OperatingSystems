#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "part2.h"
#include "main.h"

const int INTER_ARRIVAL_TIME = 5;
const int NUM_TRAINS = 5;


struct user_station {
	sem_t boarding_sema;
	int waits_on_boarding_sema;
};

struct train_station {
	sem_t alight_sema;
	int waits_on_alight_sema;
};


struct train {
	int total_seats;
	int total_persons_at_the_moment;
	struct train_station * arr_train_stations;
};


struct user_station * arr_user_stations; /*these are the stations where user will come and wait for train*/
struct train * trains; /*these are the trains where each train has an arr of stations where user will tell which station he/she wants to go*/
int * stations_arr_for_train_detection;/*these are the stations list and each train will mark the station number when it reaches at that particular station*/
/*NO NEED to protect stations_arr_for_train_detection as interarrival time of trains is greater than 0, (5 in our case)*/


pthread_mutex_t mutex_waits_on_boarding_sema;
pthread_mutex_t mutex_waits_on_alight_sema;



void initializeP2(int numStations, int maxNumPeople) {
	/*initialize trains*/
	trains = malloc(sizeof(struct train) * NUM_TRAINS);

	int i;
	for(i=0; i< NUM_TRAINS; i++) {
		trains[i].total_seats = maxNumPeople;
		trains[i].total_persons_at_the_moment = 0;
		trains[i].arr_train_stations = malloc(sizeof(struct train_station) * numStations);/*initialize arr_train_stations*/
		int k;
		for(k=0; k< numStations; k++) {
			trains[i].arr_train_stations[k].waits_on_alight_sema = 0;
		}
	}


	/*initialze train detection stations list*/
	stations_arr_for_train_detection = malloc(sizeof(int) * numStations);


	/*initialize arr_user_stations*/
	arr_user_stations = malloc(sizeof(struct user_station) * numStations);
	for(i=0; i< numStations; i++) {
		arr_user_stations[i].waits_on_boarding_sema = 0;
	}

	/*Starting the trains*/
	pthread_t train_thread;
	for(i = 0; i < NUM_TRAINS; i++) {
		int * train_data = malloc(sizeof(int) * 3);
		train_data[0] = numStations;
		train_data[1] = (i % numStations);
		train_data[2] = i;
		pthread_create(& train_thread, NULL, train_f, (void *)train_data);
	}
}


void * goingFromToP2(void * user_data) {
	user_f_2(user_data);
}

void * startP2(){}

void * train_f(void * train_data) {
	int total_stations = ((int *)train_data)[0];
	int starting_from_station = ((int *)train_data)[1];
	int train_number = ((int *)train_data)[2];

	int current_station = starting_from_station;

	sleep(2);
	while(1) {
		stations_arr_for_train_detection[current_station] = train_number;
		/*printf("Train : %d | at station : %d | id@station : %d@%d\n", train_number, current_station, stations_arr_for_train_detection[current_station],current_station);*/
		fflush(stdout);


		pthread_mutex_lock(& mutex_waits_on_boarding_sema);
		pthread_mutex_lock(& mutex_waits_on_alight_sema);


		int total_waits_at_the_moment = arr_user_stations[current_station].waits_on_boarding_sema;
		int total_allowed_at_the_moment = trains[train_number].total_seats - trains[train_number].total_persons_at_the_moment;
		total_allowed_at_the_moment += trains[train_number].arr_train_stations[current_station].waits_on_alight_sema;


		if(total_allowed_at_the_moment > total_waits_at_the_moment) {total_allowed_at_the_moment = total_waits_at_the_moment;}


		while(total_allowed_at_the_moment >= 1) {
			/*printf("Signalling Boarding sema @ station : %d\n", current_station);*/
			sem_post(& arr_user_stations[current_station].boarding_sema);


			/*---------------------------------------------------------*/
			trains[train_number].total_persons_at_the_moment++;
			total_allowed_at_the_moment--;
			arr_user_stations[current_station].waits_on_boarding_sema --;
		}


		while(trains[train_number].arr_train_stations[current_station].waits_on_alight_sema >= 1) {
			/*printf("Signalling Alight sema @ station : %d\n", current_station);*/
			sem_post(& trains[train_number].arr_train_stations[current_station].alight_sema);


			/*----------------------------------------------------------------------------*/
			trains[train_number].total_persons_at_the_moment--;
			trains[train_number].arr_train_stations[current_station].waits_on_alight_sema--;
		}


		pthread_mutex_unlock(& mutex_waits_on_boarding_sema);
		pthread_mutex_unlock(& mutex_waits_on_alight_sema);

		/*-----------------------------------------------------*/
		current_station = (current_station + 1)% total_stations;
		sleep(5);
	}

}


void * user_f_2(void * data) {
	struct argument* casted_data = (struct argument *)data;

	int departure_point = casted_data->from;
	int destination_point = casted_data->to;
	int id = casted_data->id;


	pthread_mutex_lock(& mutex_waits_on_boarding_sema);
	arr_user_stations[departure_point].waits_on_boarding_sema++;
	pthread_mutex_unlock(& mutex_waits_on_boarding_sema);


	/*printf("Wait: boarding_sema | from station : %d | by user: %d\n", departure_point, id);fflush(stdout);*/
	sem_wait(& arr_user_stations[departure_point].boarding_sema);
	/*printf("%d PICKED BY %d\n", id, stations_arr_for_train_detection[departure_point]);fflush(stdout);*/
	/*printf("Train # %d PICKED UP user from : %d | user: %d\n",stations_arr_for_train_detection[departure_point], departure_point, id);fflush(stdout);*/


	pthread_mutex_lock(& mutex_waits_on_alight_sema);
	sleep(1);
	int current_train = stations_arr_for_train_detection[departure_point];
	trains[current_train].arr_train_stations[destination_point].waits_on_alight_sema++;



	/*printf("Wait: alight sema | from station : %d | by user: %d | in train : %d | for loc : %d\n", departure_point, id, current_train, destination_point);fflush(stdout);*/
	pthread_mutex_unlock(& mutex_waits_on_alight_sema);
	sem_wait(& (trains[current_train].arr_train_stations[destination_point].alight_sema));
	/*printf("%d DROPPED BY %d\n", id, current_train);fflush(stdout);*/
	printf("%d %d %d\n", current_train, departure_point, destination_point);fflush(stdout);
	/*printf("Train DROPPED user at : %d | user: %d | train: %d\n", destination_point, id, current_train);fflush(stdout);*/

}
