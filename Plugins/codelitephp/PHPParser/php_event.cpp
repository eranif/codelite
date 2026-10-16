#include "php_event.h"

wxDEFINE_EVENT(wxEVT_PHP_LOAD_URL, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_STACK_TRACE_ITEM_ACTIVATED, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_BREAKPOINT_ITEM_ACTIVATED, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_DELETE_BREAKPOINT, PHPEvent);
wxDEFINE_EVENT(wxEVT_PHP_DELETE_ALL_BREAKPOINTS, PHPEvent);

PHPEvent::PHPEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}

wxEvent* PHPEvent::Clone() const { return new PHPEvent(*this); }
