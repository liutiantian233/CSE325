/****************************************************************************
Tianrui Liu
Computer Project #7
****************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <semaphore.h>
#include <algorithm>
#include <cassert>
#include <vector>
#include <iomanip>

using namespace std;

const uint32_t MAX_Mem_Length = 1048576;
const uint32_t Cache_Data_Length = 16;
const uint32_t MAX_Cache_Length = 8;

/*---------------------------------------------------------------------------
Name: Simulation
Purpose: Simulation class, which contains all simulated operations
Return: class no return
---------------------------------------------------------------------------*/
class Simulation {

    union _type {
        uint32_t I_32;
        uint8_t I_8[4];
        // I32 :  H .................. L
        // I8    I8[0] I8[1] I8[2] I8[3]
    };

    /*---------------------------------------------------------------------------
    Name: Class Cache_Page
    Purpose: The function that initializes all Cache_Page classes that need to be called is public.
    Return: no return
    ---------------------------------------------------------------------------*/
    class Cache_Page {
        bool _valid, _modify;
        uint16_t _tag_bits;
        vector<_type> _data;

    public:
        Cache_Page() {
            _valid = false;
            _modify = false;
            _tag_bits = 0;
            _data.resize(4);
        }

        /*---------------------------------------------------------------------------
        Name: print function
        Purpose: print output for Cache_Page class
        Return: return output
        ---------------------------------------------------------------------------*/
        void print() {
            cout << hex << "valid: " << _valid << " modify: " << _modify
                 << "  tag: " << setw(4) << setfill('0') << _tag_bits << " data: ";
            for (_type &e:_data)
                cout << hex << setw(8) << setfill('0') << e.I_32 << " ";
            cout << endl;
        }

        uint16_t _Get_Tag() const {
            return _tag_bits;
        }

        void _Set_Tag(const uint16_t _in) {
            _tag_bits = _in;
        }

        bool _Get_Valid() const {
            return _valid;
        }

        void _Set_Valid(const bool _in) {
            _valid = _in;
        }

        bool _Get_Modify() const {
            return _modify;
        }

        void _Set_Modify(const bool _in) {
            _modify = _in;
        }


        uint32_t _Get_Data(uint8_t offset) const {
            return _data[offset / 4].I_32;
        }

        void _Get_Data(vector<_type> &_out) const {
            assert(_out.size() == 4);
            for (unsigned int i = 0; i < _out.size(); ++i)
                _out[i].I_32 = _data[i].I_32;
        }

        void _Set_Data(const uint32_t _in, uint8_t offset) {
            assert(offset < 0xf);
            assert(offset % 4 == 0);
            _data[offset / 4].I_32 = _in;
        }

        void _Set_Data(vector<_type> &_in) {
            assert(_in.size() == 4);
            for (unsigned int i = 0; i < _in.size(); ++i)
                _Set_Data(_in[i].I_32, i * 4);
        }
    };

    vector<Cache_Page> _Lines;
    vector<uint8_t> _Storage;
    bool debug_flag = false;

/*---------------------------------------------------------------------------
Name: initializes function
Purpose: The function that initializes all classes that need to be called is public.
Return: no return
---------------------------------------------------------------------------*/
public:
    explicit Simulation(const bool d_f = false) : debug_flag(d_f) {
        _Lines.resize(MAX_Cache_Length);
        _Storage.resize(MAX_Mem_Length, 0);

    }

    void Write_Mem(uint16_t address, vector<_type> &data) {
        for (unsigned int i = 0; i < Cache_Data_Length; ++i)
            _Storage[i + address] = data[i / 4].I_8[3 - i % 4];

    }

    void Read_Mem(uint16_t address, vector<_type> &data) {
        for (unsigned int i = 0; i < Cache_Data_Length; ++i)
            data[i / 4].I_8[3 - i % 4] = _Storage[i + address];

    }


//    void Test() {
//
//        vector<_type> A(4);
//        A[0].I_8[0] = 0x12;
//        A[0].I_8[1] = 0x34;
//        A[0].I_8[2] = 0x56;
//        A[0].I_8[3] = 0x78;
//        for_each(A.begin(), A.end(),
//                 [](_type &e) { cout << hex << setw(8) << setfill('0') << e.I_32 << "  "; });
//
//    }

    /*---------------------------------------------------------------------------
    Name: Read function
    Purpose: Read content by format, and analyze by content. It's different from Read_File function.
    Return: no return
    ---------------------------------------------------------------------------*/
    uint32_t Read(const uint32_t address) {
        assert(address <= MAX_Mem_Length);
        assert(address % 4 == 0);

        uint8_t offset = address % Cache_Data_Length;
        uint16_t truncation = address / Cache_Data_Length;
        uint8_t index = truncation % MAX_Cache_Length;
        uint16_t tags = truncation / MAX_Cache_Length;
        cout << hex << setw(5) << setfill('0') << address << " R " << setw(4) << setfill('0') << tags << " " <<
             (int) index << " " << (int) offset << endl;
        vector<_type> data(4);
        if (_Lines[index]._Get_Valid()) {
            if (tags == _Lines[index]._Get_Tag())  //hit
                return _Lines[index]._Get_Data(offset);
            else {
                if (_Lines[index]._Get_Modify()) {   //valid and modified


                    _Lines[index]._Get_Data(data);
                    Write_Mem(_Lines[index]._Get_Tag() * MAX_Cache_Length + index, data);      //write back

                }
            }
        }
//        vector<_type> cpy(4);
        Read_Mem(truncation, data);  //copy form memory
        _Lines[index]._Set_Data(data);
        _Lines[index]._Set_Valid(true);
        _Lines[index]._Set_Modify(false);
        _Lines[index]._Set_Tag(tags);
        return _Lines[index]._Get_Data(offset);

    }

    /*---------------------------------------------------------------------------
    Name: Write function
    Purpose: Write files and content by format, and analyze by content
    Return: no return
    ---------------------------------------------------------------------------*/
    void Write(const u_int32_t address, const u_int32_t data) {
        assert(address <= MAX_Mem_Length);
        assert(address % 4 == 0);

        uint8_t offset = address % Cache_Data_Length;
        uint16_t truncation = address / Cache_Data_Length;
        uint8_t index = truncation % MAX_Cache_Length;
        uint16_t tags = truncation / MAX_Cache_Length;
        cout << hex << setw(5) << setfill('0') << address << " W " << setw(4) << setfill('0') << tags << " " <<
             (int) index << " " << (int) offset << endl;
        if (_Lines[index]._Get_Valid()) {
            if (tags == _Lines[index]._Get_Tag()) {  //hit
                _Lines[index]._Set_Data(data, offset);
                _Lines[index]._Set_Modify(true);
                return;
            } else {
                if (_Lines[index]._Get_Modify()) {    //valid and modified
                    vector<_type> temp(4);
                    _Lines[index]._Get_Data(temp);
                    Write_Mem(_Lines[index]._Get_Tag() * MAX_Cache_Length + index, temp);      //write back
                }
            }
        }
        vector<_type> cpy(4);
        Read_Mem(truncation, cpy);  //copy form memory
        _Lines[index]._Set_Data(cpy);
        _Lines[index]._Set_Valid(true);
        _Lines[index]._Set_Modify(false);
        _Lines[index]._Set_Tag(tags);
        _Lines[index]._Set_Data(data, offset);
        _Lines[index]._Set_Modify(true);

    }

    /*---------------------------------------------------------------------------
    Name: Read_File function
    Purpose: Read files and content by format, and analyze by content
    Return: no return
    ---------------------------------------------------------------------------*/
    void Read_File(string &name) {
        ifstream in(name);
        if (!in.is_open()) {
            cout << "open file " << name << " fail!" << endl;
            return;
        }
        string line;
        cout << "Init Cache..." << endl;
        if (debug_flag)
            Print();

        while (getline(in, line)) {
            stringstream ss;
            ss << line;
            int _address = 0;
            char R_W = 0;
            ss >> hex >> _address;
            ss >> dec >> R_W;
            if (R_W == 'R') {
                Read(_address);

                /*
                 *  do something
                 *
                 * */

            } else if (R_W == 'W') {
                _type data{};
                uint32_t a0, a1, a2, a3;
                ss >> hex >> a0 >> a1 >> a2 >> a3;
                data.I_8[0] = a3;
                data.I_8[1] = a2;
                data.I_8[2] = a1;
                data.I_8[3] = a0;

                Write(_address, data.I_32);
            } else {
                cout << "unknown operator(" << R_W << ")!" << endl;
                continue;
            }
            if (debug_flag)
                Print();
        }
        in.close();
    }

    /*---------------------------------------------------------------------------
    Name: Print function
    Purpose: Print the output in format
    Return: return output
    ---------------------------------------------------------------------------*/
    void Print() {
        cout << "===================================Cache Pages=================================="
             << endl;
        int i = 0;
        for_each(_Lines.begin(), _Lines.end(), [&i](Cache_Page &e) {
            cout << "page: " << i++ << " ";
            e.print();
        });
        cout << "--------------------------------------------------------------------------------"
             << endl << endl;
    }
};

/*---------------------------------------------------------------------------
Name: main function
Purpose: Show all code and main function to run
Return: no return
---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {

    bool debug_flag = false;
    string name;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            string s(argv[i]);
            transform(s.begin(), s.end(), s.begin(), ::tolower);
            if (s == "-debug") {
                debug_flag = true;
            } else if (s == "-refs") {
                name = string(argv[++i]);
            } else {
                cout << "error argument " << s << " !" << endl;
                return -1;
            }
        } else {
            cout << "input error argument!" << endl;
            return -1;
        }
    }

    Simulation C(debug_flag);

    C.Read_File(name);

    return 0;
}

