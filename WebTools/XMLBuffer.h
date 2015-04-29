#ifndef XMLBUFFER_H
#define XMLBUFFER_H

#include <wx/string.h>
#include "XMLLexerAPI.h"
#include <vector>

class XMLBuffer
{
    wxString m_buffer;
    XMLScanner_t m_scanner;
    std::vector<wxString> m_elements;
    enum eState {
        kNormal = 0,
        kCdata,
        kComment,
    };
    eState m_state;
    
protected:
    void OnOpenTag();
    void OnTagClosePrefix();
    bool ConsumeUntil(int until);

public:
    XMLBuffer(const wxString& buffer);
    virtual ~XMLBuffer();
    void Parse();

    wxString GetCurrentScope() const;
    bool InComment() const { return m_state == kComment; }
    bool InCData() const { return m_state == kCdata; }
};

#endif // XMLBUFFER_H
