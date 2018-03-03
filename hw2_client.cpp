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

    char buffer[256];
    if (argc < 3) {
       cerr << "usage "<< argv[0] <<" hostname port" << endl;
       exit(0);
    }
    portno = atoi(argv[2]);
    while(true)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
            cerr << "ERROR opening socket" << endl;
        server = gethostbyname(argv[1]);
        if (server == NULL) {
            cerr << "ERROR, no such host\n";
            exit(0);
        }
        //bzero((char *) &serv_addr, sizeof(serv_addr));
        memset((char *) &serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
             (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);
        serv_addr.sin_port = htons(portno);
    
        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
            cerr << "ERROR connecting" << endl;
        
        cout << "Enter a college major: ";
        //bzero(buffer,256);
        memset(buffer, 256, sizeof(buffer));
        fgets(buffer,255,stdin);
        cout << "input = " << buffer << endl;
        string major = buffer;
        major = major.substr(0, major.size() - 1);
        if (major == "")
        {
            break;
        }
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             cerr << "ERROR writing to socket" << endl;
        else
            cout << "n = " << n << endl;
        //bzero(buffer,256);
        memset(buffer, 256, sizeof(buffer));
        n = read(sockfd,buffer,255);
        string early_pay;
        string mid_pay;
        if (n < 0) 
             cerr << "ERROR reading from socket" << endl;
        else
        {
            istringstream iss(buffer);
            iss >> early_pay;
            cout << "early_pay = " << early_pay << endl;
            if (early_pay == "-1")
            {
                cout << "That major is not in the table" << endl;
            }
            else
            {
                iss >> mid_pay;
                cout << "The average early career pay for a " << major
                     << " major is " << early_pay << endl;
                cout << "The corresponding mid-career pay is " << mid_pay << endl;
            }
        }
    }
    return 0;
}
