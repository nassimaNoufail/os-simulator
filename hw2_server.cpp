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

map< string, vector<pair<string, string> > > read_from_file;

void savefromfile(istringstream &iss)
{
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

	mid_pay = readofline.top();
	readofline.pop();
	early_pay = readofline.top();
	readofline.pop();
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
	vector <pair<string, string> > salary;
	salary.push_back(make_pair(early_pay, mid_pay));
	read_from_file[major] = salary;	
}

int readfromfile(char const *argc)
{
	ifstream file(argc);
	
	if(!file) 
	{
    	cout << "Cannot open input file.\n";
    	return 1;
  	}

  	string str;

	while(file && getline(file, str)) 
	{
    	istringstream iss(str);	
  		savefromfile(iss);
  	}
}



int main(int argc, char const *argv[])
{
	
	readfromfile(argv[1]);

	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	if (argc < 3) 
	{
		cerr << "ERROR, no port provided\n" << endl;
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
        cerr << "ERROR opening socket" << endl;
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    memset((char *) &serv_addr, 0, sizeof(serv_addr));

	portno = atoi(argv[2]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
	      sizeof(serv_addr)) < 0) 
		cerr << "ERROR on binding" << endl;
	
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	while(true)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen);
		if (newsockfd < 0) 
			cerr << "ERROR on accept" << endl;
		else
			cout <<"newsockfd = " << newsockfd << endl;
		  
		//bzero(buffer,256);
		memset(buffer, 256, sizeof(buffer));

		n = read(newsockfd,buffer,255);
		if (n < 0) 
			cerr << "ERROR reading from socket" << endl;
		else
			cout <<"n = " << n << " buffer = " << buffer << endl;

		string early_pay;
		string mid_pay;
		vector <pair<string, string> > salary;
		//stringstream ss;
		string major = buffer;
		major = major.substr(0, major.size() - 1);
		cout << "major is-" << major << "-" << endl;
		//map<char, int>::iterator it;
		//it = read_from_file.find(buffer);
		if (read_from_file.find(major) == read_from_file.end())
		{
			n = write(newsockfd,"-1",255);
			if (n < 0) 
				cerr << "ERROR writing to socket" << endl;
			
			// n = write(newsockfd,"-1 ",18);
			// if (n < 0) 
			// 	cerr << "ERROR writing to socket" << endl;

			continue;
		}
		
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
		
		n = write(newsockfd,buffer,255);
		if (n < 0) 
			cerr << "ERROR writing to socket" << endl;
	}

	return 0; 
	// cout << "file information: " << endl;
	// for (auto & element : read_from_file)
	// {
	// 	cout << element.first << "  ";
	// 	for(auto& ele: element.second)
	// 		cout << ele.first << " " << ele.second << " " << endl;
	// }
	// return 0;
}