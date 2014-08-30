#include "php_event.h"

wxDEFINE_EVENT(wxEVT_PHP_FILE_RENAMED, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_FILES_REMOVED, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_FILES_ADDED, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_WORKSPACE_RENAMED, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_LOAD_URL, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_STACK_TRACE_ITEM_ACTIVATED, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_BREAKPOINT_ITEM_ACTIVATED, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_DELETE_BREAKPOINT, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_DELETE_ALL_BREAKPOINTS, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_WORKSPACE_CLOSED, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_WORKSPACE_LOADED, PHPEvent);

PHPEvent::PHPEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
    , m_useDefaultBrowser(false)
    , m_lineNumber(-1)
{
}

PHPEvent::PHPEvent(const PHPEvent& src)
    : clCommandEvent(src)
{
    *this = src;
}

PHPEvent::~PHPEvent()
{
}

wxEvent* PHPEvent::Clone() const
{
    return new PHPEvent(*this);
}

PHPEvent& PHPEvent::operator=(const PHPEvent& src)
{
    // Call parent operator =
    clCommandEvent::operator=(src);
    m_oldFilename = src.m_oldFilename;
    m_fileList = src.m_fileList;
    m_url = src.m_url;
    m_useDefaultBrowser = src.m_useDefaultBrowser;
    m_lineNumber = src.m_lineNumber;
    return *this;
}
