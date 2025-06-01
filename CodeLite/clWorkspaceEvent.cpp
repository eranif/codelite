#include "clWorkspaceEvent.hpp"

clWorkspaceEvent::clWorkspaceEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}
