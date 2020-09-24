/****************************************************************************
Tianrui Liu
Computer Project #10
****************************************************************************/
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <list>
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

    // int Frames;
    vector<uint32_t> Free_frames_list;
    vector<uint8_t> Memory;
    vector<uint8_t> Disk;
    list<uint32_t> LRU;


public:

    Simulation() : debug_flag(false), total_memory_references(0), total_read_operations(0),
               total_write_operations(0) {

        Memory.assign(RAM_SIZE, 0);
        Disk.assign(RAM_SIZE, 0);
        Config();
//        for(auto&e : Free_frames_list)
//            cout<<hex<<e<<endl;
    };

    /*---------------------------------------------------------------------------
    Name: Config function
    Purpose: Config to output and input the Config file
    Return: no return
    ---------------------------------------------------------------------------*/
    void Config() {
        string name = "config";
        ifstream in(name);
        if (!in.is_open()) {
            cout << "open file " << name << " fail!" << endl;
            return;
        }

        string line;
        stringstream ss;
        //-------------------1------------
        getline(in, line);

        ss << line;
        string lru;
        ss >> lru;
        if (lru != "LRU") {
            cout << "config file first line error!!!" << endl;
            abort();
        }
        //-------------------2------------

        getline(in, line);

        ss.clear();
        ss << line;
        int m = 0;
        ss >> dec >> m;
        while (m--) {
            int v = 0;
            ss >> hex >> v;
            if (v > 0xff) {
                cout << "config file second line error!!!" << endl;
                abort();
            }
            Free_frames_list.push_back(v);
        }
        //-------------------3------------

        getline(in, line);

        ss.clear();
        ss << line;
        int n = 0;
        ss >> dec >> n;
        while (n--) {
            int v = 0;
            ss >> hex >> v;
            if (v > 0xf) {
                cout << "config file thrid line error!!!" << endl;
                abort();
            }
            Valid(v) = 1;
//            cout << "bb "<<hex << v << endl;
        }


    }

    /*---------------------------------------------------------------------------
    Name: After function
    Purpose: For After to output
    Return: return output
    ---------------------------------------------------------------------------*/
    void After() {
        cout << endl << "      All Simulations End!!!";
        PT.Print();
        cout << endl << "      The summary information    " << endl << dec <<
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

    /*---------------------------------------------------------------------------
    Name: Loss function
    Purpose: Loss to output
    Return: no return
    ---------------------------------------------------------------------------*/
    void Loss(uint32_t index) {

        if (Free_frames_list.empty()) {
            uint32_t victim = LRU.back();
            LRU.pop_back();
            //select a page which is currently resident in RAM as the victim
            if (Modified(victim) == 1) {
                for (int i = 0; i < FRAME_SIZE; ++i) {
                    Disk[Frame(victim) * FRAME_SIZE + i] = Memory[Frame(victim) * FRAME_SIZE + i];
                }
            }
            Present(victim) = 0;
            Free_frames_list.push_back(Frame(victim));
        }
        uint32_t new_frame = Free_frames_list[0];
//        uint32_t new_frame = Free_frames_list[Free_frames_list.size() / 2];
//        Free_frames_list.erase(Free_frames_list.begin() + Free_frames_list.size() / 2);
        Free_frames_list.erase(Free_frames_list.begin());
        Frame(index) = new_frame;
        Present(index) = 1;
        Referenced(index) = 0;
        Modified(index) = 0;
        for (int i = 0; i < FRAME_SIZE; ++i) {
            Memory[Frame(index) * FRAME_SIZE + i] = Disk[Frame(index) * FRAME_SIZE + i];
        }

    }

    /*---------------------------------------------------------------------------
    Name: MMU function
    Purpose: MMU to output
    Return: no return
    ---------------------------------------------------------------------------*/
    void MMU(uint32_t _address, bool R_W) {

        uint32_t index = _address / 0x1000;
        uint32_t offset = _address % 0x1000;

        cout << hex << setw(4) << setfill('0') << _address << " "
             << (R_W ? "R" : "W") << " " <<
             setw(1) << setfill('0') << index << " " <<
             setw(3) << setfill('0') << offset << " ";
        if (Valid(index) == 0) {
            cout << "       fatal error ! page number is not valid" << endl;
            return;
        } else {

            if (Present(index) == 0) {
                cout << "F ";
                Loss(index);
            } else
                cout << "  ";
            if (R_W) //read
            {
                Referenced(index) = 1;
                cout << "  ";
            } else {
                Referenced(index) = 1;
                Modified(index) = 1;
                cout << "B ";
            }

            LRU.remove(index);
            LRU.push_front(index);
            if(LRU.size() > 0xf)
                LRU.pop_back();

            cout << hex << setw(5) << setfill('0') << Frame(index) * 0x1000 + offset << endl;

        }
//        for (auto &e : Free_frames_list)
//            cout << hex << e << endl;
//        cout << endl;

    }

    /*---------------------------------------------------------------------------
    Name: Read function
    Purpose: Read content by format, and analyze by content. It's different from Read_File function.
    Return: no return
    ---------------------------------------------------------------------------*/
    void Read(uint32_t _address) {
        MMU(_address, true);

    }

    /*---------------------------------------------------------------------------
    Name: Write function
    Purpose: Write files and content by format, and analyze by content
    Return: no return
    ---------------------------------------------------------------------------*/
    void Write(uint32_t _address) {
        MMU(_address, false);
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
