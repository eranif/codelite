#ifndef XMLCODECOMPLETION_H
#define XMLCODECOMPLETION_H

#include "smart_ptr.h"
#include <wx/event.h>
#include <vector>
#include "cl_command_event.h"
#include "macros.h"

class IEditor;
class XMLCodeCompletion : public wxEvtHandler
{
public:
    typedef SmartPtr<XMLCodeCompletion> Ptr_t;
    struct HtmlCompletion {
        wxString m_tag;
        wxString m_comment;

        HtmlCompletion(const wxString& tag, const wxString& comment)
            : m_tag(tag)
            , m_comment(comment)
        {
        }
        typedef std::vector<HtmlCompletion> Vec_t;
    };
    wxStringMap_t m_completePattern;

protected:
    enum eCompleteReason {
        kNone = -1,
        kHtmlOpenSequence = 0, // User typed "<" in an HTML document
        kCloseSequence,        // user typed "</"
    };

    HtmlCompletion::Vec_t m_htmlCompletions;
    eCompleteReason m_completeReason;
    bool m_xmlCcEnabled;
    bool m_htmlCcEnabeld;
    
protected:
    void PrepareHtmlCompletions();
    wxString GetCompletePattern(const wxString& tag) const;
    /**
     * @brief Does 'tag' has a sepcial insert pattern?
     * an example is the <a>: "<a href=""></a>"
     */
    bool HasSpecialInsertPattern(const wxString& tag) const;
    void SuggestClosingTag(IEditor* editor, bool html);
    void OnCodeCompleted(clCodeCompletionEvent& event);
    
    int GetWordStartPos(IEditor* editor);
    
public:
    XMLCodeCompletion();
    virtual ~XMLCodeCompletion();

    void XmlCodeComplete(IEditor* editor);
    void HtmlCodeComplete(IEditor* editor);
    
    void Reload();
};

#endif // XMLCODECOMPLETION_H
