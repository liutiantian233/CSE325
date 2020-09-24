/****************************************************************************
Tianrui Liu
Computer Project #9
****************************************************************************/
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <sstream>


using namespace std;


/*---------------------------------------------------------------------------
Name: Page_Table
Purpose: Page_Table class, which contains all Page_Table operations
Return: class no return
---------------------------------------------------------------------------*/
class Page_Table {
private:
    class _page_table_item {
    public:
       uint8_t id, V, R, P, M, frame;

        _page_table_item() : id(0x00), V(0x00), R(0x00), P(0x00), M(0x00), frame(0xff) {};

        /*---------------------------------------------------------------------------
        Name: Class Set_Id
        Purpose: The function to set id in operations
        Return: no return
        ---------------------------------------------------------------------------*/
        void Set_Id(uint8_t i) {
            assert(i <= 0xf && i >= 0);
            id = i;
        }

        /*---------------------------------------------------------------------------
        Name: Class print
        Purpose: The function to print all operations
        Return: no return
        ---------------------------------------------------------------------------*/
        void print() {
            cout << hex << setw(1) << setfill('0') <<
                 "|" << (unsigned int) id <<
                 "  |  |" << (unsigned int) V <<
                 "  |  |" << (unsigned int) P <<
                 "    |  |" << (unsigned int) R <<
                 "      |  |" << (unsigned int) M <<
                 "      |  |" <<
                 hex << setw(2) << setfill('0') << (unsigned int) frame <<
                 " |" <<
                 endl;
        }
    };

    vector<_page_table_item> Items;
public:

    /*---------------------------------------------------------------------------
    Name: Class Page_Table
    Purpose: The function that Page_Table in operations
    Return: no return
    ---------------------------------------------------------------------------*/
    Page_Table() {
        Items.resize(16);
        uint8_t i = 0;
        auto &&func = [&i](_page_table_item &e) { e.Set_Id(i++); };
        for_each(begin(Items), end(Items), func);
    }

    _page_table_item &Get_Item(int i) {
        return Items[i];
    }

    /*---------------------------------------------------------------------------
    Name: Class Print
    Purpose: The function to print all Table
    Return: no return
    ---------------------------------------------------------------------------*/
    void Print() {
        cout << endl << "-------------------Page Table---------------------" << endl;
        cout << "Index  Valid  Present  Referenced  Modified  Frame" << endl;
        auto &&func = [](_page_table_item &e) { e.print(); };
        for_each(begin(Items), end(Items), func);
        cout << "--------------------------------------------------" << endl << endl;
    }


};

/*---------------------------------------------------------------------------
Name: Simulation
Purpose: Simulation class, which contains all simulated operations
Return: class no return
---------------------------------------------------------------------------*/
class Simulation {

    /*---------------------------------------------------------------------------
    Name: initializes function
    Purpose: The function that initializes all classes that need to be called is public.
    Return: no return
    ---------------------------------------------------------------------------*/
    const int RAM_SIZE = 1048576;
    const int FRAME_SIZE = 4096;

    bool debug_flag;
    Page_Table PT;
    int total_memory_references;  //a) total number of memory references
    int total_read_operations;    //b) total number of read operations
    int total_write_operations;   //c) total number of write operations

    int Frames;


public:

    Simulation() : debug_flag(false), total_memory_references(0), total_read_operations(0),
                   total_write_operations(0), Frames(RAM_SIZE / FRAME_SIZE) {};

    /*---------------------------------------------------------------------------
    Name: After function
    Purpose: For After to output
    Return: return output
    ---------------------------------------------------------------------------*/
    void After() {
        cout << endl << "      All Simulations End!!!";
        PT.Print();
        cout << endl << "      The summary information    " << endl <<
             " total number of memory references: " << total_memory_references << endl <<
             " total number of read operations:   " << total_read_operations << endl <<
             " total number of write operations:  " << total_write_operations << endl <<
             endl;
    }

    uint8_t &Valid(uint32_t index) {

        return PT.Get_Item(index).V;
    }

    uint8_t &Present(uint32_t index) {
        return PT.Get_Item(index).P;
    }

    uint8_t &Referenced(uint32_t index) {

        return PT.Get_Item(index).R;
    }

    uint8_t &Modified(uint32_t index) {
        return PT.Get_Item(index).M;
    }

    uint8_t &Frame(uint32_t index) {
        return PT.Get_Item(index).frame;
    }

    void Loss(uint32_t index, uint32_t num) {
        Frame(index) = num;
    }

    /*---------------------------------------------------------------------------
    Name: Read function
    Purpose: Read content by format, and analyze by content. It's different from Read_File function.
    Return: no return
    ---------------------------------------------------------------------------*/
    void Read(uint32_t _address) {
        Valid(_address / 0x1000);
        Present(_address / 0x1000);
        Referenced(_address / 0x1000);
        Modified(_address / 0x1000);
        Frame(_address / 0x1000);
        Loss(_address / 0x1000, _address / 0x1000);
    }

    /*---------------------------------------------------------------------------
    Name: Write function
    Purpose: Write files and content by format, and analyze by content
    Return: no return
    ---------------------------------------------------------------------------*/
    void Write(uint32_t _address) {
        Valid(_address);
    }

    /*---------------------------------------------------------------------------
    Name: Read_File function
    Purpose: Read files and content by format, and analyze by content
    Return: no return
    ---------------------------------------------------------------------------*/
    void Read_File(string &name, bool df) {
        debug_flag = df;
        ifstream in(name);
        if (!in.is_open()) {
            cout << "open file " << name << " fail!" << endl;
            return;
        }
        string line;
        if (debug_flag) {
            cout << "start of the simulation(" << name << ")..." << endl;
            PT.Print();
        }
        while (getline(in, line)) {
            stringstream ss;
            ss << line;
            int _address = 0;
            char R_W = 0;
            ss >> hex >> _address;
            ss >> dec >> R_W;
            total_memory_references++;
            if (R_W == 'R') {
                Read(_address);
                total_read_operations++;
            } else if (R_W == 'W') {
                Write(_address);
                total_write_operations++;
            } else {
                cout << "unknown operator(" << R_W << ")!" << endl;
                continue;
            }
            cout << hex << setw(4) << setfill('0') << _address << " "
                 << R_W << " " <<
                 setw(1) << setfill('0') << _address / 0x1000 << " " <<
                 setw(3) << setfill('0') << _address % 0x1000 << " " <<
                 endl;
            if (debug_flag)
                PT.Print();
        }
        in.close();

    }

};

/*---------------------------------------------------------------------------
Name: main function
Purpose: Show all code and main function to run
Return: no return
---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
    Simulation S;
    bool debug_flag = false;
    for (int i = 1; i < argc; ++i) {
        string s(argv[i]);
        if (s == "-debug") {
            debug_flag = true;
        }
    }

    for (int i = 1; i < argc; ++i) {
        string s(argv[i]);
        if (s[0] != '-') {
            //---------read file----------
            S.Read_File(s, debug_flag);
        }

    }

    S.After();

    return 0;
}

