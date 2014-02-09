#include "clcommandlineparser.h"

clCommandLineParser::clCommandLineParser(const wxString &str, size_t flags)
    : m_commandline(str)
    , m_flags(flags)
{
    DoParse();
}

clCommandLineParser::~clCommandLineParser()
{
}

#define STATE_NORMAL           0
#define STATE_IN_SINGLE_QUOTES 1
#define STATE_IN_DOUBLE_QUOTES 2

#define PUSH_TOKEN() \
    if ( !curtoken.IsEmpty() ) {    \
        m_tokens.Add( curtoken );   \
    }                               \
    curtoken.Clear();

#define HANDLE_NEWLINE() \
    if ( !(m_flags & kIgnoreNewLines) ) {   \
        curtoken << ch;                     \
    }
    
void clCommandLineParser::DoParse()
{
    wxString tmpstr = m_commandline;
    wxString curtoken;
    
    int state = STATE_NORMAL;
    for(size_t i=0; i<tmpstr.length(); ++i) {
        wxChar ch = tmpstr.at(i);
        switch(state) {
        case STATE_NORMAL: {
            switch( ch ) {
            case '\n':
            case '\r':
                HANDLE_NEWLINE();
                break;
            case ' ':
            case '\t':
                // whitespace, start new token
                PUSH_TOKEN();
                break;
            case '"':
                PUSH_TOKEN();
                state = STATE_IN_DOUBLE_QUOTES;
                break;
            case '\'':
                PUSH_TOKEN();
                state = STATE_IN_SINGLE_QUOTES;
                break;
            default: {
                // Normal
                curtoken << ch;
                break;
            }
            }
            break;
        }
        case STATE_IN_DOUBLE_QUOTES:
            if ( ch == '"' ) {
                PUSH_TOKEN();
                state = STATE_NORMAL;
            } else {
                curtoken << ch;
            }
            break;
        case STATE_IN_SINGLE_QUOTES:
            if ( ch == '\'' ) {
                PUSH_TOKEN();
                state = STATE_NORMAL;
            } else {
                curtoken << ch;
            }
            break;
        }
    }
    PUSH_TOKEN();
}
