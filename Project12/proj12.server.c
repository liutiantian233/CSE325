/****************************************************************************
Tianrui Liu
Computer Project #12
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <algorithm>
#include <arpa/inet.h>

using namespace std;

/*---------------------------------------------------------------------------
Name: Server
Purpose: Server class, which contains all Server operations
Return: class no return
---------------------------------------------------------------------------*/
class Server {
private:
    int fd_listen;
    int fd_connect;
    struct sockaddr_in address{};
    const int buffer_size = 128;
    string file;
public:
    Server() {
        fd_listen = -1;
        fd_connect = -1;
    }

    ~Server() {
        close(fd_listen);
        close(fd_connect);
    }

    /*---------------------------------------------------------------------------
    Name: Class Start
    Purpose: The function that Start in operations
    Return: no return
    ---------------------------------------------------------------------------*/
    void Start() {
        fd_listen = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_listen < 0) {
            cout << "create socket error!" << endl;
            exit(0);
        }
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(0);

        if (bind(fd_listen, (struct sockaddr *) &address, sizeof(address)) < 0) {
            cout << "bind error!" << endl;
            exit(0);
        }
        if (listen(fd_listen, 32) < 0) {
            cout << "listen error!" << endl;
            exit(0);
        }
        socklen_t temp = sizeof(address);
        if (getsockname(fd_listen, (struct sockaddr *) &address, &temp) < 0) {
            cout << "getsockname error!" << endl;
            exit(0);
        }
        char hostname[256];
        gethostname(hostname, 256);
        cout << hostname << " " << ntohs(address.sin_port) << endl;
    }

    /*---------------------------------------------------------------------------
    Name: Class Get_FileName
    Purpose: The function that Get_FileName in operations
    Return: no return
    ---------------------------------------------------------------------------*/
    void Get_FileName() {
        struct sockaddr_in sock{};
        int sock_length = sizeof(sock);
        fd_connect = accept(fd_listen, reinterpret_cast<sockaddr *>(&sock),
                            reinterpret_cast<socklen_t *>(&sock_length));
        if (fd_connect < 0) {
            cout << "accept error!" << endl;
            exit(0);
        }
        struct hostent *host_infomation = gethostbyaddr(&sock.sin_addr.s_addr, sizeof(sock.sin_addr.s_addr), AF_INET);
        if (host_infomation == nullptr) {
            cout << "gethostbyaddr error!" << endl;
            exit(0);
        }
        char *host = inet_ntoa(sock.sin_addr);
        if (host == nullptr) {
            cout << "inet_ntoa error!" << endl;
            exit(0);
        }
        char B[buffer_size];
        int len = recv(fd_connect, B, buffer_size, 0);
        if (len < 0) {
            cout << "receive file name error!" << endl;
            exit(0);
        }
        B[len] = '\0';
        file = B;

    }

    /*---------------------------------------------------------------------------
    Name: Class Send_File
    Purpose: The function that Send_File in operations
    Return: no return
    ---------------------------------------------------------------------------*/
    void Send_File() {
        ifstream in(file, ios::in);

        char buffer[buffer_size];
        memset(buffer, 0, buffer_size);
        if (in.is_open())
            strcpy(buffer, "SUCCESS");
        else
            strcpy(buffer, "FAILURE");
//        buffer[7] = '\0';

        if (send(fd_connect, buffer, strlen(buffer), 0) < 0) {
            cout << "send information " << buffer << " error!!!" << endl;
            exit(0);
        }

        if (recv(fd_connect, buffer, buffer_size, 0) < 0) {
            cout << "receive file name error!" << endl;
            exit(0);
        }
        if (strcmp(buffer, "PROCEED") != 0) {
            cout << "client don't send PROCEED!" << endl;
            exit(0);
        }


        auto &&read = [](ifstream &f, char *B, const int _max_) {
            int size = 0;
            char c;
            while (f.get(c) && size < _max_) {
                B[size++] = c;
            }
            return size;
        };

        while (true) {
            int length = read(in, buffer, buffer_size);
            if (length == 0)
                break;
            buffer[length] = '\0';

            if (send(fd_connect, buffer, length, 0) < 0) {
                cout << "send information " << buffer << " error!!!" << endl;
                exit(0);
            }
        }
        in.close();

    }

};

/*---------------------------------------------------------------------------
Name: main function
Purpose: Show all code and main function to run
Return: no return
---------------------------------------------------------------------------*/
int main() {
    Server S;
    S.Start();
    S.Get_FileName();
    S.Send_File();
    return 1;
}