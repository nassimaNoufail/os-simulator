// c++ 11 standard
/*
compile: g++ filename.cpp -o filename.out -std=c++11
run: ./filename.out < inputfile.txt > output.txt
input: inputfile.txt
output: output.txt
*/


#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <queue>
#include <iomanip>

using namespace std;
	

map<string, vector<pair<string, int> > > resource_state_table;
queue<int> ready_queue;
queue<int> ssd_queue;
queue<int> input_queue;
int NUM_PROCESS = 0;
int SSDcount = 0;
int SSDtimes = 0;
int CORE_REQ = 0;
int SSD_REQ = 0;

// event list including process id, process state or device, and time of event
struct EventList
{
    int p_id;
    string event;
    // eventime include arrival and completion time
    // arrival time when new process arrive 
    // or completion time when running process release the core
    int eventime;

    EventList(const int& p_id, const string& event, const int& eventime): p_id(p_id),event(event),eventime(eventime)
    {}
};

vector<EventList> event_list;

// sort event list by eventime
struct is_early
{
	bool operator()(const EventList& x, const EventList& y) const
	{
		return x.eventime < y.eventime;
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
			NUM_PROCESS++;
			// every even time meet a new process
			if(flag % 2 == 0)
			{
				if(flag != 0)
				{
					process[pNum] = operation;
					operation.clear();
					pNum++;
				}

				event_list.push_back(EventList(flag, "Arrival", stoi(ele.second)));
			}
			// odd time
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

	}
	process[pNum] = operation;
	operation.clear();

}

// initial resource table
void initial_resource_table(const int cores)
{
	vector<pair<string, int> > resource_table;
	for (int i = 0; i < cores; ++i)
	{
		resource_table.push_back(make_pair("available", -1));
		resource_state_table[to_string(i)] = resource_table;
		resource_table.clear();
	}
	
	resource_table.push_back(make_pair("available", -1));
	resource_state_table["SSD"] = resource_table;

	resource_table.clear();

	resource_table.push_back(make_pair("available", -1));
	resource_state_table["INPUT"] = resource_table;
}

// check device state
void check_device_state(string event_name, vector<pair<string, string> > &temp)
{
	string state;
	string device;
	int f = 0;
	int count = 0;
	temp.clear();

	if (event_name != "CORE")
	{
		state = resource_state_table[event_name].front().first;
		device = event_name;
	}
	else
	{
		for (auto & it : resource_state_table)
		{
			count ++;
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

// change device state
void change_device_state(string device, string core_id, int p_id)
{
	if (device != "CORE")
	{
		if (resource_state_table[device].front().first == "available")
		{
			resource_state_table[device].front().first = "block";
			resource_state_table[device].front().second = p_id;
		}
		else
		{
			resource_state_table[device].front().first = "available";
			resource_state_table[device].front().second = -1;
		}
		
	}
	else 
	{		
		if (resource_state_table[core_id].front().first == "available")
		{
			resource_state_table[core_id].front().first = "block";
			resource_state_table[core_id].front().second = p_id;
		}
		else
		{
			resource_state_table[core_id].front().first = "available";
			resource_state_table[core_id].front().second = -1;
		}

	}
}


// push every instruction to the correspond queue first before doing other operation
string push_in_queue(string event_name, int p_id)
{
	string queue_info;

	if (event_name == "CORE")
	{
		ready_queue.push(p_id);
		queue_info = "READY";
	}
	else if (event_name == "SSD")
	{
		ssd_queue.push(p_id);
		queue_info = "BLOCKED";
	}
	else
	{
		input_queue.push(p_id);
		queue_info = "BLOCKED";
	}
	return queue_info;
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
	// initial arrival event when split the process
	split_process_and_save(process, read_from_file);

	// for (auto & element : process)
	// {
	// 	cout << element.first << "th iteration" << endl;
	// 	cout << "size of : " << element.second.size() << endl;
	// 	// for(auto& ele: element.second)
	// 	// 	cout << ele.first << " " << ele.second << " " << endl;
	// }

	// read core amount
	int cores = process[-1].front().second;
	//cout << "cores = " << cores << endl;

	// initial resource table
	initial_resource_table(cores);
	
	// for(auto & res : resource_state_table)
	// {
	// 	cout << res.first << " "; 
	// 	for(auto& r : res.second)
	//  		cout << r.first << " " << r.second << " " << endl;
	// }

	
	int current_time;
	

	int current_event_time;
	int event_process_id;
	string instruction;
	int execution_time;
	string event_name;
	string device;
	string device_state;
	string queue_info;
	int is_empty;
	
	// read process id & instruction from event
	vector<pair<string, int> > event_process_id_instruction;

	vector <pair<string, string> > temp;
	
	map<int, vector <pair<string, int> > > process_state_table;
	vector <pair<string, int> > process_state_vector;

	int ssd_count = 0;
	int ssd_time = 0;
	
	int release_time;

	// start from read first event in the event list until the list is empty
	while(event_list.size())
	{	
		// sort event first before read the frist event
		sort(event_list.begin(), event_list.end(), is_early());

		current_time = event_list.front().eventime;

		// read current event information
		// event will be either a process arrival event, or a process completion event
		// event time, process id, event name (arrival/device release event)
		current_event_time = event_list.front().eventime;
		event_process_id = event_list.front().p_id;
		event_name = event_list.front().event;
		

		//cout << "current event: ";
		//cout << event_process_id << " " << event_name << " " << current_event_time << endl;

		// if it is a arrival event, read next instruction, push in the queue
		// if it is a device event, read next instruction, push in the queue
		// check correspond device state, if it is avail, pop first process to device
		// change device state, update event lsit and process state
		// if it is not avail, update process state
		int pro_id;
		string core_id;
		int exe_time;
		// if no more operations, process terminate
		if (process.find(event_process_id)->second.empty())
		{
				
			// process terminate
			// print terminate time
			execution_time = process_state_table[event_process_id].front().second;
			cout << endl;
			cout << "Process " << event_process_id << " terminates at time "
							   << execution_time << " ms" << endl;
			cout << "Process " << event_process_id << " is TERMINATED" << endl;

			// delete process and print other process state
			process_state_table.erase(process_state_table.find(event_process_id));


			for(auto & table : process_state_table)
			{
				cout << "Process " << table.first << " is "
								   << table.second.front().first << endl; 
			}
			cout << endl;
		}
		else
		{
			// if not empty, find correspond instruction
			event_process_id_instruction.clear();
			event_process_id_instruction = process.find(event_process_id)->second;

			process.find(event_process_id)->second.erase(process.find(event_process_id)->second.begin(), 
													process.find(event_process_id)->second.begin()+1);
			// read next instruction
			instruction = event_process_id_instruction.front().first;
			execution_time = event_process_id_instruction.front().second;
			//cout << instruction << " " << execution_time << endl;

			// push in the queue
			queue_info = push_in_queue(instruction, event_process_id);
			// update process state, excution time
			// ready in the ready queue, blocked in the device queue
			process_state_vector.clear();
			process_state_vector.push_back(make_pair(queue_info, execution_time));
			process_state_table[event_process_id] = process_state_vector;
		}


		// check event type
		if (event_name != "Arrival")
		{
			// if it is not new process arrival
			// it should be core, ssd or input release operation
			// current event time should be resouce operation completion time 
			// the correspond process state should be running if process in core
			// or blocked if process in input/ssd
			// 
			//-- CORE completion event for process 1 at time 60 ms
			// cout << "-- " << event_name << " completion event for process "
			// 							<< event_process_id << " at time "
			// 							<< current_event_time << " ms" << endl;

			if (event_name == "CORE")
			{
				for(auto & res : resource_state_table)
				{ 
					for(auto& r : res.second)
				 		if (event_process_id == r.second)
				 		{
				 			core_id = res.first;
				 		}
				}
			}

			// change the device state after device completion
			change_device_state(event_name, core_id, event_process_id);
			
			// check device state and queue
			
			// if device is avail and queue is not empty, pop front process to device
			if (event_name == "INPUT")
			{
				if (!input_queue.empty())
				{
					pro_id = input_queue.front();
					exe_time = process_state_table[pro_id].front().second;
					release_time = current_time + exe_time;
					input_queue.pop();
					// cout << "-- Process " << pro_id << " will release a " 
		 		//  		 << event_name << " at time " 
		 		//  		 << release_time << " ms" << endl;
		 		 	change_device_state(event_name, to_string(event_process_id), pro_id);
					// update process state
					process_state_vector.clear();
					process_state_vector.push_back(make_pair("BLOCKED", release_time));
					process_state_table[pro_id] = process_state_vector;

					// update event list
					event_list.push_back(EventList(pro_id, event_name, release_time));
				}
			}
			else if (event_name == "CORE")
			{				
				if (!ready_queue.empty())
				{
					pro_id = ready_queue.front();
					exe_time = process_state_table[pro_id].front().second;
					CORE_REQ += exe_time;
					release_time = current_time + exe_time;
					ready_queue.pop();

					//cout << pro_id << " " << "2 core id = " << core_id << endl;


					// cout << "-- Process " << pro_id << " will release a " 
		 		//  		 << event_name << " at time " 
		 		//  		 << release_time << " ms" << endl;
		 		 	change_device_state(event_name, core_id, pro_id);
		 		 	
					// update process state
					process_state_vector.clear();
					process_state_vector.push_back(make_pair("RUNNING", release_time));
					process_state_table[pro_id] = process_state_vector;

					// update event list
					event_list.push_back(EventList(pro_id, event_name, release_time));
				}
				
			}
			else
			{
				// event name == SSD
				SSDtimes += current_time;
				if (!ssd_queue.empty())
				{
					pro_id = ssd_queue.front();
					exe_time = process_state_table[pro_id].front().second;
					release_time = current_time + exe_time;
					ssd_queue.pop();
					// cout << "-- Process " << pro_id << " will release a " 
		 		//  		 << event_name << " at time " 
		 		//  		 << release_time << " ms" << endl;
		 		 	change_device_state(event_name, to_string(event_process_id), pro_id);
					// update process state
					process_state_vector.clear();
					process_state_vector.push_back(make_pair("BLOCKED", release_time));
					process_state_table[pro_id] = process_state_vector;

					// update event list
					event_list.push_back(EventList(pro_id, event_name, release_time));
				}
				
			}


			check_device_state(instruction, temp);
			device = temp.front().first;
			device_state = temp.front().second;



			// after process the queue, process next instruction
			if (instruction == "INPUT")
			{

				if (!input_queue.empty())
				{
					if (device_state == "available")
					{
						pro_id = input_queue.front();
						exe_time = process_state_table[pro_id].front().second;
						release_time = current_time + exe_time;
						input_queue.pop();
						// cout << "-- Process " << pro_id << " will release a " 
			 		//  		 << event_process_id_instruction.front().first << " at time " 
			 		//  		 << release_time << " ms" << endl;

			 		 	change_device_state(instruction, to_string(event_process_id), pro_id);

						// update process state
						process_state_vector.clear();
						process_state_vector.push_back(make_pair("BLOCKED", release_time));
						process_state_table[pro_id] = process_state_vector;

						// update event list
						event_list.push_back(EventList(pro_id, instruction, release_time));
					}
					else
					{
						process_state_vector.clear();
						process_state_vector.push_back(make_pair("BLOCKED", execution_time));
						process_state_table[event_process_id] = process_state_vector;
					}
				}
			}
			else if (instruction == "CORE")
			{
				for(auto & res : resource_state_table)
				{
					//cout << res.first << " "; 
					for(auto& r : res.second)
					{
				 		//cout << r.first << " " << r.second << " " << endl;
				 		if (r.first == "available")
				 		{
				 			core_id = res.first;
				 			//cout << "1 core id = " << core_id << endl;
				 		}
				 	}
				}

				if (!ready_queue.empty())
				{
					if (device_state == "available")
					{
						pro_id = ready_queue.front();
						exe_time = process_state_table[pro_id].front().second;
						CORE_REQ += exe_time;
						release_time = current_time + exe_time;
						ready_queue.pop();
						// cout << "-- Process " << pro_id << " will release a " 
			 		//  		 << event_process_id_instruction.front().first << " at time " 
			 		//  		 << release_time << " ms" << endl;

			 		 	change_device_state(instruction, core_id, pro_id);
			 		 	

						// update process state
						process_state_vector.clear();
						process_state_vector.push_back(make_pair("RUNNING", release_time));
						process_state_table[pro_id] = process_state_vector;

						// update event list
						event_list.push_back(EventList(pro_id, instruction, release_time));
					}
					else
					{
						process_state_vector.clear();
						process_state_vector.push_back(make_pair("READY", execution_time));
						process_state_table[event_process_id] = process_state_vector;
					}
				}

				
			}
			else
			{
				// instruction == SSD
				SSDcount++;
				SSDtimes -= current_time;
				if (!ssd_queue.empty())
				{
					if (device_state == "available")
					{					
						pro_id = ssd_queue.front();
						exe_time = process_state_table[pro_id].front().second;
						release_time = current_time + exe_time;
						ssd_queue.pop();
						// cout << "-- Process " << pro_id << " will release a " 
			 		//  		 << event_process_id_instruction.front().first << " at time " 
			 		//  		 << release_time << " ms" << endl;
			 		 	change_device_state(instruction, to_string(event_process_id), pro_id);
						// update process state
						process_state_vector.clear();
						process_state_vector.push_back(make_pair("BLOCKED", release_time));
						process_state_table[pro_id] = process_state_vector;

						// update event list
						event_list.push_back(EventList(pro_id, instruction, release_time));
					}
					else
					{
						process_state_vector.clear();
						process_state_vector.push_back(make_pair("BLOCKED", execution_time));
						process_state_table[event_process_id] = process_state_vector;
					}
				}

			}
		
		}
		else
		{
			// new process arrival
			// check core state, if core is avail, pop first process in the queue to core
			// update event list and process state running
			cout << endl;
			cout << "Process " << event_process_id
							   << " starts at time "
							   << current_event_time
							   << " ms" << endl;
			check_device_state(instruction, temp);
			device = temp.front().first;
			device_state = temp.front().second;

			// cout << "-- Process " << event_process_id << " requests a "
			// 						  << instruction << " at time "
			// 						  << current_time << " for " 
			// 						  << execution_time << " ms" << endl;

			// release_time = current_time + execution_time;
			// queue_is_empty(event_name, event_process_id_instruction);

			if (device_state == "available")
			{
				// if device is available
				// check if queue is empty
				if (!ready_queue.empty())
				{
					// arrival process already push in the ready queue
					// pop front process to core
					pro_id = ready_queue.front();
					exe_time = process_state_table[pro_id].front().second;
					release_time = current_time + exe_time;
					ready_queue.pop();
					// cout << "-- Process " << event_process_id << " will release a " 
			 	// 	 		 << event_process_id_instruction.front().first << " at time " 
			 	// 	 		 << release_time << " ms" << endl;
			 		CORE_REQ += exe_time;
					// change core state
					change_device_state("CORE", to_string(event_process_id), pro_id);
					// update process state
					process_state_vector.clear();
					process_state_vector.push_back(make_pair("RUNNING", release_time));
					process_state_table[event_process_id] = process_state_vector;

					// update event list
					event_list.push_back(EventList(event_process_id, instruction, release_time));

				}
				else
				{
					// impossible
				}
				
			}
			else
			{
				// if device is not avail
				// update process state to ready
				
				// cout << "-- Process " << event_process_id << " must wait for a " 
				// 					  << instruction << endl;
				
				// cout << "-- Ready queue now contains " << ready_queue.size() 
				// 									   << " process(es) waiting for a "
				// 									   << instruction << endl;
				process_state_vector.clear();
				process_state_vector.push_back(make_pair("READY", execution_time));
				process_state_table[event_process_id] = process_state_vector;
			}

			
			for(auto & table : process_state_table)
			{
				if (event_process_id == table.first)
				{
					continue;
				}
				cout << "Process " << table.first << " is " 
								   << table.second.front().first << endl;
								   
			}


		}

		event_list.erase(event_list.begin(), event_list.begin()+1);
		// for(auto & res : resource_state_table)
		// {
		// 	cout << res.first << " "; 
		// 	for(auto& r : res.second)
		//  		cout << r.first << " " << r.second << " " << endl;
		// }


		// for(auto & table : process_state_table)
		// {
		// 	cout << "Process " << table.first << " is " 
		// 					   << table.second.front().first
		// 					   << " until "
		// 					   << table.second.front().second << endl; 
		// }
		// cout << "---------------------------" << endl;
		// cout << "---------------------------" << endl;
		//break;
	}

	cout << setprecision(2) << fixed;

	cout << "SUMMARY:" << endl;
	cout << "Number of processes that completed: " << NUM_PROCESS << endl;
	cout << "Total number of SSD accesses: " << SSDcount << endl;
	cout << "Average SSD access time: " << (float)SSDtimes/(float)SSDcount << " ms" << endl;
	cout << "Total elapsed time: " << current_time << " ms" << endl;
	cout << "Core utilization: " << 100 * (float)CORE_REQ/(float)current_time << " percent" << endl;
	cout << "SSD utilization: " << 100 * (float)SSDtimes/(float)current_time << " percent" << endl;

	return 0;
}

