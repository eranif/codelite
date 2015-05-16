#ifndef GITCOMMANDPULL_H
#define GITCOMMANDPULL_H

#include "GitCommand.h"
class GitCommandPull : public GitCommand
{
public:
    virtual void ProcessOutput(const wxString& text);
    virtual void ProcessTerminated();
    
    GitCommandPull();
    virtual ~GitCommandPull();
};

#endif // GITCOMMANDPULL_H
