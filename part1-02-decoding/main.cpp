#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <utility>
typedef uint8_t uchar;
typedef uint16_t ushort;

using std::cout;
using std::endl;
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

    const uchar movReg   = 0b100010;
    const uchar immToReg = 0b1011;
    const uchar regFromRegMem = 0b100010;
    std::string opcode = "mov";
    const char* registers[2][8] = { {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"}, // w = 0
                                    {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di" } }; // w = 1

    int i=0,iCount=bytes.size();
    for (i=0 ; i<iCount ;  )
    {
        std::ostringstream displace("");
        std::string offset;
        // debug
        uchar b = bytes[i++];
        if ((b >> 4) == immToReg)
        {
            uchar w = (b>>3) & 0b1;
            uchar reg = b & 0b111;
            uchar data = bytes[i++];
            uchar data1 = 0;
            if (w) { data1 = bytes[i++]; }
            ushort dataW = ushort(data1)<<8 | ushort(data);

            cout << opcode << " " << registers[w][reg] << ", " << dataW << endl;
        }
        else if ( (b >> 2) == movReg)
        {
            uchar d = b & 0b10;
            uchar w = b & 0b1;
            uchar arg = bytes[i++];

            uchar mod = arg >> 6; // mod 2 bits, value of 0b11 means Register Mode
            uchar reg = (arg >> 3) & 0b111;
            uchar rm = arg & 0b111;
            auto fromReg = "";
            auto toReg = "";
            offset = "";
            ushort dlo = 0;
            ushort dw = 0;
            switch (rm) {
                case 0b000: offset = "[bx + si]"; break;
                case 0b001: offset = "[bx + di]"; break;
                case 0b010: offset = "[bp + si]"; break;
                case 0b011: offset = "[bp + di]"; break;
                case 0b100: offset = "[si]"; break;
                case 0b101: offset = "[di]"; break;
                case 0b110: offset = "[bp]"; break;
                case 0b111: offset = "[bx]"; break;
            }
            fromReg = registers[w][rm];
            toReg = registers[w][reg];
            switch (mod)
            {
                case 0b00: {
                    if (rm == 0b110)
                    {
                        dlo = bytes[i++];
                        dw = static_cast<ushort>(bytes[i++]) << 8 | static_cast<ushort>(dlo);
                        displace << dw;
                        displace.flush();
                        offset = displace.str();
                    }
                }
                break;
                case 0b01: {
                    dlo = bytes[i++];
                    offset = offset.substr(0, offset.find_first_of("]"));
                    displace << offset;
                    if (dlo) {
                        displace << " + " << dlo;
                    }
                    displace << "]";
                    displace.flush();
                    offset = displace.str();
                }
                break;
                case 0b10: {
                    dlo = bytes[i++];
                    dw = static_cast<ushort>(bytes[i++]) << 8 | static_cast<ushort>(dlo);
                    offset = offset.substr(0, offset.find_first_of("]"));
                    displace << offset;
                    if (dw) {
                        displace << " + " << dw;
                    }
                    displace << "]";
                    displace.flush();
                    offset = displace.str();
                }
                break;
                case 0b11: {
                    fromReg = registers[w][rm];
                    toReg = registers[w][reg];
                    offset = fromReg;
                }
                break;
                default:
                    break; //std::unreachable();
            }
            if (d) {
                cout << opcode << " " << toReg << ", " << offset << endl;
            } else {
                cout << opcode << " " << offset << ", " << toReg << endl;
            }
        }
        else if ( (b >> 2) == regFromRegMem)
        {
            uchar d = b & 0b10;
            uchar w = b & 0b1;
            int x = 1;
        }
        else
        {
            cout << "ERROR at position: " << i << endl;
            return 1;
        }
    }
}
