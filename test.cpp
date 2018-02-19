// c++ 11 standard


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
    // or completion time when running process will release the core
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
			if(flag % 2 == 0)
			{
				//cout << "I'm here" << endl;
				if(flag != 0)
				{
					process[pNum] = operation;
					operation.clear();
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
	operation.clear();

}

//void initial_resource_table(map<string, vector<pair<string, int> > > &resource_state_table, const int cores)
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
	else //if (core_id == "0" || core_id == "1")
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
	// else
	// {
	// 	if (resource_state_table["0"].front().first == "available" && resource_state_table["1"].front().first == "available")
	// 	{
	// 		resource_state_table["0"].front().first = "block";
	// 		resource_state_table["0"].front().second = p_id;
	// 	}
	// 	else if (resource_state_table["1"].front().first == "available")
	// 	{
	// 		resource_state_table["1"].front().first = "block";
	// 		resource_state_table["1"].front().second = p_id;
	// 	}
	// }
}

void queue_is_empty(string event_name, vector<pair<string, string> > &next_instruction)
{
	int p_id;
	string instruction;

	if (event_name == "CORE")
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
	else if (event_name == "SSD")
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

	//next_instruction.push_back(make_pair(instruction, p_id));
}

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

// void completion_of_computing_event(int current_time, string event_name, vector<pair<string, int> > &next_queue_instruction)
// {
// 	queue_is_empty(event_name, next_queue_instruction);
			
// 	event_process_id = next_queue_instruction.front().second;
// 	// event_process_id = -1 means empty
// 	// else event_process_id = queue.front which is the process id

// 	change_device_state(event_name, to_string(event_process_id));
// 	cout << "-- " << event_name << " completion event for process "
// 									<< event_process_id 
// 									<< " at time "
// 									<< current_time << " ms" << endl;
// 	if (event_process_id != -1)
// 	{
// 		// if the queue is not empty
// 		// get front process from the queue, add it in event
// 		// pop next process in the device, do it in the queue_is_empty function
// 		// save the front in next_instucetion
// 		// print process release what device
// 		// CORE completion event for process 1 at time 60 ms
// 		instruction = next_queue_instruction.front().first;
// 		execution_time = process_state_table[event_process_id].front().second;
// 		// then change the correspond device's state
// 		change_device_state(instruction, to_string(event_process_id));
		

		
// 	}
// 	else
// 	{
// 		// if the queue is empty
// 		// go directly to the resource

// 		// then change the correspond device's state
// 		event_process_id = event_list.front().p_id;
// 		change_device_state(instruction, to_string(event_process_id));
// 		// add new event
// 		// release_time = current_time + execution_time;
// 		// event_list.push_back(EventList(event_process_id, instruction, release_time));
		
// 		// put next instruction in the queue
// 		//push_in_queue(event_name, event_process_id);
// 	}

// 	// add new event
// 	release_time = current_time + execution_time;
// 	event_list.push_back(EventList(event_process_id, instruction, release_time));
// }

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
	
	for(auto & res : resource_state_table)
	{
		cout << res.first << " "; 
		for(auto& r : res.second)
	 		cout << r.first << " " << r.second << " " << endl;
	}

	
	// sort(event_list.begin(), event_list.end(),[](const EventList& a,
 //    const EventList& b){return a.starttime<b.starttime;});

    

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
	
	vector<pair<string, int> > event_process_id_instruction;
	vector<pair<string, int> > next_queue_instruction;
	vector <pair<string, string> > temp;
	
	map<int, vector <pair<string, int> > > process_state_table;
	vector <pair<string, int> > process_state_vector;

	int ssd_count = 0;
	int ssd_time = 0;
	
	int release_time;

	int count = 0;
	cout << "*************************" << endl;
	cout << "*******    start ********" << endl;
	cout << "*************************" << endl;


	while(event_list.size())
	{	
		count ++;
		//cout << "Iteration : " << count << endl;
		// if (count == 50)
		// {
		// 	break;
		// }

		//cout << "sort event list" << endl;
		sort(event_list.begin(), event_list.end(), is_early());
		// print the event list
	  //   for(int i=0;i<event_list.size();++i)
			// cout << event_list[i].p_id << " " 
			// 	 << event_list[i].event << " " 
			// 	 << event_list[i].eventime
			// 	 << endl;

		current_time = event_list.front().eventime;


		//cout << "current time  = " << current_time << endl;


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
		// if not more operations, process terminate
		if (process.find(event_process_id)->second.empty())
		{
			// if (current_time == 15615)
			// {
			// 	cout << "15615 process 2 empty" << endl;
			// }
				
			// process terminate
			// print terminate time
			execution_time = process_state_table[event_process_id].front().second;
			cout << endl;
			cout << "Process " << event_process_id << " terminates at time "
							   << execution_time << " ms" << endl;
			cout << "Process " << event_process_id << " is TERMINATED" << endl;
			// reset 

			// delete process and print other process state
			process_state_table.erase(process_state_table.find(event_process_id));

			cout << endl;

			for(auto & table : process_state_table)
			{
				cout << "Process " << table.first << " is "
								   << table.second.front().first << endl; 
			}
			cout << endl;
			// delete current event
			//event_list.erase(event_list.begin(), event_list.begin()+1);
			//continue;
		}
		else
		{
			// if (current_time == 15615)
			// {
			// 	cout << "15615 process 2 not empty" << endl;
			// 	for (auto & element : process)
			// 	{
			// 		//cout << element.first << "th iteration" << endl;
			// 		if (element.first == 2)
			// 		{
			// 			//cout << "size of 2 : " << element.second.size() << endl;
			// 			for(auto& ele: element.second)
			// 				cout << ele.first << " " << ele.second << " " << endl;
			// 		}

			// 	}
			// }
			// if not empty
			event_process_id_instruction.clear();
			event_process_id_instruction = process.find(event_process_id)->second;

			//cout << "size of 2 : " << process[2].size() << endl;
			process.find(event_process_id)->second.erase(process.find(event_process_id)->second.begin(), 
													process.find(event_process_id)->second.begin()+1);
			//cout << "size of 2 : " << process.find(2)->second.size() << endl;
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
					//cout << res.first << " "; 
					for(auto& r : res.second)
				 		//cout << r.first << " " << r.second << " " << endl;
				 		if (event_process_id == r.second)
				 		{
				 			core_id = res.first;
				 			//cout << "1 core id = " << core_id << endl;
				 		}
				}
				
			}
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
				// else
				// {
				// 	cout << "ready queue is empty" << endl;
				// }

				//cout << "hwewe" << endl;
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
						// cout << "-- Process " << event_process_id << " requests a "
						// 			  << instruction << " at time "
						// 			  << current_time << " for " 
						// 			  << execution_time << " ms" << endl;

						// cout << "-- Process " << event_process_id << " must wait for user" <<  endl;
						// // -- Ready queue now contains 1 process(es) waiting for a core
						// cout << "-- Input queue now contains " << input_queue.size() 
						// 									   << " process(es) waiting for the user" << endl;
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
						// if (current_time == 15110)
						// {
						// 	cout << "device = " << device << " device_state = " << device_state << endl;
						// 	for(auto & res : resource_state_table)
						// 	{
						// 		cout << res.first << " "; 
						// 		for(auto& r : res.second)
						// 	 		cout << r.first << " " << r.second << " " << endl;
						// 	}

						// }


						// cout << "-- Process " << event_process_id << " requests a "
						// 			  << instruction << " at time "
						// 			  << current_time << " for " 
						// 			  << execution_time << " ms" << endl;

						// cout << "-- Process " << event_process_id << " must wait for a " 
						// 			  << instruction << endl;
						// // -- Ready queue now contains 1 process(es) waiting for a core
						// cout << "-- Ready queue now contains " << ready_queue.size() 
						// 									   << " process(es) waiting for a "
						// 									   << instruction << endl;
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
						// cout << "-- Process " << event_process_id << " requests a "
						// 			  << instruction << " at time "
						// 			  << current_time << " for " 
						// 			  << execution_time << " ms" << endl;

						// cout << "-- Process " << event_process_id << " must wait for ssd" <<  endl;
						// // -- Ready queue now contains 1 process(es) waiting for a core
						// cout << "-- SSD queue now contains " << ssd_queue.size() 
						// 									   << " process(es) waiting for the ssd" << endl;
						process_state_vector.clear();
						process_state_vector.push_back(make_pair("BLOCKED", execution_time));
						process_state_table[event_process_id] = process_state_vector;
					}
				}

			}


			// if (!ready_queue.empty())
			// {
			// 	// process completion already push in the queue
			// 	// pop front process to core
			// 	pro_id = ready_queue.front();
			// 	exe_time = process_state_table[pro_id].second();
			// 	release_time = current_time + exe_time;
			// 	ready_queue.pop();
			// 	cout << "-- Process " << event_process_id << " will release a " 
		 	// 		 		 << event_process_id_instruction.front().first << " at time " 
		 	// 		 		 << release_time << " ms" << endl;
			// 	// change core state
			// 	change_device_state("CORE", to_string(pro_id));
			// 	// update process state
			// 	process_state_vector.clear();
			// 	process_state_vector.push_back(make_pair("RUNNING", release_time));
			// 	process_state_table[event_process_id] = process_state_vector;

			// 	// update event list
			// 	event_list.push_back(EventList(event_process_id, instruction, release_time));

			// }








			// cout << "Process " << event_process_id
			// 				   << " starts at time "
			// 				   << current_event_time
			// 				   << " ms" << endl;
			// check_device_state(instruction, temp);
			// device = temp.front().first;
			// device_state = temp.front().second;

			// cout << "-- Process " << event_process_id << " requests a "
			// 						  << instruction << " at time "
			// 						  << current_time << " for " 
			// 						  << execution_time << " ms" << endl;

			// The correspond resource should be released, so don't need to check resource table
			// if it's core release, change that core state
			// if it's ssd or input release, change the device state
			// then read correspond queue, if the queue is not empty, 
			// pop next waiting process, process it immediately
			// if the queue is empty or after pop, read the next instruction 

			
			// check queue state, if it's not empty, pop top process to the device
			// read next instruction and add it in event list 
			
			//completion_of_computing_event(current_time, event_name, next_queue_instruction);
		
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
				// -- Process 2 must wait for a core
				// cout << "-- Process " << event_process_id << " must wait for a " 
				// 					  << instruction << endl;
				// // -- Ready queue now contains 1 process(es) waiting for a core
				// cout << "-- Ready queue now contains " << ready_queue.size() 
				// 									   << " process(es) waiting for a "
				// 									   << instruction << endl;
				process_state_vector.clear();
				process_state_vector.push_back(make_pair("READY", execution_time));
				process_state_table[event_process_id] = process_state_vector;
			}

			cout << endl;
			for(auto & table : process_state_table)
			{
				if (event_process_id == table.first)
				{
					continue;
				}
				cout << "Process " << table.first << " is " 
								   << table.second.front().first
								   << " until "
								   << table.second.front().second << endl; 
			}


		}




			// else if core is blocked, update process state ready
			//
			// check ready queue, 
			// if queue is empty, then check core state, 
			// if core is avail, push process directly to core
			// if core is not avail, push process in the queue
			// else if queue is not empty, push process in the queue
			// then check core state, if core is avail, pop first process in the queue to core.
			// Process 0 starts at time 10 ms
			// if (ready_queue.is_empty())
			// {
			// 	check_device_state(instruction, temp);
			// 	device = temp.front().first;
			// 	device_state = temp.front().second;
			// 	if (device_state == "available")
			// 	{
			// 		cout << "Process " << event_process_id << " starts at time " 
			//  							<< current_event_time << endl;
					
			// 		// change the core to blocked
			// 		change_device_state(instruction, to_string(event_process_id));
			// 		// add new process running event to event list

			// 		release_time = current_time + execution_time;
			// 		event_list.push_back(EventList(event_process_id, instruction, release_time));
			// 		// update process state
			// 		process_state_table[event_process_id] = "RUNNING";
			// 		// -- Process 0 will release a core at time 110 ms
			// 		cout << "-- Process " << event_process_id << " will release a " 
			// 	 		 		 << event_process_id_instruction.front().first << " at time " 
			// 	 		 		 << release_time << " ms" << endl;

			// 	}
			// 	else
			// 	{
			// 		cout << "-- Process " << event_process_id << " requests a "
			// 						  << instruction << " at time "
			// 						  << current_time << " for " 
			// 						  << execution_time << " ms" << endl;
			// 		// push new process in the ready queue
			// 		push_in_queue(instruction, event_process_id)
			// 		// save process state ready
			// 		// process_state_table[event_process_id] = "READY";

			// 		cout << "-- Ready queue now contains " << ready_queue.size()
			// 											   << " process(es)"
			// 											   << " waiting for a "
			// 											   << event_name << endl;
			// 		temp.clear();
			// 		temp.push_back(make_pair("READY", execution_time));
			// 		process_state_table[event_process_id] = temp;
			// 	}

			// }
			// cout << "Process " << event_process_id
			// 				   << " starts at time "
			// 				   << current_event_time
			// 				   << " ms" << endl;
			// check_device_state(instruction, temp);
			// device = temp.front().first;
			// device_state = temp.front().second;
			// release_time = current_time + execution_time;
			// queue_is_empty(event_name, event_process_id_instruction);

			// if (device_state == "available" && is_empty)
			// {
			// 	// if device is available
			// 	// check if queue is empty

			// 	// go directly to the resource
			// 	// change the resource state
			// 	// -- Process 0 requests a core at time 10 ms for 100 ms
			// 	cout << "-- Process " << event_process_id << " requests a "
			// 						  << instruction << " at time "
			// 						  << current_time << " for " 
			// 						  << execution_time << " ms" << endl;
				
			
			// 	change_device_state(instruction, to_string(event_process_id));
			// 	// add new event
			// 	event_list.push_back(EventList(event_process_id, instruction, release_time));
			// 	// update process state
			// 	process_state_table[event_process_id] = "RUNNING";
			// 	// -- Process 0 will release a core at time 110 ms
			// 	cout << "-- Process " << event_process_id << " will release a " 
			//  		 		 << event_process_id_instruction.front().first << " at time " 
			//  		 		 << release_time << " ms" << endl;
			// }
			// else
			// {
			// 	// if device is available but queue is not empty
			// 	if (!is_empty)
			// 	{
			// 		// pop top process to the device, already did in is_empty function
			// 		cout << "-- Process " << event_process_id << " requests a "
			// 						  << instruction << " at time "
			// 						  << current_time << " for " 
			// 						  << execution_time << " ms" << endl;
				
			// 		// change the device state
			// 		change_device_state(instruction, to_string(event_process_id))
			// 		// add new event
			// 		event_list.push_back(EventList(event_process_id, instruction, release_time));
			// 	}
			// 	else
			// 	{

			// 	}
			// 	// if device is not available
			// 	// put this event in the correspond queue
			// 	push_in_queue(event_name, event_process_id);
			// 	// -- Process 2 requests a core at time 50 ms for 40 ms
			// 	// -- Process 2 must wait for a core
			// 	// -- Ready queue now contains 1 process(es) waiting for a core

			// 	cout << "-- Process " << event_process_id << " requests a "
			// 						  << event_name << " at time "
			// 						  << current_event_time << " ms for "
			// 						  << execution_time << " ms" << endl;
			// 	cout << "-- Process " << event_process_id << " must wait for a "
			// 										  << event_name << endl;

			// 	// update process state
			// 	if (event_name == "CORE")
			// 	{
			// 		cout << "-- Ready queue now contains " << ready_queue.size()
			// 											   << " process(es)"
			// 											   << " waiting for a "
			// 											   << event_name << endl;
			// 		temp.clear();
			// 		temp.push_back(make_pair("READY", execution_time));
			// 		process_state_table[event_process_id] = temp;
			// 	}
			// 	else
			// 	{
			// 		temp.clear();
			// 		temp.push_back(make_pair("BLOCKED", execution_time));
			// 		process_state_table[event_process_id] = temp;
			// 	}

			// 	//cout << "Process " << event_process_id << " is ready" << endl;
			// }
			
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

	// SUMMARY:
	// Number of processes that completed: 5
	// Total number of SSD accesses: 40
	// Average SSD access time: 1.00 ms
	// Total elapsed time: 21615 ms
	// Core utilization: 19.50 percent
	// SSD utilization: 0.19 percent
	cout << setprecision(2) << fixed;

	cout << "SUMMARY:" << endl;
	cout << "Number of processes that completed: " << NUM_PROCESS << endl;
	cout << "Total number of SSD accesses: " << SSDcount << endl;
	cout << "Average SSD access time: " << (float)SSDtimes/(float)SSDcount << " ms" << endl;
	cout << "Total elapsed time: " << current_time << " ms" << endl;
	cout << "Core utilization: " << 100 * (float)CORE_REQ/(float)current_time << " percent" << endl;
	cout << "SSD utilization: " << 100 * (float)SSDtimes/(float)current_time << " percent" << endl;

	//cout << "Iteration : " << count << endl;

	return 0;
}

