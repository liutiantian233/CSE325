/****************************************************************************
Tianrui Liu
Computer Project #5
****************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <cctype>
#include <pthread.h>

#define PRINT_ERROR

using namespace std;

/*---------------------------------------------------------------------------
Name: extern_cmd
Purpose: for extern command
Return: no return
---------------------------------------------------------------------------*/
void *extern_cmd(void *cmd) {
    //thread function for extern command by calling system()
    cout << (char *) cmd << endl;
    system((char *) cmd);
    pthread_exit(nullptr);
}

/*---------------------------------------------------------------------------
Name: display_time
Purpose: Show time
Return: no return
---------------------------------------------------------------------------*/
void display_time() {
    //for display time
    time_t t;
    time(&t);
    cout << "date: " << ctime(&t);
}

/*---------------------------------------------------------------------------
Name: display_cwd
Purpose: Show the cwd case
Return: no return
---------------------------------------------------------------------------*/
void display_cwd() {
    //for display cwd
    char *path = nullptr;
    path = getcwd(nullptr, 0);
    cout << "cwd: " << path << endl;
    free(path);
}

/*---------------------------------------------------------------------------
Name: display_env
Purpose: Show the env case
Return: no return
---------------------------------------------------------------------------*/
extern char **environ;

void display_env() {
    //for display env
    cout << "env: " << endl;
    for (int i = 0; environ[i] != nullptr; ++i) {
        cout << "    " << environ[i] << endl;
    }
}

/*---------------------------------------------------------------------------
Name: display_path
Purpose: Show the path case
Return: no return
---------------------------------------------------------------------------*/
void display_path() {
    //for display path
    cout << "path: " << getenv("PATH") << endl;

}

/*---------------------------------------------------------------------------
Name: display_cd
Purpose: Show the cd case
Return: no return
---------------------------------------------------------------------------*/
void display_cd(string &line) {
    //for operate 'cd'
    char first[10], last[30];
    last[0] = '\0';
    sscanf(line.c_str(), "%s %s", first, last);

    char *path = nullptr;
    path = getcwd(nullptr, 0);
    if (last[0] == '\0')  // The directive "cd" without any other tokens
    {
        cout << "cwd: " << path << endl;
        setenv("HOME", path, 1);
        cout << "set current USER HOME to be " << path << "!" << endl;

    } else {
        if (last[0] == '~')  // The directive "cd ~USER"
        {
            char *login = getlogin();
            char d[100] = "/user/";
            strcat(d, login);
            if (strcmp(last, "~") == 0 || (last[0] == '~' && strcmp(last + 1, login) == 0)) {

                setenv("PWD", d, 1);
#ifdef PRINT_ERROR
                cout << "set USER " << last << "'s HOME to be " << d << "!" << endl;
#endif
            } else {
                char dest[100] = "/user/";
                strcat(dest, last + 1);
                setenv("PWD", dest, 1);
#ifdef PRINT_ERROR
                cout << "set USER " << last << "'s HOME to be " << dest << "!" << endl;
#endif
            }


        } else  // The directive "cd DIR"
        {
            chdir(last);
        }
    }
    free(path);
}

/*---------------------------------------------------------------------------
Name: display_set
Purpose: Show the set case
Return: no return
---------------------------------------------------------------------------*/
void display_set(string &line) {
    //for operate 'set'
    char first[10], mid[30], last[50];
    last[0] = '\0';
    mid[0] = '\0';
    sscanf(line.c_str(), "%s %s %s", first, mid, last);

    if (last[0] != '\0')  // The directive "set VAR VALUE"
    {
        int f = 1;
        if (getenv(mid) == nullptr)  // if the env mid exits!
            f = 0;
        if (0 != setenv(mid, last, f)) {
#ifdef PRINT_ERROR
            cout << "set env " << mid << "to " << last << " fail!" << endl;
#endif
            return;
        } else {
#ifdef PRINT_ERROR
            cout << "set env " << mid << "to " << last << " success!" << endl;
#endif
        }
    } else  // The directive "set VAR "
    {
        if (0 != unsetenv(mid)) {
#ifdef PRINT_ERROR
            cout << "unset env " << mid << " fail!" << endl;
#endif
            return;
        } else
            cout << "unset env " << mid << " success!" << endl;

    }

}

/*---------------------------------------------------------------------------
Name: exe
Purpose: Show the exe case
Return: return int to make sure true and false
 ---------------------------------------------------------------------------*/
int exe(string &cmd) {
    //read cmd and exe it
    if (cmd.length() == 0) {  // empty line
        cout << endl;
        return 1;
    } else if (cmd.find("exit") != string::npos) {
        return 0;
    } else if (cmd.find("date") != string::npos) {
        display_time();
    } else if (cmd.find("cwd") != string::npos) {
        display_cwd();
    } else if (cmd.find("path") != string::npos) {
        display_path();
    } else if (cmd.find("env") != string::npos) {
        display_env();
    } else if (cmd.find("cd") != string::npos) {
        display_cd(cmd);
    } else if (cmd.find("set") != string::npos) {
        display_set(cmd);
    } else {
        pthread_t id;
        if (0 != pthread_create(&id, nullptr, extern_cmd, (void *) cmd.c_str())) {
#ifdef PRINT_ERROR
            cout << "thread create fail!" << endl;
#endif
            return -1;
        }
        pthread_join(id, nullptr);
        return 1;

    }
    return 1;
}

/*---------------------------------------------------------------------------
Name: main function
Purpose: Show all code and main function to run
Return: no return
---------------------------------------------------------------------------*/
int main(int argc, const char *argv[]) {

    bool verbose_silent = true;  // true for verbose; false for silent
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {  // the argument head is '-'
            if (strcmp(argv[i], "-v") == 0)
                verbose_silent = true;
            else if (strcmp(argv[i], "-s") == 0)
                verbose_silent = false;
            else {
#ifdef PRINT_ERROR
                cout << "error argument!(" << argv[i] << ")" << endl;
#endif
                return -1;
            }

        } else {
            cout << argv[i] << "  ";
            if (verbose_silent)
                cout << "-verbose" << endl;
            else
                cout << "-silent" << endl;
            if (access(argv[i], F_OK) != 0) {  // make sure the file exists!
#ifdef PRINT_ERROR
                cout << "file (" << argv[i] << ") don't exist!" << endl;
#endif
                return -2;
            }
            ifstream in(argv[i]);

            if (!in.is_open()) {
#ifdef PRINT_ERROR
                cout << "open file (" << argv[i] << ") error!" << endl;
#endif
                return -2;
            }
            string line;
            long count = 1;

            while (getline(in, line)) {  // read one line
                if (verbose_silent) {  // verbose
                    cout << "<" << count++ << "> " << line << endl;
                }
                string t;
                for (auto &e:line) {
                    if (e != '\n' && e != '\r' && e != '\t')
                        t += e;
                }
                line = t;
                int rr = exe(line);
                if (rr == 0)  // exit the file
                {
                    cout << "\"exit\" read!" << endl;
                    break;
                } else if (rr != 1) {
                    cout << "cmd exe error code(" << rr << ")" << endl;
                } else;

            }
            in.close();  // close file
        }
        cout << endl;
    }

    return 0;
}

