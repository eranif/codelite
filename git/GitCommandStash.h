#ifndef GITCOMMANDSTASH_H
#define GITCOMMANDSTASH_H

#include "GitCommand.h"

class GitCommandStash : public GitCommand
{
public:
    virtual void ProcessOutput(const wxString& text);
    virtual void ProcessTerminated();
    GitCommandStash();
    virtual ~GitCommandStash();
};

#endif // GITCOMMANDSTASH_H
