// ReadMe
// Compile: g++ hw3.cpp -o 3.out -fpermissive -pthread
// Input: ./3.out < input3a.txt

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <pthread.h>
#include <queue>
#include <time.h>

using namespace std;

static pthread_mutex_t traffic_lock;
static pthread_mutex_t mylock;
static pthread_cond_t clear = PTHREAD_COND_INITIALIZER;
static pthread_cond_t wb_can = PTHREAD_COND_INITIALIZER;
static pthread_cond_t bb_can = PTHREAD_COND_INITIALIZER;
static string traffic;
static int maxNoCarsInTunnel = 0;
static int currNoCarsInTunnel = 0;
static int NoCarsWB = 0;
static int NoCarsBB = 0;
static int NoCarsWait = 0;

struct struct_vehicle {
	int id;
	int arrival_time;
	string bound_for;
	int access_time;
};

void *carsToWB(void *arg)
{
	struct_vehicle *vehicle = (struct_vehicle*) arg;
	cout << "Car #" << vehicle->id << " going to Whittier arrives at the tunnel." << endl;
	pthread_mutex_init(&mylock, NULL);
	if (traffic == 'WB')
	{
		// maxNoCarsInTunnel < 3
		if (currNoCarsInTunnel < maximumNoCars)
		{
			cout << "Car #" << vehicle->id << " going to Whittier arrives at the tunnel." << endl;
			pthread_mutex_lock(&mylock);
			currNoCarsInTunnel ++;
			pthread_mutex_unlock(&mylock);

			pthread_mutex_lock(&mylock);
			maxNoCarsInTunnel --;
			pthread_mutex_unlock(&mylock);
		}
		
	}
	else
	{
		// traffic == BB or traffic == N
		// car has to wait
	}
}

void *carsToBB(void *arg)
{
	struct_vehicle *vehicle = (struct_vehicle*) arg;
	cout << "Car #" << vehicle->id << " going to Bear Valley arrives at the tunnel." << endl;
	pthread_mutex_init(&mylock, NULL);
	// if (traffic != 'BB')
	// {
	// 	/* code */
	// }
}
// void *carsToWB(void *arg)
// {

// 	// car is arrival
// 	pthread_mutex_t mutex;
// 	pthread_mutex_init(&mutex, NULL);
	
	
// 	// check the status of tunnel
// 	// check the status of direction 
// 	// if match then
// 	// check the status of capacity
// 		// if not full
// 		// wake up the car and push to the tunnel
// 		// the car increase the capacity
// 		// wake up next car, then sleep access time
// 		// when leaving, decrease the capacity
// 		// and wake up next car
// 	// else wait

// 	struct_vehicle *vehicle = (struct_vehicle*) arg;
// 	if (vehicle->bound_for == "WB")
// 	{
// 		queue_1.push(vehicle->id);
// 		if (queue_1.front == vehicle->id)
// 		{
// 			queue_1.pop();
// 			pthread_mutex_lock(&mutex);
// 			maximumNoCars++;
// 			pthread_mutex_unlock(&mutex);
// 		}
// 		else
// 		{
// 			// wait
// 		}
// 	}
// 	else
// 	{
// 		queue_2.push(vehicle->id);
// 		if (queue_2.front == vehicle->id)
// 		{
// 			queue_2.pop();
// 			pthread_mutex_lock(&mutex);
// 			maximumNoCars++;
// 			pthread_mutex_unlock(&mutex);
// 		}
// 		else
// 		{
// 			// wait
// 		}
// 	}

// 	sleep(vehicle->access_time);

	
// }

void *tunnelstate(void *arg)
{
	int done;
	done = (int) arg;
	cout << "done = " << done << endl;
	pthread_mutex_init(&traffic_lock, NULL);
	// cout << "pthread_mutex_init() rc = " << rc << endl;
	while(done == 0)
	{
		
		pthread_mutex_lock(&traffic_lock);
	
	    traffic = "WB";
		printf("The tunnel is now open to Whittier-bound traffic.\n");
	    pthread_cond_broadcast(&wb_can);
	    pthread_mutex_unlock(&traffic_lock);
	    sleep(5);

	    pthread_mutex_lock(&traffic_lock);
	    traffic = "N";
	    printf("The tunnel is now closed to ALL traffic.\n");
	    pthread_mutex_unlock(&traffic_lock);
	    sleep(5);

	    pthread_mutex_lock(&traffic_lock);
	    traffic = "BB";
		printf("The tunnel is now open to Valley-bound traffic.\n");
	    pthread_cond_broadcast(&bb_can);
	    pthread_mutex_unlock(&traffic_lock);
	    sleep(5);

	    pthread_mutex_lock(&traffic_lock);
	    traffic = "N";
	    printf("The tunnel is now closed to ALL traffic.\n");
	    pthread_mutex_unlock(&traffic_lock);
	    sleep(5);

	    pthread_exit(0);
	}
	// for (::)
	// {
	// 	// 4 status for tunnel
	// 	// 0 for WB, 1 for Deadtime
	// 	// 2 for BB, 3 for Deadtime again
	// 	pthread_mutex_lock(&mutex);
	// 	statusOfTunnel = count % 4;
	// 	pthread_mutex_unlock(&mutex);
	// 	if (statusOfTunnel == 0)
	// 	{
	// 		// send the WB signal here
	// 		// change the value of WB
	// 		whittierBound = 1;
	// 	}

	// 	if (statusOfTunnel == 2)
	// 	{
	// 		// send the BB signal here
	// 		bigbear = 1;
	// 	}

	// 	sleep(5);
	// 	count ++;
	// 	if (count == 4)
	// 	{
	// 		count = 0;
	// 	}
		
	// 	// count ++;
	// 	// pthread_mutex_lock(&mutex);
	// 	// statusOfTunnel = count % 3;
	// 	// pthread_mutex_unlock(&mutex);
	// 	// sleep(5);
	// 	// if (count == 3)
	// 	// {
	// 	// 	count = 0;
	// 	// }
	// 	// if something happen
	// 	// break the for loop, then exit the thread
	// }

	
}

int main(int argc, char *argv[]) {
	struct_vehicle vehicleList[100];
	pthread_t tid[100];	// ID
	pthread_t tunnel;
	int count = 0;
	int totalNCars;
	
	// bridgeMaxLoad = atoi(argv[1]) ;
	// cout << "Maximum bridge load is " << bridgeMaxLoad << " tons." << endl;
	// string plate;
	// int weight, arrival_delay, processTime;

	int tunnelCapacity;
	int arrival_time, access_time;
	string bound_for;

	cin >> tunnelCapacity;

	while (!cin.eof()) {

		count++;
		cin >> arrival_time;
		cin >> bound_for;
		cin >> access_time;
		vehicleList[count].id = count;
		vehicleList[count].arrival_time = arrival_time;
		vehicleList[count].bound_for = bound_for;
		vehicleList[count].access_time = access_time;
	}
	cout <<"Maximum number of cars in the tunnel: " << tunnelCapacity << endl;

	for (int i = 1; i <= count; i++) {
		cout << vehicleList[i].id << " ";
		cout << vehicleList[i].arrival_time << " ";
		cout << vehicleList[i].bound_for << " ";
		cout << vehicleList[i].access_time;
		cout << endl;
	}

	maxNoCarsInTunnel = tunnelCapacity;
	totalNCars = count;
	// create tunnel pthread, update the tunnel state
	cout << "create tunnel thread" << endl;
	pthread_create(&tunnel, NULL, tunnelstate, (void*) 0);
	
	for (int i = 1; i <= totalNCars; i++)
	{
		// cTime += vehicleList[i].arrival_delay;
		// sleep 
		sleep(vehicleList[i].arrival_time);
		if (vehicleList[i].bound_for == "WB")
		{
			pthread_create(&tid[i], NULL, carsToWB, (void *) &vehicleList[i]);
		}
		else
		{
			pthread_create(&tid[i], NULL, carsToBB, (void *) &vehicleList[i]);
		}
	}

	for (int i = 1; i <= totalNCars; i++)
	{
		pthread_join(tid[i], NULL);
	}
	cout << "Total number of vehicles: " << totalNCars << endl;
	pthread_exit(0);
	return 0;
}