#include "StringUtils.h"

#include <vector>

std::string StringUtils::ToStdString(const wxString& str)
{
    const char* data = str.mb_str(wxConvUTF8).data();
    if(!data) {
        data = str.To8BitData();
    }

    std::string res;
    if(!data) {
        return res;
    }
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
        if(modbuffer.IsEmpty()) {
            modbuffer = wxString::From8BitData(output.c_str());
        }
    } else {
        modbuffer.Clear();
    }
}

void StringUtils::DisableMarkdownStyling(wxString& buffer)
{
    buffer.Replace("\\", "\\\\");
    buffer.Replace("#", "\\#");
    buffer.Replace("-", "\\-");
    buffer.Replace("=", "\\=");
    buffer.Replace("*", "\\*");
    buffer.Replace("~", "\\~");
    buffer.Replace("`", "\\`");
}

#define ARGV_STATE_NORMAL 0
#define ARGV_STATE_DQUOTE 1
#define ARGV_STATE_SQUOTE 2
#define ARGV_STATE_ESCAPE 3
#define ARGV_STATE_BACKTICK 4 // `
#define ARGV_STATE_DOLLAR 5   //$
#define ARGV_STATE_PAREN 6    // (

#define PUSH_CURTOKEN()          \
    {                            \
        if(!curstr.empty()) {    \
            A.push_back(curstr); \
            curstr.clear();      \
        }                        \
    }

namespace
{
int get_current_state(const std::vector<int>& states)
{
    if(states.empty()) {
        return ARGV_STATE_NORMAL;
    }
    return states[0];
}

int get_prev_state(const std::vector<int>& states)
{
    if(states.size() < 2) {
        return ARGV_STATE_NORMAL;
    }
    return states[1];
}

void push_state(std::vector<int>& states, int state) { states.insert(states.begin(), state); }
void pop_state(std::vector<int>& states) { states.erase(states.begin()); }
} // namespace

char** StringUtils::BuildArgv(const wxString& str, int& argc)
{
    std::vector<wxString> A;
    int dollar_paren_depth = 0;
    std::vector<int> states = { ARGV_STATE_NORMAL };
    wxString curstr;
    for(wxChar ch : str) {
        switch(get_current_state(states)) {
        case ARGV_STATE_NORMAL: {
            switch(ch) {
            case '$':
                curstr << ch;
                push_state(states, ARGV_STATE_DOLLAR);
                break;
            case ' ':
            case '\t':
            case ';':
                PUSH_CURTOKEN();
                break;
            case '\'':
                push_state(states, ARGV_STATE_SQUOTE);
                curstr << ch;
                break;
            case '"':
                push_state(states, ARGV_STATE_DQUOTE);
                curstr << ch;
                break;
            case '`':
                push_state(states, ARGV_STATE_BACKTICK);
                curstr << ch;
                break;
            case '\\':
                push_state(states, ARGV_STATE_ESCAPE);
                curstr << ch;
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        case ARGV_STATE_DOLLAR: {
            switch(ch) {
            case '(':
                curstr << ch;
                push_state(states, ARGV_STATE_PAREN);
                dollar_paren_depth++; // makes it 1
                break;
            case ';':
            case ' ':
            case '\t':
                PUSH_CURTOKEN();
                pop_state(states);
                break;
            default:
                curstr << ch;
                pop_state(states);
                break;
            }
        } break;
        case ARGV_STATE_PAREN: {
            switch(ch) {
            case '(':
                curstr << ch;
                dollar_paren_depth++; // increase teh depth
                break;
            case ')':
                curstr << ch;
                dollar_paren_depth--; // reduce the depth
                // if the depth reached 0, we should leave this state
                if(dollar_paren_depth == 0) {
                    // leave the state
                    pop_state(states);
                }
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        case ARGV_STATE_ESCAPE: {
            if(get_prev_state(states) == ARGV_STATE_NORMAL) {
                curstr << ch;
                pop_state(states);
                break;
            } else if(get_prev_state(states) == ARGV_STATE_DQUOTE) {
                switch(ch) {
                case '"':
                    curstr << "\"";
                    pop_state(states);
                    break;
                default:
                    curstr << "\\" << ch;
                    pop_state(states);
                    break;
                }
            } else if(get_prev_state(states) == ARGV_STATE_BACKTICK) {
                switch(ch) {
                case '`':
                    curstr << "`";
                    pop_state(states);
                    break;
                default:
                    curstr << "\\" << ch;
                    pop_state(states);
                    break;
                }
            } else { // single quote
                switch(ch) {
                case '\'':
                    curstr << "'";
                    pop_state(states);
                    break;
                default:
                    curstr << "\\" << ch;
                    pop_state(states);
                    break;
                }
            }
        } break;
        case ARGV_STATE_DQUOTE: {
            switch(ch) {
            case '\\':
                push_state(states, ARGV_STATE_ESCAPE);
                break;
            case '"':
                curstr << ch;
                pop_state(states);
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        case ARGV_STATE_SQUOTE: {
            switch(ch) {
            case '\\':
                push_state(states, ARGV_STATE_ESCAPE);
                break;
            case '\'':
                curstr << ch;
                pop_state(states);
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        case ARGV_STATE_BACKTICK: {
            switch(ch) {
            case '\\':
                push_state(states, ARGV_STATE_ESCAPE);
                break;
            case '`':
                curstr << ch;
                pop_state(states);
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        }
    }

    if(!curstr.IsEmpty()) {
        A.push_back(curstr);
    }

    if(A.empty()) {
        return nullptr;
    }

    char** argv = new char*[A.size() + 1];
    argv[A.size()] = NULL;
    for(size_t i = 0; i < A.size(); ++i) {
        argv[i] = strdup(A[i].mb_str(wxConvUTF8).data());
    }
    argc = (int)A.size();
    return argv;
}

void StringUtils::FreeArgv(char** argv, int argc)
{
    for(int i = 0; i < argc; ++i) {
        free(argv[i]);
    }
    delete[] argv;
}

wxArrayString StringUtils::BuildArgv(const wxString& str)
{
    int argc = 0;
    char** argv = BuildArgv(str, argc);
    wxArrayString arrArgv;
    for(int i = 0; i < argc; ++i) {
        arrArgv.Add(argv[i]);
    }
    FreeArgv(argv, argc);

    for(wxString& s : arrArgv) {
        if((s.length() > 1) && s.StartsWith("\"") && s.EndsWith("\"")) {
            s.RemoveLast().Remove(0, 1);
        }
    }
    return arrArgv;
}
