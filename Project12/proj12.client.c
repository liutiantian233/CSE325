/****************************************************************************
Tianrui Liu
Computer Project #12
****************************************************************************/
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>

using namespace std;

/*---------------------------------------------------------------------------
Name: Stimulation
Purpose: Stimulation class, which contains all Stimulation operations
Return: class no return
---------------------------------------------------------------------------*/
class Stimulation {
private:
    string host;
    int port;
    string filename;
    struct sockaddr_in address{};
    int socket_id;
    const int BUFFER_SIZE = 128;
    ofstream file_stream;

public:

    Stimulation(string h, int p, string f) : host(move(h)), port(p), filename(move(f)) {
        cout << host << " " << p << "  " << filename << endl;
        file_stream.open(filename, ios::ate);
        socket_id = -1;
        socket_id = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_id < 0) {
            cout << "created socket error!" << endl;
            abort();
        }
        struct hostent *server_host = nullptr;
        server_host = gethostbyname(host.c_str());
        if (server_host == nullptr) {
            cout << "get host by name (" << host << ") error!" << endl;
            abort();
        }
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        memcpy(&address.sin_addr.s_addr, server_host->h_addr, server_host->h_length);
        address.sin_port = htons(port);
        if (connect(socket_id, (struct sockaddr *) &address, sizeof(address)) < 0) {
            cout << "connected to host server error!" << endl;
            abort();
        }
    }

    /*---------------------------------------------------------------------------
    Name: Class Send_FileName
    Purpose: The function that Send_FileName in operations
    Return: no return
    ---------------------------------------------------------------------------*/
    int Send_FileName() {
        char file_desired[BUFFER_SIZE];
        memcpy(file_desired, filename.c_str(), strlen(filename.c_str()));
        if (send(socket_id, file_desired, strlen(file_desired), 0) < 0) {
            cout << "send file name (" << filename << ") error!!!" << endl;
            return -1;
        }
        int size_rec = recv(socket_id, file_desired, BUFFER_SIZE, 0);
        if (size_rec < 0) {
            cout << "receive server respond  error!!!" << endl;
            return -2;
        }
//        cout << file_desired << endl;
        file_desired[7] = '\0';
        if (strcmp(file_desired, "SUCCESS") == 0)
            return 1;
        else if (strcmp(file_desired, "FAILURE") == 0) {
            cout << " the server is unable to open the specified file name (" << filename << ")." << endl;
            return -3;
        } else {
            cout << "unknown command (" << file_desired << ")." << endl;
            return -4;
        }
    }

    /*---------------------------------------------------------------------------
    Name: Class Receive_File
    Purpose: The function that Receive_File in operations
    Return: no return
    ---------------------------------------------------------------------------*/
    int Receive_File() {
        char buffer[BUFFER_SIZE];
        char buffer_with_end[BUFFER_SIZE + 1];
        memcpy(buffer, "PROCEED", strlen("PROCEED"));
        if (send(socket_id, buffer, strlen(buffer), 0) < 0) {
            cout << "send command(PROCEED) error!!!" << endl;
            return -1;
        }
        while (true) {
            int ret = recv(socket_id, buffer, BUFFER_SIZE, 0);
            if (ret < 0) {
                cout << "receive file interrupt!!!" << endl;
                return -2;
            }
            memset(buffer_with_end, 0, BUFFER_SIZE + 1);
            memcpy(buffer_with_end, buffer, BUFFER_SIZE);
            buffer[BUFFER_SIZE] = '\0'; //i have to add this for output
            string r = buffer_with_end;
            file_stream << r;
            write(STDOUT_FILENO, buffer_with_end, strlen(buffer_with_end));
            if (strlen(buffer) == 0)
                break;
            memset(buffer, 0, BUFFER_SIZE);
        }

        return 1;
    }

    /*---------------------------------------------------------------------------
    Name: Class destructor function
    Purpose: The function that destructor in Stimulation
    Return: no return
    ---------------------------------------------------------------------------*/
    ~Stimulation() {
        close(socket_id);
        file_stream.close();
    }

};

/*---------------------------------------------------------------------------
Name: main function
Purpose: Show all code and main function to run
Return: no return
---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {

    if (argc != 4) {
        cout << "argument error!!! hostname port filename are required!!!" << endl;
        return -1;
    }
    int p = (int) strtol(argv[2], &argv[2], 10);
    Stimulation S(string(argv[1]), p, string(argv[3]));
    if (S.Send_FileName() != 1)
        return -2;

    cout << endl << "get file " << argv[3];
    if (S.Receive_File() == 1) {
        cout << " success!" << endl;
        return 1;
    } else {
        cout << " failure!" << endl;
        return -3;
    }

}