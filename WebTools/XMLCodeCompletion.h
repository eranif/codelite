//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : XMLCodeCompletion.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef XMLCODECOMPLETION_H
#define XMLCODECOMPLETION_H

#include "smart_ptr.h"
#include <wx/event.h>
#include <vector>
#include "cl_command_event.h"
#include "macros.h"
#include "ServiceProvider.h"

class WebTools;
class IEditor;
class XMLCodeCompletion : public ServiceProvider
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
    WebTools* m_plugin = nullptr;
    
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
    void OnCodeComplete(clCodeCompletionEvent& event);

public:
    XMLCodeCompletion(WebTools* plugin);
    virtual ~XMLCodeCompletion();

    void XmlCodeComplete(IEditor* editor);
    void HtmlCodeComplete(IEditor* editor);

    void Reload();
};

#endif // XMLCODECOMPLETION_H
