#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
typedef unsigned char uchar;

using std::cout, std::endl;
int main(int argc, char *argv[])
{
    cout << "bits 16\n" << endl;
    if (argc != 2)
    {
        cout << "Usage: " << argv[0] << " <filename>\n" << endl;
        return 1;
    }

    std::vector<uchar> bytes;
    std::ifstream in(argv[1], std::ios::binary);
    if (in.is_open())
    {
        while (!in.eof())
        {
            bytes.push_back(in.get());
        }
        bytes.pop_back();
    }

    const uchar movReg = 0b10001000;
    std::string opcode = "mov";
    const char* registers[2][8] = { {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"},
                                  {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di" } };

    for (auto bIter = bytes.begin(); bIter != bytes.end(); )
    {
        if (uchar b = *bIter++; (b & movReg) == movReg)
        {
            uchar d = b & 0b10;
            uchar w = b & 0b1;
            uchar arg = *bIter++;

            uchar mod = arg >> 6; // mod 2 bits, value of 0b11 means Register Mode
            uchar reg = (arg >> 3) & 0b111;
            uchar regOrMem = arg & 0b111;

            auto fromReg = registers[w][reg];
            auto toReg = registers[w][regOrMem];

            cout << opcode << " " << toReg << ", " << fromReg << endl;
        }
    }


    // cout << bytes.size() << endl;
}