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

static pthread_mutex_t traffic_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t clear = PTHREAD_COND_INITIALIZER;
static pthread_cond_t wb_can = PTHREAD_COND_INITIALIZER;
static pthread_cond_t bb_can = PTHREAD_COND_INITIALIZER;
static int done = 0;
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
	pthread_mutex_lock(&traffic_lock);
	cout << "Car #" << vehicle->id << " going to Whittier arrives at the tunnel." << endl;

	
	while (traffic != "WB" || currNoCarsInTunnel == maxNoCarsInTunnel)
	{
		if (traffic == "WB" && currNoCarsInTunnel == maxNoCarsInTunnel)
		{
			NoCarsWait ++;
		}
		pthread_cond_wait(&wb_can, &traffic_lock);
	}
	pthread_mutex_unlock(&traffic_lock);
	pthread_mutex_lock(&mylock);
	cout << "Car #" << vehicle->id << " going to Whittier enter the tunnel." << endl;
	NoCarsWB ++;
	currNoCarsInTunnel ++;
	pthread_mutex_unlock(&mylock);
	sleep(vehicle->access_time);

	pthread_mutex_lock(&mylock);
	currNoCarsInTunnel --;
	cout << "Car #" << vehicle->id << " going to Whittier exits the tunnel." << endl;

	if (currNoCarsInTunnel == maxNoCarsInTunnel - 1)
	{
		pthread_cond_signal(&wb_can);
	}
	pthread_mutex_unlock(&mylock);
		
}

void *carsToBB(void *arg)
{
	struct_vehicle *vehicle = (struct_vehicle*) arg;
	pthread_mutex_lock(&traffic_lock);
	cout << "Car #" << vehicle->id << " going to Bear Valley arrives at the tunnel." << endl;
	while (traffic != "BB" || currNoCarsInTunnel == maxNoCarsInTunnel)
	{
		if (traffic == "BB" && currNoCarsInTunnel == maxNoCarsInTunnel)
		{
			NoCarsWait ++;
		}
		pthread_cond_wait(&bb_can, &traffic_lock);	
	}
	pthread_mutex_unlock(&traffic_lock);

	pthread_mutex_lock(&mylock);
	cout << "Car #" << vehicle->id << " going to Bear Valley enter the tunnel." << endl;
	NoCarsBB ++;
	currNoCarsInTunnel ++;
	pthread_mutex_unlock(&mylock);

	sleep(vehicle->access_time);

	pthread_mutex_lock(&mylock);
	currNoCarsInTunnel --;
	cout << "Car #" << vehicle->id << " going to Bear Valley exits the tunnel." << endl;
	if (currNoCarsInTunnel == maxNoCarsInTunnel - 1)
	{
		pthread_cond_signal(&bb_can);
	}
	pthread_mutex_unlock(&mylock);
}

void *tunnelstate(void *arg)
{
	while(1)
	{
		if (done == 1)
			break;
		pthread_mutex_lock(&traffic_lock);
	    traffic = "WB";
		printf("The tunnel is now open to Whittier-bound traffic.\n");
	    pthread_cond_broadcast(&wb_can);
	    pthread_mutex_unlock(&traffic_lock);
	    sleep(5);

	    if (done == 1)
			break;
	    pthread_mutex_lock(&traffic_lock);
	    traffic = "N";
	    printf("The tunnel is now closed to ALL traffic.\n");
	    pthread_mutex_unlock(&traffic_lock);
	    sleep(5);

	    if (done == 1)
			break;
	    pthread_mutex_lock(&traffic_lock);
	    traffic = "BB";
		printf("The tunnel is now open to Valley-bound traffic.\n");
	    pthread_cond_broadcast(&bb_can);
	    pthread_mutex_unlock(&traffic_lock);
	    sleep(5);

	    if (done == 1)
			break;
	    pthread_mutex_lock(&traffic_lock);
	    traffic = "N";
	    printf("The tunnel is now closed to ALL traffic.\n");
	    pthread_mutex_unlock(&traffic_lock);
	    sleep(5);

	}
}

int main(int argc, char *argv[]) {
	struct_vehicle vehicleList[100];
	pthread_t tid[100];	// ID
	pthread_t tunnel;
	int count = 0;
	int totalNCars;
	
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

	// for (int i = 1; i <= count; i++) {
	// 	cout << vehicleList[i].id << " ";
	// 	cout << vehicleList[i].arrival_time << " ";
	// 	cout << vehicleList[i].bound_for << " ";
	// 	cout << vehicleList[i].access_time;
	// 	cout << endl;
	// }

	maxNoCarsInTunnel = tunnelCapacity;
	totalNCars = count;
	// create tunnel pthread, update the tunnel state

	pthread_create(&tunnel, NULL, tunnelstate, (void*) 0);
	
	for (int i = 1; i <= totalNCars; i++)
	{
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
	done = 1;
	pthread_join(tunnel, NULL);

	cout << endl;
	cout << NoCarsWB << " cars(s) going to Whittier arrived at the tunnel." << endl;
	cout << NoCarsBB << " cars(s) going to Bear Valley arrived at the tunnel." << endl;
	cout << NoCarsWait << " cars(s) had to wait because the tunnel was full." << endl;
	pthread_exit(0);
	return 0;
}