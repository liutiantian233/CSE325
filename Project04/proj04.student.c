 /****************************************************************************
 Tianrui Liu
 Computer Project #4
 ****************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <cctype>
#include <climits>

#define PRINT_ERROR

using namespace std;


 /*---------------------------------------------------------------------------
 Name: display_time
 Purpose: Show time
 Return: no return
 ---------------------------------------------------------------------------*/
void display_time() {  //for display time
    time_t t;
    time(&t);
    cout << "date: " << ctime(&t);
}

 /*---------------------------------------------------------------------------
 Name: display_cwd
 Purpose: Show the cwd case
 Return: no return
 ---------------------------------------------------------------------------*/
void display_cwd() { //for display cwd
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
void display_env() { //for display env
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
void display_path() { //for display path
    cout << "path: " << getenv("PATH") << endl;

}

 /*---------------------------------------------------------------------------
 Name: exe
 Purpose: Show the exe case
 Return: return int to make sure true and false
 ---------------------------------------------------------------------------*/
int exe(string &cmd) { //read cmd and exe it
//    cout << cmd << " " << cmd.length() << endl;
    if (cmd.length() == 0) { //empty line
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
    } else {
#ifdef PRINT_ERROR
        cout << "unrecognizable command!" << endl;
#endif
        return -1;
    }
    return 1;
}

 /*---------------------------------------------------------------------------
 Name: format
 Purpose: Show the format the string when have string inline
 Return: return int to make sure true and false
 ---------------------------------------------------------------------------*/
int format(string &line, bool v_b, long &count) { //format it

    if (v_b) {
        if (line[0] != '<') { //begin with '<'
#ifdef PRINT_ERROR
            cout << "'<' not find!" << endl;
#endif
            return -1;
        }
        auto pos = line.find('>');
        if (pos == std::string::npos)  //if not find '>'
        {
#ifdef PRINT_ERROR
            cout << "'>'  not find!" << endl;
#endif
            return -2;
        }
        long n;
        n = strtol(line.substr(1, pos - 1).c_str(), nullptr, 10);
        cout << "<" << n << "> ";
        if (n == LLONG_MAX || count != n) {  //number wrong
#ifdef PRINT_ERROR
            cout << "sequence number is not correct!" << endl;
#endif
            return -3;
        }
        count++;
        line = line.substr(pos + 1);
    }
    string t;
    for (auto &i :line) {
        if (isalpha(i))
            t += i; //copy to temp
        else if (i == ' ' || i == '\n' || i == '\t' || i == '\0' || i == '\r')
            continue; //jump if whitespace or newline etc..
        else { //unknown char
#ifdef PRINT_ERROR
            cout << i << " in " << line << "command line is not correct!" << endl;
#endif
            return -4;
        }
    }
    line = t;
//    cout << line << endl;
    return 1;
}

 /*---------------------------------------------------------------------------
 Name: main function
 Purpose: Show all code and main function to run
 Return: no return
 ---------------------------------------------------------------------------*/
int main(int argc, const char *argv[]) {

    bool verbose_silent = true;    //true for verbose ;false for silent
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') { //the argument head is '-'
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
            if (access(argv[i], F_OK) != 0) {  //make sure the file exists!
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

            while ( getline(in, line)) { //read one line
                int r = format(line, verbose_silent, count);
                if (r == 1) {
                    //change the format and check the number at the begin of line when verbose
                    int rr = exe(line);
                    if (rr == 0) //exit the file
                    {
                        cout << "\"exit\" read!" << endl;
                        break;
                    } else if (rr != 1) {
                        cout << "cmd exe error code(" << rr << ")" << endl;
                    } else;
                } else {
                    cout << "file read interrupt!! format line error code(" << r << ")" << endl; //read wrong line
                    break;
                }
            }
            in.close();   //close file
        }
        cout << endl;
    }

    return 0;
}


