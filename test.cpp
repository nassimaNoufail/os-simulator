// c++ 11 standard
// Zhenggang Li UHID:1543212

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <queue>

using namespace std;


map<string, vector<pair<string, int> > > resource_state_table;
queue<int> ready_queue;
queue<int> ssd_queue;
queue<int> input_queue;

// event list including process id, process state or device, and time of event
struct EventList
{
    int p_id;
    string event;
    int endtime;

    EventList(const int& p_id, const string& event, const int& endtime): p_id(p_id),event(event),endtime(endtime)
    {}
};

vector<EventList> event_list;

// sort event list by endtime
struct is_early
{
	bool operator()(const EventList& x, const EventList& y) const
	{
		return x.endtime < y.endtime;
	}
};

// read instructions from file
void read_fuction(vector <pair<string, string> > &read_from_file)
{
	string str;
	string value;
	// read file line by line, save each line in a paired vector
	while (cin >> str)
	{
		cin >> value;
		read_from_file.push_back(make_pair(str, value));
	}
}

// read each process from read_from_file, save it in map with vector
void split_process_and_save(map< int, vector<pair<string, int> > > &process, vector <pair<string, string> > &read_from_file)
{
	int flag = 0;
	int pNum = 0;
	vector<pair<string, int> > operation;
	for (auto & ele : read_from_file)
	{	
		// save the amount of cores
		if(ele.first == "NCORES")
		{
			operation.push_back(make_pair(ele.first, stoi(ele.second)));
			process[-1] = operation;
			operation.clear();
			continue;
		}

		// save the NEW process time
		else if (ele.first == "NEW")
		{
			if(flag % 2 == 0)
			{
				//cout << "I'm here" << endl;
				if(flag != 0)
				{
					process[pNum] = operation;
					pNum++;
				}

				event_list.push_back(EventList(flag, "Arrival", stoi(ele.second)));
			}
			else
			{
				process[pNum] = operation;

				operation.clear();
				event_list.push_back(EventList(flag, "Arrival", stoi(ele.second)));
				pNum++;
			}

			flag ++ ;
		}
		else
		{
			operation.push_back(make_pair(ele.first, stoi(ele.second)));
		}

    	//cout << it->first << " " << it->second << endl;
	}
	process[pNum] = operation;

}

//void initial_resource_table(map<string, vector<pair<string, int> > > &resource_state_table, const int cores)
void initial_resource_table(const int cores)
{
	vector<pair<string, int> > resource_table;
	for (int i = 0; i < cores; ++i)
	{
		resource_table.push_back(make_pair("available", 0));
		resource_state_table[to_string(i)] = resource_table;
		resource_table.clear();
	}
	
	resource_table.push_back(make_pair("available", 0));
	resource_state_table["SSD"] = resource_table;

	resource_table.clear();

	resource_table.push_back(make_pair("available", 0));
	resource_state_table["INPUT"] = resource_table;
}

void check_device_state(string next_event, vector<pair<string, string> > &temp)
{
	string state;
	string device;
	int f = 0;
	int count = 0;
	temp.clear();

	if (next_event != "CORE")
	{
		state = resource_state_table[next_event].front().first;
		device = next_event;
	}
	else
	{
		for (auto & it : resource_state_table)
		{
			count ++;
			//cout << it.first << " "; 
			//cout << it.second.front().first << " " << it.second.front().second << endl;
			if (it.second.front().first == "available")
			{
	 			device = it.first;
	 			state = "available";
	 			break;
	 		}
	 		else
	 		{
	 			f++;
	 		}
	 		if (count == resource_state_table.size() - 2)
	 			break;
		 		
		}
		if (count == f)
		{
			device = "CORE";
			state = "block";
		}
	}

	temp.push_back(make_pair(device, state));
}

void change_device_state(string device, string p_id)
{
	if (device != "CORE")
	{
		if (resource_state_table[device].front().first == "available")
		{
			resource_state_table[device].front().first = "block";
		}
		else
		{
			resource_state_table[device].front().first = "available";
		}
		
	}
	else
	{
		if (resource_state_table[p_id].front().first == "available")
		{
			resource_state_table[p_id].front().first = "block";
		}
		else
		{
			resource_state_table[p_id].front().first = "available";
		}

	}
}

void queue_is_empty(string next_event, vector<pair<string, string> > &next_instruction)
{
	int p_id;
	string instruction;

	if (next_event == "CORE")
	{
		if (ready_queue.empty())
		{
			p_id = -1;
		}
		else
		{
			p_id = ready_queue.front();
			instruction = "CORE";
			ready_queue.pop();
		}
	}
	else if (next_event == "SSD")
	{
		if (ssd_queue.empty())
		{
			p_id = -1;
		}
		else
		{
			p_id = ssd_queue.front();
			instruction = "SSD";
			ssd_queue.pop();
		}
	}
	else
	{
		if (input_queue.empty())
		{
			p_id = -1;
		}
		else
		{
			p_id = input_queue.front();
			instruction = "INPUT";
			input_queue.pop();
		}
	}

	next_instruction.push_back(make_pair(instruction, p_id));
}

void push_in_queue(string next_event, int p_id)
{
	if (next_event == "CORE")
	{
		ready_queue.push(p_id);
	}
	else if (next_event == "SSD")
	{
		ssd_queue.push(p_id);
	}
	else
	{
		input_queue.push(p_id);
	}
}

void completion_of_computing_event(int current_time, string next_event, vector<pair<string, int> > &next_queue_instruction)
{
	queue_is_empty(next_event, next_queue_instruction);
			
	next_process = next_queue_instruction.front().second;
	// next_process = -1 means empty
	// else next_process = queue.front which is the process id

	change_device_state(next_event, to_string(next_process));
	cout << "-- " << next_event << " completion event for process "
									<< next_process 
									<< " at time "
									<< current_time << " ms" << endl;
	if (next_process != -1)
	{
		// if the queue is not empty
		// get front process from the queue, add it in event
		// pop next process in the device, do it in the queue_is_empty function
		// save the front in next_instucetion
		// print process release what device
		// CORE completion event for process 1 at time 60 ms
		instruction = next_queue_instruction.front().first;
		execution_time = process_state_table[next_process].front().second;
		// then change the correspond device's state
		change_device_state(instruction, to_string(next_process));
		

		
	}
	else
	{
		// if the queue is empty
		// go directly to the resource

		// then change the correspond device's state
		next_process = event_list.front().p_id;
		change_device_state(instruction, to_string(next_process));
		// add new event
		// release_time = current_time + execution_time;
		// event_list.push_back(EventList(next_process, instruction, release_time));
		
		// put next instruction in the queue
		//push_in_queue(next_event, next_process);
	}

	// add new event
	release_time = current_time + execution_time;
	event_list.push_back(EventList(next_process, instruction, release_time));
}

int main(int argc, char const *argv[])
{
	
	// read instructions from file to vector

	vector <pair<string, string> > read_from_file;

	read_fuction(read_from_file);

	// for (auto & element : read_from_file)
	// {
	// 	cout << element.first << " " << element.second << endl;
	// }

	// read each process task from read_from_file, put in map with vector
	map< int, vector<pair<string, int> > > process;
	split_process_and_save(process, read_from_file);

	// for (auto & element : process)
	// {
	// 	cout << element.first << "th iteration" << endl;
	// 	for(auto& ele: element.second)
	// 		cout << ele.first << " " << ele.second << " " << endl;
	// }
	int cores = process[-1].front().second;
	//cout << "cores = " << cores << endl;

	// initial resource table
	
	initial_resource_table(cores);
	
	for(auto & res : resource_state_table)
	{
		cout << res.first << " "; 
		for(auto& r : res.second)
	 		cout << r.first << " " << r.second << " " << endl;
	}

	
	// sort(event_list.begin(), event_list.end(),[](const EventList& a,
 //    const EventList& b){return a.starttime<b.starttime;});

    

	int current_time = event_list.front().endtime;
	

	int next_event_time;
	int next_process;
	string instruction;
	int execution_time;
	string next_event;
	string device;
	string device_state;
	int is_empty;
	
	vector<pair<string, int> > next_process_instruction;
	vector<pair<string, int> > next_queue_instruction;
	vector <pair<string, string> > temp;
	map<int, pair<string, int> > process_state_table;

	int ssd_count = 0;
	int ssd_time = 0;
	int release_time;



	while(event_list.size())
	{

		cout << "sort event list" << endl;
		sort(event_list.begin(), event_list.end(), is_early());
		// print the event list
	    for(int i=0;i<event_list.size();++i)
			cout << event_list[i].p_id << " " 
				 << event_list[i].event << " " 
				 << event_list[i].endtime
				 << endl;

		cout << "current time  = " << current_time << endl;

		next_event_time = event_list.front().endtime;
		next_process = event_list.front().p_id;
		next_event = event_list.front().event;
		

		cout << "next event: ";
		cout << next_process << " " << next_event << " " << next_event_time << endl;

		next_process_instruction.clear();
		next_process_instruction = process.find(next_process)->second;

		process.find(next_process)->second.erase(process.find(next_process)->second.begin(), 
												process.find(next_process)->second.begin()+1);

		string instruction = next_process_instruction.front().first;
		int execution_time = next_process_instruction.front().second;
		cout << instruction << " " << execution_time << endl;

		
		if (next_event != "Arrival")
		{
			// if it is not new process arrival
			// it should be core, ssd or input release operation
			// The correspond resource should be released, so don't need to check resource table
			// if it's core release, change that core state
			// if it's ssd or input release, change the device state
			// then read correspond queue, if the queue is not empty, 
			// pop next waiting process, process it immediately
			// if the queue is empty or after pop, read the next instruction 

			
			// check queue state, if it's not empty, pop top process to the device
			// read next instruction and add it in event list 
			
			completion_of_computing_event(current_time, next_event, next_queue_instruction);
		
		}
		else
		{
			// new process arrival
			// read next instruction
			// check resource state
			// Process 0 starts at time 10 ms
			cout << "Process " << next_process
							   << " starts at time "
							   << next_event_time
							   << " ms" << endl;
			check_device_state(instruction, temp);
			device = temp.front().first;
			device_state = temp.front().second;
			release_time = current_time + execution_time;
			is_empty = queue_is_empty(next_event);

			if (device_state == "available" && is_empty)
			{
				// if device is available
				// check if queue is empty

				// go directly to the resource
				// change the resource state
				// -- Process 0 requests a core at time 10 ms for 100 ms
				cout << "-- Process " << next_process << " requests a "
									  << instruction << " at time "
									  << current_time << " for " 
									  << execution_time << " ms" << endl;
				
			
				change_device_state(instruction, to_string(next_process));
				// add new event
				event_list.push_back(EventList(next_process, instruction, release_time));
				// update process state
				process_state_table[next_process] = "RUNNING";
				// -- Process 0 will release a core at time 110 ms
				cout << "-- Process " << next_process << " will release a " 
			 		 		 << next_instruction.front().first << " at time " 
			 		 		 << release_time << " ms" << endl;
			}
			else
			{
				// if device is available but queue is not empty
				if (!is_empty)
				{
					// pop top process to the device, already did in is_empty function
					cout << "-- Process " << next_process << " requests a "
									  << instruction << " at time "
									  << current_time << " for " 
									  << execution_time << " ms" << endl;
				
					// change the device state
					change_device_state(instruction, to_string(next_process))
					// add new event
					event_list.push_back(EventList(next_process, instruction, release_time));
				}
				else
				{

				}
				// if device is not available
				// put this event in the correspond queue
				push_in_queue(next_event, next_process);
				// -- Process 2 requests a core at time 50 ms for 40 ms
				// -- Process 2 must wait for a core
				// -- Ready queue now contains 1 process(es) waiting for a core

				cout << "-- Process " << next_process << " requests a "
									  << next_event << " at time "
									  << next_event_time << " ms for "
									  << execution_time << " ms" << endl;
				cout << "-- Process " << next_process << " must wait for a "
													  << next_event << endl;

				// update process state
				if (next_event == "CORE")
				{
					cout << "-- Ready queue now contains " << ready_queue.size()
														   << " process(es)"
														   << " waiting for a "
														   << next_event << endl;
					temp.clear();
					temp.push_back(make_pair("READY", execution_time));
					process_state_table[next_process] = temp;
				}
				else
				{
					temp.clear();
					temp.push_back(make_pair("BLOCKED", execution_time));
					process_state_table[next_process] = temp;
				}

				//cout << "Process " << next_process << " is ready" << endl;
			}
			
		}

		break;
	}




	return 0;
}

