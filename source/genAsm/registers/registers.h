#pragma once

std::string genAsm::selectReg(const char *reg, int size)
{
    std::stringstream ss;
    switch (size)
    {
    case 8:
        ss << reg; 
        break;

    case 4:
        ss << 'e' << reg[1] << reg[2];
        break;

    case 2:
        ss << reg[1] << "i";
        break;

    case 1:
        if(reg[2] == 'x'){
            ss << reg[1] << 'l';
        }else if(reg[2] == 'i'){
            ss << reg[1] << reg[2] << 'l';
        }
        break;

    default:
        return "";
    }

    return ss.str();
}

std::string genAsm::selectWord(int size)
{
    switch (size)
    {
    case 8:
        return "QWORD";

    case 4:
        return "DWORD";

    case 2:
        return "WORD";
    
    case 1:
        return "BYTE";

    default:
        return "";
    }
}