#ifndef CSMANAGER_H
#define CSMANAGER_H

#include "codelite_events.h"
#include <wx/event.h>
#include "file_logger.h"

class csManager : public wxEvtHandler
{
public:
    csManager();
    virtual ~csManager();
};

#endif // CSMANAGER_H
