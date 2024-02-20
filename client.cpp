#include <iostream>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <getopt.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFERSIZE 1024

using namespace std; 

int main()
{
    char hostname[] = "localhost";
    char portNum[8] = "38918";
    char filename[] = "receivedfile.txt";
    int clientSocketFD;
    struct addrinfo hints, *servInfo, *s;
    int file;
    
    // load up address structs with getaddrinfo()
    memset( &hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((getaddrinfo(hostname, portNum, &hints, &servInfo)) != 0){
        cerr<< "Client: Failed to get address info." << endl;
        exit(1);
    };

    // create a socket, connect to the server
    for (s = servInfo; s !=NULL; s = s ->ai_next){
        if((clientSocketFD = socket(s -> ai_family, s ->ai_socktype, s ->ai_protocol))== -1 ) continue;

        if ((connect(clientSocketFD, s->ai_addr, s-> ai_addrlen)==-1)){
            close(clientSocketFD);
            continue;
        }
        break;
    }
    
    if (s == NULL){
        cerr << "Client: Failed to connect."<<endl;
        exit(1);
    }

    freeaddrinfo(servInfo);

    // receive file
    char buf[BUFFERSIZE];
    int n;
    file = open(filename, O_WRONLY | O_CREAT , S_IRUSR | S_IWUSR );

    while(1){
        if ((n =(recv(clientSocketFD, buf, BUFFERSIZE, 0)))<=0){
            break;
        }
        write(file, buf, n);
        memset(buf, 0, BUFFERSIZE);
    }
    close(file);
    close(clientSocketFD);

    return 0;
}