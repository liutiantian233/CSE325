/****************************************************************************
Tianrui Liu
Computer Project #5
****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <algorithm>
#include <cassert>
#include <vector>
#include <sstream>
#include <ctime>

using namespace std;

/*---------------------------------------------------------------------------
Name: _Log
Purpose: for write log
Return: class no return
---------------------------------------------------------------------------*/
class _Log {  //A class for write log
private:
    ofstream out;

    /*---------------------------------------------------------------------------
    Name: GetCurrentTime
    Purpose: the time of log
    Return: return string of time
    ---------------------------------------------------------------------------*/
    static string GetCurrentTime() { //time
        time_t t;
        time(&t);
        string s(ctime(&t));
        s.pop_back();
        s += ":  ";
        return s;
    }

public:
    _Log() {
        out.open("accout.log", ios::app);
        if (!out.is_open())
            cout << "open log file fail!" << endl;
    }

    ~_Log() {
        out.close();
    }

    void Write(string &s) {
        out << GetCurrentTime();
        out << s << endl;
    }

};

/*---------------------------------------------------------------------------
Name: _Accounts
Purpose: for all account
Return: class no return
---------------------------------------------------------------------------*/
class _Accounts { //A class for all account
private:
    struct node {
        int number;
        double balance;
    };
    vector<node> accounts;
    double Before_Balance = -1; //for store the current balance before trans
    double Current_Balance = -1; //for store the current balance before trans

public:
    _Accounts() {
        ifstream in;
        in.open("accounts.old");
        if (!in.is_open()) {
            cout << "open accounts.old fail!" << endl;
            return;
        }
        stringstream ss;
        string line;
        while (getline(in, line)) {
//        while (!in.eof()) {
            ss.clear();
            ss << line;
            node a{};
            ss >> a.number;
            ss >> a.balance;
            accounts.push_back(a);
        }
        in.close();
    }

    double Get_Before_Balance() {
        return Before_Balance;

    }

    double Get_Current_Balance() {
        return Current_Balance;

    }
//    void Print() {
//        for_each(accounts.begin(), accounts.end(), [](auto &e) {
//            cout << e.number << "  " << e.balance << endl;
//        });
//    }

    /*---------------------------------------------------------------------------
    Name: Withdraw
    Purpose: for loop the accounts
    Return: no return
    ---------------------------------------------------------------------------*/
    int Withdraw(int n, double t) {
        int r = 1;
        Before_Balance = -1;
        Current_Balance = -1;
//        for (unsigned int i = 0; i < accounts.size(); ++i) {
//            if (accounts[i].number == n) {
//                if (accounts[i].balance >= t) {
//                    Before_Balance = accounts[i].balance;
//                    accounts[i].balance -= t;
//                    Current_Balance = accounts[i].balance;
//                } else
//                    r = -1;
//
//            }
//        }
        for_each(accounts.begin(), accounts.end(), [&](auto &e) {
            if (e.number == n) {
                if (e.balance >= t) {
                    Before_Balance = e.balance;
                    e.balance -= t;
                    Current_Balance = e.balance;
                } else
                    r = -1;

            }
        });
        return r;
    }

    /*---------------------------------------------------------------------------
    Name: Deposit
    Purpose: for loop the accounts
    Return: no return
    ---------------------------------------------------------------------------*/
    void Deposit(int n, double t) {
        Before_Balance = -1;
        Current_Balance = -1;
//        for (unsigned int i = 0; i < accounts.size(); ++i) {
//            if (accounts[i].number == n) {
//
//                Before_Balance = accounts[i].balance;
//                accounts[i].balance += t;
//                Current_Balance = accounts[i].balance;
//
//            }
//        }
        for_each(accounts.begin(), accounts.end(), [&](auto &e) {
            if (e.number == n) {
                Before_Balance = e.balance;
                e.balance += t;
                Current_Balance = e.balance;
            }

        });
    }

    ~_Accounts() {
        ofstream out;
        out.open("accounts.new", ios::trunc);
        if (!out.is_open()) {
            cout << "Can not open accounts.new" << endl;
            return;
        }
        for (auto &e : accounts) {
            out << e.number << " " << e.balance << endl;
        }
        out.close();
    }
};

/*---------------------------------------------------------------------------
Name: _Record
Purpose: for store the every _Record
Return: struct no return
---------------------------------------------------------------------------*/
struct _Record { // for store the every _Record
    int thread = -1;
    int account_Number = -1;
    string tran_type;
    double money = 0.0;
};


int b = 5, p = 1;
int *arg_id = nullptr;
int buffer_top = 0, sum = 0, items = 0; //cursor
sem_t FULL, BUFFER, EMPTY; //semaphore
vector<_Record> Record;  //buffer
vector<pthread_t> tid;  //thread id
_Accounts *A = nullptr;

/*---------------------------------------------------------------------------
Name: _Threads
Purpose: for loop every _Threads
Return: class no return
---------------------------------------------------------------------------*/
class _Threads {

public:

    /*---------------------------------------------------------------------------
    Name: Consumer
    Purpose: for the Consumer with while loop
    Return: no return
    ---------------------------------------------------------------------------*/
    static void *Consumer(void *arg) {
        _Log log;
        int t = 0;
//        cout << "consumer" << endl;
        while (sum != p || items > 0) {
            _Record a;
            /// read from buffer
            sem_wait(&EMPTY);
            sem_wait(&BUFFER);
            a.account_Number = Record[t].account_Number;
            a.thread = Record[t].thread;
            a.tran_type = Record[t].tran_type;
            a.money = Record[t].money;
            t = (t + 1) % b;
            items--;

            int _success;
            if (a.tran_type == "withdraw") {
                int r = A->Withdraw(a.account_Number, a.money);
                if (r != 1) {
                    cout << a.account_Number << "'s balance " << a.money << " is not enough to be minus " << a.money
                         << "!" << endl;
                    _success = false;
                } else
                    _success = true;
            } else if (a.tran_type == "deposit") {
                {
                    A->Deposit(a.account_Number, a.money);
                    _success = true;
                }
            } else {
                cout << a.tran_type << endl;
                cout << "error tran type" << a.tran_type << endl;
                _success = false;
            }
            sem_post(&BUFFER);
            sem_post(&FULL);
//            a) thread number where the transaction originated
//            b) account number
//            c) transaction type
//            d) current account balance
//            e) transaction amount
//            f) updated account balance

            string line;   //write log
            if (_success) {
                line += "thread number: ";
                line += to_string(a.thread);
                line += "; account number: ";
                line += to_string(a.account_Number);
                line += "; transaction type: ";
                line += a.tran_type;
                line += "; current account balance: ";
                line += to_string(A->Get_Before_Balance());
                line += "; transaction amount: ";
                line += to_string(a.money);
                line += "; updated account balance: ";
                line += to_string(A->Get_Current_Balance());
                line += ".";
                log.Write(line);
            } else {
                line += "thread number: ";
                line += to_string(a.thread);
                line += "; account number: ";
                line += to_string(a.account_Number);
                line += "; transaction type: ";
                line += a.tran_type;
                line += " fail!!!";
                log.Write(line);
            }
        }
        pthread_exit(nullptr);
    }

    /*---------------------------------------------------------------------------
    Name: Producer
    Purpose: for the Producer with while loop
    Return: no return
    ---------------------------------------------------------------------------*/
    static void *Producer(void *arg) {
        int nid = *(int *) arg;
//        cout << nid << endl;
        string name("trans");
        name += to_string(nid);
        ifstream in(name);
        if (!in.is_open()) {
            cout << "open file " << name << "fali!" << endl;
            return nullptr;
        }
        stringstream ss;
        string line;
        while (getline(in, line)) {

            sem_wait(&FULL);
            sem_wait(&BUFFER);
            Record[buffer_top].thread = nid;
            ss.clear();
            ss << line;
            ss >> Record[buffer_top].account_Number;
            ss >> Record[buffer_top].tran_type;
            ss >> Record[buffer_top].money;

//            in >> Record[buffer_top].account_Number;
//            in >> Record[buffer_top].tran_type;
//            in >> Record[buffer_top].money;

            buffer_top = (buffer_top + 1) % b;
            items++;
//            Record.push_back(a);
            sem_post(&BUFFER);
            sem_post(&EMPTY);
        }
        sum++;
        in.close();
        pthread_exit(nullptr);
    }

    /*---------------------------------------------------------------------------
    Name: Init
    Purpose: Init the threads
    Return: no return
    ---------------------------------------------------------------------------*/
    static void Init() { //init all variable
        sem_init(&BUFFER, 0, 1);
        sem_init(&EMPTY, 0, 0);
        sem_init(&FULL, 0, b);

        Record.resize(b);  //buffer
 
        A = new _Accounts;
        tid.resize(p + 1); //thread id
        arg_id = new int[p];

    }

    /*---------------------------------------------------------------------------
    Name: Begin
    Purpose: Begin the threads
    Return: no return
    ---------------------------------------------------------------------------*/
    static void Begin() { //start all threads


        for (int i = 0; i < p; ++i) {
            arg_id[i] = i;
        }
        for (int i = 0; i < p; ++i) {
//            pthread_create(&tid[i], nullptr, Call_Back(this), (void *) &id[i]);
            void *arg = (void *) &(arg_id[i]);
            pthread_create(&tid[i], nullptr, Producer, arg);  //producer

        }

        pthread_create(&tid[p], nullptr, Consumer, nullptr); //consumer

    }

    /*---------------------------------------------------------------------------
    Name: Exit
    Purpose: delete the value
    Return: no return
    ---------------------------------------------------------------------------*/
    static void Exit() {
        delete A;
        delete[]arg_id;
        sem_destroy(&FULL);
        sem_destroy(&BUFFER);
        sem_destroy(&EMPTY);
    }

    /*---------------------------------------------------------------------------
    Name: Join
    Purpose: for loop the p and join
    Return: no return
    ---------------------------------------------------------------------------*/
    static void Join() {
        for (int i = 0; i < p + 1; i++) {
            pthread_join(tid[i], nullptr);
        }
    }
};

/*---------------------------------------------------------------------------
Name: main function
Purpose: Show all code and main function to run
Return: no return
---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {

    assert((argc - 1) % 2 == 0);

    stringstream ss;
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-p") {
            ss.clear();
            ss << argv[++i];
            ss >> p;
            if (p > 10 || p < 1) {
                cout << "input argument  i_p out of range(1,10]!" << endl;
                return -1;
            }
//            cout << p << endl;
        } else if (string(argv[i]) == "-b") {
            ss.clear();
            ss << argv[++i];
            ss >> b;
            if (b > 20 || b < 1) {
                cout << "input argument  i_b out of range(1,20]!" << endl;
                return -1;
            }
//            cout << b << endl;
        } else {
            cout << "input argument error!" << endl;
            return -1;
        }
    }

    _Threads::Init();
    _Threads::Begin();
    _Threads::Join();
    _Threads::Exit();

    cout << "Done!" << endl;

    return 1;
}

