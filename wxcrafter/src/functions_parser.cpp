#include "functions_parser.h"
#include <y.tab.h>

FunctionsParser::FunctionsParser(wxcWidget::Map_t& connectedEvents, const wxString& clsname, const wxString& header)
    : m_allEvents(connectedEvents)
    , m_classname(clsname)
{
    m_scanner.SetText(header.mb_str(wxConvUTF8).data());
}

FunctionsParser::~FunctionsParser() {}

//////////////////////////////////////////////////////////
// Macros
//////////////////////////////////////////////////////////

#define YYNEXT_RET_ON_EOF()       \
    {                             \
        type = m_scanner.yylex(); \
        if(type == 0) return;     \
    }
#define INC_DEPTH_CONTINUE() \
    {                        \
        depth++;             \
        continue;            \
    }

#define DEC_DEPTH_BREAK_IF_ZERO() \
    {                             \
        depth--;                  \
        if(depth == 0) {          \
            break;                \
        } else {                  \
            continue;             \
        }                         \
    }

int FunctionsParser::ReadClassName(wxString& clsname)
{
    clsname.clear();
    int type = 0;

    while(true) {
        type = m_scanner.yylex();
        if(type == 0) return 0;

        if(type == IDENTIFIER) {
            clsname = m_scanner.YYText();

        } else if(type == '{' || type == ':') {
            return type;

        } else if(type == ';') {
            // we probably encountered a forward declaration or 'friend' statement
            clsname.Clear();
            return (int)';';
        }
    }
    return 0;
}

void FunctionsParser::Parse()
{
    int type = 0;
    int depth = 0;

    // search for the class first
    while(true) {
        YYNEXT_RET_ON_EOF();

        if(type == lexCLASS) {
            // YYNEXT_RET_ON_EOF();

            // read all tokens until we find a ':' or '{'
            wxString identifierName;
            // ReadClassName consumes all tokens from the current position
            // until it hits a ':' or '{'
            // the last IDENTIFIER consumed is the class name
            type = ReadClassName(identifierName);
            if(type == 0) return;

            if(identifierName == m_classname) { break; }
        }
    }

    if(type == '{') {
        // we already consumed the '{' char by calling to ReadClassName()
        depth = 1;

    } else {
        // search for the open curly brace
        while(true) {
            YYNEXT_RET_ON_EOF();
            if(type == '{') {
                depth = 1;
                break;
            }
        }
    }

    while(true) {
        YYNEXT_RET_ON_EOF();

        if(type == '{') {
            INC_DEPTH_CONTINUE();

        } else if(type == '}') {
            DEC_DEPTH_BREAK_IF_ZERO();

        } else if(type == lexVOID && depth == 1) {
            // Check the next type, we only check if detph == 1 (meaning we are still inside our class scope)
            YYNEXT_RET_ON_EOF();

            if(type == '{') {
                INC_DEPTH_CONTINUE();

            } else if(type == '}') {
                DEC_DEPTH_BREAK_IF_ZERO();

            } else if(type == IDENTIFIER) {
                CheckIfFunctionsExists(wxString(m_scanner.YYText(), wxConvUTF8));
            }
        }
    }
}

void FunctionsParser::CheckIfFunctionsExists(const wxString& name) { m_allEvents.erase(name); }
