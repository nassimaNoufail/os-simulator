/*
Description: A server program that will wait for connection requests
from your client and average early career and mid-career
pay for the requested college major.
compile: g++ filename.cpp -o filename.out -std=c++11
run: ./filename.out
2 inputs: 
inputfile.txt
port number

output: 
stdout
*/
#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>



using namespace std;

// a map with paired vector to save each line information from file
map< string, vector<pair<string, string> > > read_from_file;

// save each line to map
// input: stream per line
void savefromfile(istringstream &iss)
{
	// push each word in the line in stack
	stack <string> readofline;
	string word;
	string temp;
	string major;
	string mid_pay;
	string early_pay;
	while(iss >> word)
	{
		readofline.push(word);
	}

	// get mid pay and early pay from the top of the stack
	mid_pay = readofline.top();
	readofline.pop();
	early_pay = readofline.top();
	readofline.pop();

	// get the major name from the rest of the stack
	while(!readofline.empty())
	{
		temp = readofline.top();
		major = temp + major;

		readofline.pop();
		if (!readofline.empty())
		{
			major = " " + major;
		}
	}

	// push all the information in the map
	vector <pair<string, string> > salary;
	salary.push_back(make_pair(early_pay, mid_pay));
	read_from_file[major] = salary;	
}

// read per line from the file
int readfromfile(char const *argc)
{
	ifstream file(argc);
	
	if(!file) 
	{
    	cout << "Cannot open input file.\n";
    	return 0;
  	}

  	string str;
  	// read line by line and process each line in savefromfile function
	while(file && getline(file, str)) 
	{
    	istringstream iss(str);	
  		savefromfile(iss);
  	}
}



int main(int argc, char const *argv[])
{

	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	int flag;
	cout << "Enter file name: ";
	cin.getline(buffer, 256);
	flag = readfromfile(buffer);
	// if user input wrong file name, exit
	if (flag == 0)
	{
		cout << "Wrong file" << endl;
		return 0;
	}
	memset(buffer, 256, sizeof(buffer));
	cout << "Enter server port number: ";
	cin.getline(buffer, 256);

	// create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
        cerr << "ERROR opening socket" << endl;
    
    memset((char *) &serv_addr, 0, sizeof(serv_addr));

	portno = atoi(buffer);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	// bind socket with corresponding address
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
	      sizeof(serv_addr)) < 0) 
	{
		cerr << "ERROR on binding" << endl;
		return 0;
	}
	
	// listen to the client
	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	// keep listen
	while(true)
	{
		// Accept a connection with the accept() system call. 
		// This call typically blocks until a client connects with the server.
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen);
		if (newsockfd < 0) 
			cerr << "ERROR on accept" << endl;
		memset(buffer, 256, sizeof(buffer));
		// receive data from client
		n = read(newsockfd,buffer,255);
		if (n < 0) 
			cerr << "ERROR reading from socket" << endl;
		else
			cout << "Running..." << endl;

		string early_pay;
		string mid_pay;
		vector <pair<string, string> > salary;
		
		string major = buffer;
		
		cout << "major is-" << major << "-" << endl;
		
		// if received data not in the map, return -1 to client
		// then continue wait for next request
		if (read_from_file.find(major) == read_from_file.end())
		{
			n = write(newsockfd,"-1",255);
			if (n < 0) 
				cerr << "ERROR writing to socket" << endl;
			continue;
		}
		
		// else base on the major, find the early and mid pay
		salary = read_from_file[major];
		early_pay = salary.front().first;
		mid_pay = salary.front().second;
		memset(buffer, 256, sizeof(buffer));
		string c = early_pay + " " + mid_pay;
		int arrayLength = c.length();
		for (int i = 0; i < arrayLength; i++)
		{
			buffer[i] = c[i];
		}
		
		// send to the client
		n = write(newsockfd,buffer,255);
		if (n < 0) 
			cerr << "ERROR writing to socket" << endl;
	}

	return 0; 
}