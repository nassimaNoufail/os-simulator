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

struct EventList
{
    int name;
    //int starttime;
    int starttime;


    EventList(const int& name, const int& starttime): name(name),starttime(starttime)
    {}
};



int main(int argc, char const *argv[])
{
	// use a map with paired vector to save all the operations for each process
	map< int, vector<pair<string, int> > > process;
	vector<pair<string, int> > operation;
	queue<int> ready_queue;
	queue<int> ssd_queue;
	queue<int> input_queue;

	// create event list to track event time
	// create resource table to maintain the state of cores, input and ssd
	// create process state toable to maintain each process current state
	vector<EventList> event_list;
	//vector<pair<string, string> > resource_state_table;
	map<string, vector<pair<string, int> > > resource_state_table;
	//vector<pair<int, string> > process_state_table;
	map<int, string> process_state_table;
    
	
	//operation.push_back(make_pair("c1", "100"));
	
	//process[1] = operation;

	//cout << process[1][0].first << "  "<< process[1][0].second << endl;
	// pNum to record the number of process
	int pNum = 0;
	// string new_process_time;
	// cout << "Insert one array" ;


	string str;
	string value;
	string cores;
	int flag = 0;


	// use a vector to first save all the data from redirect input
	vector<pair<string, string> > read_in_file;

	// every iteration save an row in an vector


	while (cin >> str)
	{
		cin >> value;
		read_in_file.push_back(make_pair(str, value));

	}

	
	// for (auto & element : read_in_file)
	// {
	// 	cout << element.first << " " << element.second << endl;
	// }
	
	cout << endl << "process start" << endl;
	
	// read from vector
	for (auto & ele : read_in_file)
	{	
		// save the amount of cores
		if(ele.first == "NCORES")
		{
			cores = ele.second;
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

				event_list.push_back(EventList(flag, stoi(ele.second)));
			}
			else
			{
				//cout << "I'm here again" << endl;
				process[pNum] = operation;

				cout << pNum << "th " << "iteration" << endl;

				// for (auto & element : process)
				// {
				// 	cout << element.first << " ";
				// 	for(auto& ele: element.second)
				// 		cout << ele.first << " " << ele.second << " " << endl;
				// }

				operation.clear();
				event_list.push_back(EventList(flag, stoi(ele.second)));
				pNum++;
			}

			flag ++ ;
			//cout << "flag = " << flag << endl;
		}
		else
		{
			operation.push_back(make_pair(ele.first, stoi(ele.second)));
		}

    	//cout << it->first << " " << it->second << endl;
	}
	process[pNum] = operation;
	cout << pNum << "th " << "iteration" << endl;

	cout << "Done" << endl;

	// for (auto & element : process)
	// {
	// 	cout << element.first << "th iteration" << endl;
	// 	for(auto& ele: element.second)
	// 		cout << ele.first << " " << ele.second << " " << endl;
	// }

	
	// initial resource table
	// vector<pair<string, string> > resource_state_table;

	cout << "initial resource table " << endl;
	vector<pair<string, int> > resource_table;
	for (int i = 0; i < stoi(cores); ++i)
	{
		//resource_state_table.push_back(make_pair(to_string(i), "available"));
		resource_table.push_back(make_pair("available", 0));
		resource_state_table[to_string(i)] = resource_table;
		resource_table.clear();
	}

	resource_table.push_back(make_pair("available", 0));
	resource_state_table["SSD"] = resource_table;
	resource_table.clear();

	resource_table.push_back(make_pair("available", 0));
	resource_state_table["INPUT"] = resource_table;
	resource_table.clear();


	// for (auto & element : process)
	// {
	// 	cout << element.first << "th iteration" << endl;
	// 	for(auto& ele: element.second)
	// 		cout << ele.first << " " << ele.second << " " << endl;
	// }

	for(auto & res : resource_state_table)
	{
		cout << res.first << " "; 
		cout << res.second.front().first << " " << res.second.front().second << endl;
		// for(auto& r : res.second)
	 // 		cout << r.first << " " << r.second << " " << endl;
	}
	

	cout << "size of event list " << event_list.size() << endl;
	int event_clock;

	while(event_list.size())
	{
		// sort event list every time 
		cout << "sort event list" << endl;
		sort(event_list.begin(), event_list.end(),[](const EventList& a,
	    const EventList& b){return a.starttime<b.starttime;});

	    // print the event list
	    for(int i=0;i<event_list.size();++i)
			cout << event_list[i].name << " " << event_list[i].starttime << endl;

		//cout << event_list.front().name << " " << event_list.front().endtime << endl;

		int process_num  = event_list.front().name;
		event_clock = event_list.front().starttime;

		cout << "Process " << process_num << " starts at time " 
			 << event_list.front().starttime << endl;

		// from process number enter the process list to get next instruction
		// create a vector to save the instructions
		vector<pair<string, int> > process_ins;

		process_ins = process.find(process_num)->second;

		//cout << "before earse front instruction" << endl;
		//cout << process.find(process_num)->second.front().first << endl;

		process.find(process_num)->second.erase(process.find(process_num)->second.begin(), 
												process.find(process_num)->second.begin()+1);

		//cout << "after earse front instruction" << endl;
		//cout << process.find(process_num)->second.front().first << endl;

		string instruction = process_ins.front().first;
		int execution_time = process_ins.front().second;

		cout << instruction << " " << execution_time << endl;
		// Process 0 requests a core at time 10 ms for 100 ms
		cout << "Process " << process_num << " requests a " 
			 << instruction << " at time " 
			 << event_list.front().starttime << " ms for "
			 << execution_time << " ms" << endl;

		int release_time = event_clock + execution_time;

		// after get the instruction, put the operation in correspond queue
		// if looking for ssd or input
			// check the state of ssd or input
				// change state
			// else if looking for state of each cores
			// if any core is avaiable, change the state of that core
			// else 
			// put the process in the ready queue

		//cout << "instruction: " << instruction << endl;
		if(instruction == "SSD")
		{
			cout << "SSD" << endl;
			ssd_queue.push(process_num);
			if (resource_state_table.find(instruction)->second.front().first == "available")
			{
				ssd_queue.pop();
				resource_state_table.find(instruction)->second.front().first = "block";
				event_list.push_back(EventList(process_num, release_time));
				//process_state_table.push_back(make_pair(process_num, "RUNNING"));
				process_state_table[process_num] =  "BLOCKED";
				cout << "Process " << process_num << " will release a " 
			 		 << process_ins.front().first << " at time " 
			 		 << release_time << " ms" << endl;
			}
			else
			{
				//process_state_table.push_back(make_pair(process_num, "READY"));
				process_state_table[process_num] = "BLOCKED";
				cout << "Process " << process_num << " is ready" << endl;

			}
		}
		else if(process_ins.front().first == "INPUT")
		{
			cout << "INPUT" << endl;
			input_queue.push(process_num);
			if (resource_state_table.find(instruction)->second.front().first == "available")
			{
				input_queue.pop();
				resource_state_table.find(instruction)->second.front().first = "block";
				event_list.push_back(EventList(process_num, release_time));
				//process_state_table.push_back(make_pair(process_num, "RUNNING"));
				process_state_table[process_num] =  "BLOCKED";
				cout << "Process " << process_num << " will release a " 
			 		 << process_ins.front().first << " at time " 
			 		 << release_time << " ms" << endl;
			}
			else
			{
				//process_state_table.push_back(make_pair(process_num, "READY"));
				process_state_table[process_num] = "BLOCKED";
				cout << "Process " << process_num << " is ready" << endl;
			}
		}
		else
		{
			cout << "CORE" << endl;
			ready_queue.push(process_num);
			int f = 0;
			for(auto & res : resource_state_table)
			{
				//cout << "resource: " << res.first << endl;
				if (res.first == "INPUT") 
				{
					//cout << " not INPUT" << endl;
					continue;
				}
				else if (res.first == "SSD")
				{
					//cout << " not SSD" << endl;
					continue;
				}
				else
				{	

					if(res.second.front().first == "available")
					{
						ready_queue.pop();
						res.second.front().first = "block";
						event_list.push_back(EventList(process_num, release_time));
						//process_state_table.push_back(make_pair(process_num, "RUNNING"));
						process_state_table[process_num] = "RUNNING";
						cout << "Process " << process_num << " will release a " 
			 		 		 << process_ins.front().first << " at time " 
			 		 		 << release_time << " ms" << endl;
			 		 	
						break;
					}
					else
					{
						f ++ ;
					}
					if (f == stoi(cores))
					{
						//process_state_table.push_back(make_pair(process_num, "READY"));
						process_state_table[process_num] = "READY";
						cout << "Process " << process_num << " is ready" << endl;
					}
				}
			}
		}

		cout << "resouce table : " << endl;
		// for(auto & res : resource_state_table)
		// {
		// 	cout << res.first << " " << res.second << endl; 
		// }
		for(auto & res : resource_state_table)
		{
			cout << res.first << " "; 
			for(auto& r : res.second)
		 		cout << r.first << " " << r.second << " " << endl;
		}



		// update the event list
		
		
		// sort the event list every time a new event come
		


		// update the porcess table
		// initial process state table
		//vector<pair<int, string> > process_state_table;
		
		// based on the situation puch the state of that process to process table
		//process_state_table.push_back(make_pair(process_num, ));


		//process_state_table.push_back(make_pair())
		// Process 0 starts at time 10 ms
		cout << "current event name = " << event_list.front().name << endl 
			 << "current event start time = " << event_list.front().starttime << endl
			 << "current event clock = " << event_clock << endl;
		int current_name = event_list.front().name;

		int current_time = event_list.front().starttime;

		cout << process_ins.front().first << process_ins.front().second << endl;
		event_list.erase(event_list.begin(), event_list.begin()+1);


		//cout << event_list.front().name << " " << event_list.front().endtime << endl;
		
		//cout << "event list size: " << event_list.size() << endl;


		sort(event_list.begin(), event_list.end(),[](const EventList& a,
    					const EventList& b){return a.starttime<b.starttime;});

		// after erase an event means a resource is released, check the queue based on the event
		// assign new task to that resource
		if (to_string(current_name) == "SSD")
		{
			if (!ssd_queue.empty())
			{
				ssd_queue.pop();
			}
		} 
		bool current_name_exsit = false;
		for(auto & pro : process_state_table)
		{
			cout << pro.first << " " << pro.second << endl;
		}
		cout << "after erase the front event" << endl;
		for(int i=0;i<event_list.size();++i)
		{

			cout << event_list[i].name << " " << event_list[i].starttime << endl;
			if (current_name == event_list[i].name)
			{
				cout << current_name << " is exsit" << endl;
					
				current_name_exsit = true;
				
			}
		}

		if (!current_name_exsit)
			{
				if (process_state_table.count(current_name) != 0)
				{
					current_name_exsit = true;
				} 
			}

		if (!current_name_exsit)
		{
			cout << current_name << " is not exsit" << endl;
			process_ins.clear();
			process_ins = process.find(current_name)->second;
			instruction = process_ins.front().first;
			execution_time = process_ins.front().second;
			release_time = current_time + execution_time;
			if(instruction == "SSD")
			{
				cout << "SSD" << endl;
				ssd_queue.push(process_num);
				if (resource_state_table.find(instruction)->second.front().first == "available")
				{
					ssd_queue.pop();
					resource_state_table.find(instruction)->second.front().first = "block";
					event_list.push_back(EventList(process_num, release_time));
					process_state_table[process_num] = "RUNNING";
					cout << "Process " << process_num << " will release a " 
				 		 << process_ins.front().first << " at time " 
				 		 << release_time << " ms" << endl;
				}
				else
				{
					//process_state_table.push_back(make_pair(process_num, "READY"));
					process_state_table[process_num] = "READY";
					cout << "Process " << process_num << " is ready" << endl;
				}
			}
			else if(process_ins.front().first == "INPUT")
			{
				cout << "INPUT" << endl;
				input_queue.push(process_num);
				if (resource_state_table.find(instruction)->second.front().first == "available")
				{
					input_queue.pop();
					resource_state_table.find(instruction)->second.front().first = "block";
					event_list.push_back(EventList(process_num, release_time));
					//process_state_table.push_back(make_pair(process_num, "RUNNING"));
					process_state_table[process_num] = "RUNNING";
					cout << "Process " << process_num << " will release a " 
				 		 << process_ins.front().first << " at time " 
				 		 << release_time << " ms" << endl;
				}
				else
				{
					//process_state_table.push_back(make_pair(process_num, "READY"));
					process_state_table[process_num] = "READY";
					cout << "Process " << process_num << " is ready" << endl;
				}
			}
			else
			{
				cout << "CORE" << endl;
				ready_queue.push(process_num);
				int f = 0;
				for(auto & res : resource_state_table)
				{
					cout << "resource: " << res.first << endl;
					if (res.first == "INPUT") 
					{
						cout << " not INPUT" << endl;
						continue;
					}
					else if (res.first == "SSD")
					{
						cout << " not SSD" << endl;
						continue;
					}
					else
					{	

						if(res.second.front().first == "available")
						{
							ready_queue.pop();
							res.second.front().first = "block";
							event_list.push_back(EventList(process_num, release_time));
							//process_state_table.push_back(make_pair(process_num, "RUNNING"));
							process_state_table[process_num] = "RUNNING";
							cout << "Process " << process_num << " will release a " 
				 		 		 << process_ins.front().first << " at time " 
				 		 		 << release_time << " ms" << endl;
				 		 	
							break;
						}
						else
						{
							f ++ ;
						}
						if (f == stoi(cores))
						{
							//process_state_table.push_back(make_pair(process_num, "READY"));
							process_state_table[process_num] = "READY";
							cout << "Process " << process_num << " is ready" << endl;
						}
					}
				}
			}

		}
			
		
		/*
		

		// vector<pair<string, int> > process_ins;

		// process_ins = process.find(process_num)->second;

		// cout << "before earse front instruction" << endl;
		// cout << process.find(process_num)->second.front().first << endl;

		// process.find(process_num)->second.erase(process.find(process_num)->second.begin(), 
		// 										process.find(process_num)->second.begin()+1);

		// cout << "after earse front instruction" << endl;
		// cout << process.find(process_num)->second.front().first << endl;

		// string instruction = process_ins.front().first;
		
		
		*/
		//break;
	}

	cout << "print the event list again" << endl;
	for(int i=0;i<event_list.size();++i)
		cout << event_list[i].name << " " << event_list[i].starttime << endl;



	return 0;
}