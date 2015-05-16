#ifndef GITCOMMANDPUSH_H
#define GITCOMMANDPUSH_H

#include "GitCommand.h"

class GitCommandPush : public GitCommand
{
public:
    virtual void ProcessOutput(const wxString& text);
    virtual void ProcessTerminated();
    GitCommandPush();
    virtual ~GitCommandPush();
};

#endif // GITPUSH_H
