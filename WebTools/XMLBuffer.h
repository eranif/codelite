#ifndef XMLBUFFER_H
#define XMLBUFFER_H

#include <wx/string.h>
#include "XMLLexerAPI.h"
#include <vector>
#include "macros.h"

class XMLBuffer
{
public:
    struct Scope {
        wxString tag;
        int line;
        bool isEmptyTag;
        
        Scope()
            : line(wxNOT_FOUND)
            , isEmptyTag(false)
        {
        }
        bool IsOk() const { return line != wxNOT_FOUND && !tag.IsEmpty(); }
    };

    enum eState {
        kNormal = 0,
        kCdata,
        kComment,
    };

protected:
    wxString m_buffer;
    XMLScanner_t m_scanner;
    std::vector<XMLBuffer::Scope> m_elements;
    eState m_state;
    bool m_htmlMode;

    /// HTML empty tags
    static wxStringSet_t m_emptyTags;

protected:
    void OnOpenTag();
    void OnCloseTag();
    void OnTagClosePrefix();
    bool ConsumeUntil(int until);

public:
    XMLBuffer(const wxString& buffer, bool htmlMode = false);
    virtual ~XMLBuffer();
    void Parse();

    XMLBuffer::Scope GetCurrentScope() const;
    bool InComment() const { return m_state == kComment; }
    bool InCData() const { return m_state == kCdata; }
    /**
     * @brief return if 'tag' is an empty html tag
     */
    static bool IsEmptyHtmlTag(const wxString& tag);
};

#endif // XMLBUFFER_H
