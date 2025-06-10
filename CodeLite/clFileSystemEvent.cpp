#include "clFileSystemEvent.h"

clFileSystemEvent::clFileSystemEvent(wxEventType commandType, int winid)
    : clCommandEvent(commandType, winid)
{
}
