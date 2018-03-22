#include<iostream>
#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fstream>
#include<string>
#include<pthread.h>
#include <queue>
#include<time.h>

using namespace std;

static pthread_mutex_t mutex;
static int maximumNoCars = 0;
static int statusOfTunnel = 0;


queue<int> queue_1;
queue<int> queue_2;

struct struct_vehicle {
	int id;
	int arrival_time;
	string bound_for;
	int access_time;
};

void *car(void *arg)
{
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	

	struct_vehicle *vehicle = (struct_vehicle*) arg;
	if (vehicle->bound_for == "WB")
	{
		queue_1.push(vehicle->id);
		if (queue_1.front == vehicle->id)
		{
			queue_1.pop();
			pthread_mutex_lock(&mutex);
			maximumNoCars++;
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			// wait
		}
	}
	else
	{
		queue_2.push(vehicle->id);
		if (queue_2.front == vehicle->id)
		{
			queue_2.pop();
			pthread_mutex_lock(&mutex);
			maximumNoCars++;
			pthread_mutex_unlock(&mutex);
		}
		else
		{
			// wait
		}
	}

	sleep(vehicle->access_time);

	
}

void *tunnelstate(void *arg)
{
	int count = arg;
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

	for (::)
	{
		count ++;
		pthread_mutex_lock(&mutex);
		statusOfTunnel = count % 3;
		pthread_mutex_unlock(&mutex);
		sleep(5);
		if (count == 3)
		{
			count = 0;
		}
		// if something happen
		// break the for loop, then exit the thread
	}

	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	struct_vehicle vehicleList[100];
	pthread_t tid[100];	// ID
	pthread_t tunnel;
	int count = 0;
	
	int rc;
	// bridgeMaxLoad = atoi(argv[1]) ;
	// cout << "Maximum bridge load is " << bridgeMaxLoad << " tons." << endl;
	// string plate;
	// int weight, arrival_delay, processTime;

	int maximum_no_cars;
	int arrival_time, access_time;
	string bound_for;

	cin >> maximum_no_cars;

	while (!cin.eof()) {
		cin >> arrival_time;
		cin >> bound_for;
		cin >> access_time;
		vehicleList[count].id = count + 1;
		vehicleList[count].arrival_time = arrival_time;
		vehicleList[count].bound_for = bound_for;
		vehicleList[count].access_time = access_time;
		count++;
	}

	cout << maximum_no_cars << endl;

	for (int i = 0; i < count; i++) {
		cout << vehicleList[count].id << " ";
		cout << vehicleList[i].arrival_time << " ";
		cout << vehicleList[i].bound_for << " ";
		cout << vehicleList[i].access_time;
		cout << endl;
	}

	maximumNoCars = maximum_no_cars;

	// create tunnel pthread, update the tunnel state
	// every 5s, 1: WB, 2: deadtime, 3: BB
	rc = pthread_create(&tunnel, NULL, tunnelstate, (void*) 0);
	asset(rc == 0);

	// create car pthread
	int carid;
	for (int i = 0; i < count; i++)
	{
		
		if (vehicleList[i].bound_for == "WB")
			queue_1.push(vehicleList[i].id);
		else
			queue_2.push(vehicleList[i].id);

		sleep(vehicleList[i].arrival_time);

		if (statusOfTunnel == 1)
		{
			if (maximumNoCars < 10)
			{
				if (!queue_1.empty())
				{
					carid = queue_1.front();
					queue_1.pop();
					
					rc = pthread_create(&carid, NULL, car, (void *) &vehicleList[carid]);
					asset(rc == 0);
				}
			}
		}
		else if (statusOfTunnel == 3)
		{
			if (maximumNoCars <= 10)
			{
				if (!queue_1.empty())
				{
					carid = queue_2.front();
					queue_2.pop();
					
					rc = pthread_create(&carid, NULL, car, (void *) &vehicleList[carid]);
					asset(rc == 0);
				}
			}
		}
	}
	

	// for (int i = 0; i < count; i++)
	// {
	// 	cTime += vehicleList[i].arrival_delay;
	// 	sleep(vehicleList[i].arrival_delay);
	// 	pthread_create(&tid[i], NULL, vehicle, (void *) &vehicleList[i]);
	// }
	for (int i = 0; i < count; i++)
	{
		pthread_join(tid[i], NULL);
	}
	// cout << "Total number of vehicles: " << count << endl;
	// pthread_exit(0);
	return 0;
}