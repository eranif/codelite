#ifndef PHPEVENT_H
#define PHPEVENT_H

#include "cl_command_event.h" // Base class: clCommandEvent

class PHPEvent : public clCommandEvent
{
    wxString      m_oldFilename;
    wxArrayString m_fileList;
    wxString      m_url;
    bool          m_useDefaultBrowser;
    int           m_lineNumber;

public:
    PHPEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
    PHPEvent(const PHPEvent& src);
    PHPEvent& operator=(const PHPEvent& src);
    virtual ~PHPEvent();

    virtual wxEvent *Clone() const;
    void SetLineNumber(int lineNumber) {
        this->m_lineNumber = lineNumber;
    }
    int GetLineNumber() const {
        return m_lineNumber;
    }
    void SetFileList(const wxArrayString& fileList) {
        this->m_fileList = fileList;
    }
    const wxArrayString& getFileList() const {
        return m_fileList;
    }
    
    void SetOldFilename(const wxString& oldFilename) {
        this->m_oldFilename = oldFilename;
    }
    
    const wxString& GetOldFilename() const {
        return m_oldFilename;
    }
    void SetUrl(const wxString& url) {
        this->m_url = url;
    }
    const wxString& GetUrl() const {
        return m_url;
    }
    void SetUseDefaultBrowser(bool useDefaultBrowser) {
        this->m_useDefaultBrowser = useDefaultBrowser;
    }
    bool IsUseDefaultBrowser() const {
        return m_useDefaultBrowser;
    }
};

typedef void (wxEvtHandler::*PHPEventFunction)(PHPEvent&);
#define PHPEventHandler(func) \
    wxEVENT_HANDLER_CAST(PHPEventFunction, func)

wxDECLARE_EVENT(wxEVT_PHP_FILE_RENAMED, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_FILES_REMOVED, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_FILES_ADDED, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_WORKSPACE_RENAMED, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_WORKSPACE_CLOSED, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_WORKSPACE_LOADED, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_LOAD_URL, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_STACK_TRACE_ITEM_ACTIVATED, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_DELETE_BREAKPOINT, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_DELETE_ALL_BREAKPOINTS, PHPEvent);
wxDECLARE_EVENT(wxEVT_PHP_BREAKPOINT_ITEM_ACTIVATED, PHPEvent);

#endif // PHPEVENT_H
