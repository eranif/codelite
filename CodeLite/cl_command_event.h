#ifndef CLCOMMANDEVENT_H
#define CLCOMMANDEVENT_H

#include <wx/event.h>
#include <wx/sharedptr.h>
#include "codelite_exports.h"
#include "entry.h"
#include <wx/arrstr.h>

// Set of flags that can be passed within the 'S{G}etInt' function of clCommandEvent
enum {
    kEventImportingFolder = 0x00000001,
};

/// a wxCommandEvent that takes ownership of the clientData
class WXDLLIMPEXP_CL clCommandEvent : public wxCommandEvent
{

protected:
    wxSharedPtr<wxClientData> m_ptr;
    wxArrayString             m_strings;
    wxString                  m_fileName;
    bool                      m_answer;

public:
    clCommandEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCommandEvent(const clCommandEvent& event);
    clCommandEvent& operator=(const clCommandEvent& src);
    virtual ~clCommandEvent();

    // Hides wxCommandEvent::Set{Get}ClientObject
    void SetClientObject(wxClientData* clientObject) ;

    wxClientData *GetClientObject() const ;

    virtual wxEvent *Clone() const;

    void SetAnswer(bool answer) {
        this->m_answer = answer;
    }
    bool IsAnswer() const {
        return m_answer;
    }

    void SetFileName(const wxString& fileName) {
        this->m_fileName = fileName;
    }
    const wxString& GetFileName() const {
        return m_fileName;
    }
    void SetStrings(const wxArrayString& strings) {
        this->m_strings = strings;
    }
    const wxArrayString& GetStrings() const {
        return m_strings;
    }
};

typedef void (wxEvtHandler::*clCommandEventFunction)(clCommandEvent&);
#define clCommandEventHandler(func) \
    wxEVENT_HANDLER_CAST(clCommandEventFunction, func)

/// a clCodeCompletionEvent
class WXDLLIMPEXP_CL clCodeCompletionEvent : public clCommandEvent
{
    TagEntryPtrVector_t m_tags;
    wxObject*           m_editor;
    wxString            m_word;
    int                 m_position;
    wxString            m_tooltip;
    bool                m_insideCommentOrString;
    TagEntryPtr         m_tagEntry;

public:
    clCodeCompletionEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clCodeCompletionEvent(const clCodeCompletionEvent& event);
    clCodeCompletionEvent& operator=(const clCodeCompletionEvent& src);
    virtual ~clCodeCompletionEvent();
    virtual wxEvent *Clone() const;

    void SetTagEntry(TagEntryPtr tag) {
        this->m_tagEntry = tag;
    }

    /**
     * @brief return the tag entry associated with this event.
     * This usually makes sense for event wxEVT_CC_CODE_COMPLETE_BOX_DISMISSED
     */
    TagEntryPtr GetTagEntry() const {
        return m_tagEntry;
    }

    void SetInsideCommentOrString(bool insideCommentOrString) {
        this->m_insideCommentOrString = insideCommentOrString;
    }

    bool IsInsideCommentOrString() const {
        return m_insideCommentOrString;
    }
    void SetTags(const TagEntryPtrVector_t& tags) {
        this->m_tags = tags;
    }
    const TagEntryPtrVector_t& GetTags() const {
        return m_tags;
    }
    void SetEditor(wxObject* editor) {
        this->m_editor = editor;
    }
    /**
     * @brief return the Editor object
     */
    wxObject* GetEditor() {
        return m_editor;
    }
    void SetWord(const wxString& word) {
        this->m_word = word;
    }

    /**
     * @brief return the user typed word up to the caret position
     */
    const wxString& GetWord() const {
        return m_word;
    }
    void SetPosition(int position) {
        this->m_position = position;
    }
    /**
     * @brief return the editor position
     */
    int GetPosition() const {
        return m_position;
    }
    void SetTooltip(const wxString& tooltip) {
        this->m_tooltip = tooltip;
    }
    const wxString& GetTooltip() const {
        return m_tooltip;
    }
};

typedef void (wxEvtHandler::*clCodeCompletionEventFunction)(clCodeCompletionEvent&);
#define clCodeCompletionEventHandler(func) \
    wxEVENT_HANDLER_CAST(clCodeCompletionEventFunction, func)

class WXDLLIMPEXP_CL clColourEvent : public clCommandEvent
{
    wxColour  m_bgColour;
    wxColour  m_fgColour;
    wxColour  m_borderColour;
    wxWindow* m_page;
    bool      m_isActiveTab;

public:
    clColourEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clColourEvent(const clColourEvent& event);
    clColourEvent& operator=(const clColourEvent& src);
    virtual ~clColourEvent();
    virtual wxEvent *Clone() const {
        return new clColourEvent(*this);
    };

    void SetBorderColour(const wxColour& borderColour) {
        this->m_borderColour = borderColour;
    }
    const wxColour& GetBorderColour() const {
        return m_borderColour;
    }
    void SetPage(wxWindow* page) {
        this->m_page = page;
    }
    wxWindow* GetPage() {
        return m_page;
    }
    void SetBgColour(const wxColour& bgColour) {
        this->m_bgColour = bgColour;
    }
    void SetFgColour(const wxColour& fgColour) {
        this->m_fgColour = fgColour;
    }
    const wxColour& GetBgColour() const {
        return m_bgColour;
    }
    const wxColour& GetFgColour() const {
        return m_fgColour;
    }
    void SetIsActiveTab(bool isActiveTab) {
        this->m_isActiveTab = isActiveTab;
    }
    bool IsActiveTab() const {
        return m_isActiveTab;
    }
};

typedef void (wxEvtHandler::*clColourEventFunction)(clColourEvent&);
#define clColourEventHandler(func) \
    wxEVENT_HANDLER_CAST(clColourEventFunction, func)

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
class WXDLLIMPEXP_CL clBuildEvent : public clCommandEvent
{
    wxString m_projectName;
    wxString m_configurationName;
    wxString m_command;
    bool     m_projectOnly;

public:
    clBuildEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clBuildEvent(const clBuildEvent& event);
    clBuildEvent& operator=(const clBuildEvent& src);
    virtual ~clBuildEvent();
    virtual wxEvent *Clone() const {
        return new clBuildEvent(*this);
    };

    void SetProjectOnly(bool projectOnly) {
        this->m_projectOnly = projectOnly;
    }
    bool IsProjectOnly() const {
        return m_projectOnly;
    }
    void SetCommand(const wxString& command) {
        this->m_command = command;
    }
    const wxString& GetCommand() const {
        return m_command;
    }
    void SetConfigurationName(const wxString& configurationName) {
        this->m_configurationName = configurationName;
    }
    void SetProjectName(const wxString& projectName) {
        this->m_projectName = projectName;
    }
    const wxString& GetConfigurationName() const {
        return m_configurationName;
    }
    const wxString& GetProjectName() const {
        return m_projectName;
    }
};

typedef void (wxEvtHandler::*clBuildEventFunction)(clBuildEvent&);
#define clBuildEventHandler(func) \
    wxEVENT_HANDLER_CAST(clBuildEventFunction, func)

// -------------------------------------------------------------------------
// clDebugEvent
// -------------------------------------------------------------------------
class WXDLLIMPEXP_CL clDebugEvent : public clCommandEvent
{
    wxString m_projectName;
    wxString m_configurationName;

public:
    clDebugEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    clDebugEvent(const clDebugEvent& event);
    clDebugEvent& operator=(const clDebugEvent& src);
    
    virtual ~clDebugEvent();
    virtual wxEvent *Clone() const {
        return new clDebugEvent(*this);
    };

    void SetConfigurationName(const wxString& configurationName) {
        this->m_configurationName = configurationName;
    }
    void SetProjectName(const wxString& projectName) {
        this->m_projectName = projectName;
    }
    const wxString& GetConfigurationName() const {
        return m_configurationName;
    }
    const wxString& GetProjectName() const {
        return m_projectName;
    }
};

typedef void (wxEvtHandler::*clDebugEventFunction)(clDebugEvent&);
#define clDebugEventHandler(func) \
    wxEVENT_HANDLER_CAST(clDebugEventFunction, func)

#endif // CLCOMMANDEVENT_H
