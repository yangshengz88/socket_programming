#include <iostream>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <getopt.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

#define BUFFERSIZE 1024

using namespace std;

int main()
{
    char portNum[8] = "38918";      
    char filename[] = "myfile.txt"; 
    int socketFD, newSocketFD;
    struct addrinfo hints; 
    struct addrinfo *servInfo, *s;
    struct sockaddr_storage clientAddr;
    socklen_t clientAddrSize;
    int yes = 1;
    int file;

    // load up address structs with getaddrinfo()
    memset( &hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((getaddrinfo(NULL, portNum, &hints, &servInfo)) != 0){
        cerr<<"Server: Failed to get address info."<< endl;
        exit(1);
    };

    // create a socket, bind it
    for (s = servInfo; s !=NULL; s = s ->ai_next){
        if((socketFD = socket(s -> ai_family, s ->ai_socktype, s ->ai_protocol))== -1 ) continue;
        if(setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes)== -1){
            cerr<< "Setsockopt"<<endl;
            exit(1);
        }
        if (bind(socketFD, s->ai_addr, s-> ai_addrlen)==-1){
            close(socketFD);
            continue;
        }
        break;
    }

    if (s == NULL){
        cerr <<"Server: Failed to bind."<< endl;
        exit(1);
    }

    freeaddrinfo(servInfo);
    
    // listening 
    if(listen(socketFD, 5) == -1){
        cerr<< "Server: Failed to Listen."<< endl;
        exit(1);
    }

    while(1){
        clientAddrSize = sizeof clientAddr;
        // accept an incoming connection
        newSocketFD = accept(socketFD, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (newSocketFD == -1){
            cerr<<"Server: Failed to accept client socket." << endl;
            exit(1);
        }
        // send the file
        file = open(filename, O_RDONLY, S_IRUSR | S_IWUSR );
        if(file == -1 ){
            cerr<< "Server: Failed to open file."<< endl;
            exit(1);
        }
        
        char data[BUFFERSIZE];
        int length;
        while((length = read(file, data, BUFFERSIZE))> 0){
            int total = 0;
            int bytesLeft;
            int n;

            bytesLeft = length;
            while (total < length){
                n = send(newSocketFD, data + total, bytesLeft, 0);
                if (n == -1) {
                    cerr <<"Client: Failed to send."<<endl;
                    exit(1);
                }
                total += n;
                bytesLeft -= n;
            }
            memset(data, 0, BUFFERSIZE);

        }
        close(file);
        cout << "Server: Finish sending!"<< endl;
        close(newSocketFD);
    }

    return 0;

}