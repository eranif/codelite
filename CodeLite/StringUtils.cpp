#include "StringUtils.h"

std::string StringUtils::ToStdString(const wxString& str)
{
    wxCharBuffer cb = str.ToAscii();
    const char* data = cb.data();
    if(!data) { data = str.mb_str(wxConvUTF8).data(); }
    if(!data) { data = str.To8BitData(); }

    std::string res;
    if(!data) { return res; }
    res = data;
    return res;
}

#define BUFF_STATE_NORMAL 0
#define BUFF_STATE_IN_ESC 1
#define BUFF_STATE_IN_OSC 2

// see : https://en.wikipedia.org/wiki/ANSI_escape_code#Escape_sequences
void StringUtils::StripTerminalColouring(const std::string& buffer, std::string& modbuffer)
{
    modbuffer.reserve(buffer.length());
    short state = BUFF_STATE_NORMAL;
    for(const char& ch : buffer) {
        switch(state) {
        case BUFF_STATE_NORMAL:
            if(ch == 0x1B) { // found ESC char
                state = BUFF_STATE_IN_ESC;
            } else {
                modbuffer += ch;
            }
            break;
        case BUFF_STATE_IN_ESC:
            switch(ch) {
            case 'm':
            case 'K':
            case 'G':
            case 'J':
            case 'H':
            case 'X':
            case 'B':
            case 'C':
            case 'D':
            case 'd':
                state = BUFF_STATE_NORMAL;
                break;
            case ']':
                // Operating System Command
                state = BUFF_STATE_IN_OSC;
                break;
            default:
                break;
            }
            break;
        case BUFF_STATE_IN_OSC:
            if(ch == '\a') {
                // bell, leave the current state
                state = BUFF_STATE_NORMAL;
            }
            break;
        }
    }
    modbuffer.shrink_to_fit();
}

void StringUtils::StripTerminalColouring(const wxString& buffer, wxString& modbuffer)
{
    std::string source = ToStdString(buffer);
    std::string output;
    StripTerminalColouring(source, output);
    if(!output.empty()) {
        modbuffer = wxString(output.c_str(), wxConvUTF8);
        if(modbuffer.IsEmpty()) { modbuffer = wxString::From8BitData(output.c_str()); }
    } else {
        modbuffer.Clear();
    }
}
