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

map< string, vector<pair<int, int> > > read_from_file;

void savefromfile(istringstream &iss)
{
	stack <string> readofline;
	string word;
	string temp;
	string major;
	int mid_pay;
	int early_pay;
	while(iss >> word)
	{
		readofline.push(word);
	}

	mid_pay = stoi(readofline.top());
	readofline.pop();
	early_pay = stoi(readofline.top());
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
	vector <pair<int, int> > salary;
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

	int early_pay;
	int mid_pay;
	vector <pair<int, int> > salary;
	//stringstream ss;
	string major = buffer;
	major = major.substr(0, major.size() - 1);
	//ss << buffer;
	//ss >> major;
	cout << "major is-" << major << "-" << endl;
	//map<char, int>::iterator it;
	//it = read_from_file.find(buffer);
	if (read_from_file.find(major) == read_from_file.end())
	{
		cout << "That major is not in the table" << endl;
		return 0;
	}
	
	salary = read_from_file[major];
	early_pay = salary.front().first;
	mid_pay = salary.front().second;

	cout << "Enter a college major: " << major << endl;


	cout << "The average early career pay for a " << major
		 << " major is " << early_pay << endl;
	cout << "The corresponding mid-career pay is " << mid_pay << endl;
	
	n = write(newsockfd,"I got your message",18);
	if (n < 0) 
		cerr << "ERROR writing to socket" << endl;
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