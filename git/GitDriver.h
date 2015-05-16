#ifndef GITDRIVER_H
#define GITDRIVER_H

#include <wx/event.h>
#include "GitCommand.h"

class GitDriver : public wxEvtHandler
{
public:
    GitDriver();
    virtual ~GitDriver();
    
    /**
     * @brief execute a git command
     * @param command
     */
    void Process(GitCommand::Ptr_t command);
};

#endif // GITDRIVER_H
