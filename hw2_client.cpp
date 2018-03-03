/*
Description: A client program that will connect with your server and
send it requests for the average early career and midcareer
pays for a specific college major, say “Hospitality management.”
compile: g++ filename.cpp -o filename.out -std=c++11
run: ./filename.out
2 inputs: 
host name
port number

output: 
stdout
*/

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <cstring>
#include <unistd.h>
#include <sstream>

using namespace std;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    string argument;

    char buffer[256];
    
    cout << "Enter server host name: ";
    cin.getline(buffer, 256);
    server = gethostbyname(buffer);
    if (server == NULL) {
        cerr << "ERROR, no such host\n";
        exit(0);
    }

    memset(buffer, 256, sizeof(buffer));
    cout << "Enter server port number: ";
    cin.getline(buffer, 256);
    portno = atoi(buffer);

    while(true)
    {
        // create socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
            cerr << "ERROR opening socket" << endl;
        
        memset((char *) &serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
             (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);
        serv_addr.sin_port = htons(portno);
    
        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
            cerr << "ERROR connecting" << endl;
        
        memset(buffer, 256, sizeof(buffer));
        cout << "Enter a college major: ";
        cin.getline(buffer, 255);

        string major = buffer;
        major = major.substr(0, major.size() - 1);
        // if the user enters an empty string, then exit
        if (major == "")
        {
            break;
        }

        // else send the data to server
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             cerr << "ERROR writing to socket" << endl;
    
        memset(buffer, 256, sizeof(buffer));

        // receive the reply from the server
        n = read(sockfd,buffer,255);
        string early_pay;
        string mid_pay;
        if (n < 0) 
             cerr << "ERROR reading from socket" << endl;
        else
        {
            istringstream iss(buffer);
            iss >> early_pay;
            // if the reply is -1, means the major not in the table
            if (early_pay == "-1")
            {
                cout << "That major is not in the table" << endl << endl;
            }
            else
            {   // print the information get from the server
                iss >> mid_pay;
                cout << "The average early career pay for a " << major
                     << " major is " << early_pay << endl;
                cout << "The corresponding mid-career pay is " << mid_pay << endl;
                cout << endl;
            }
        }
    }
    return 0;
}
