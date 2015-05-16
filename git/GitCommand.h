#ifndef GITCOMMAND_H
#define GITCOMMAND_H

#include "smart_ptr.h"

class GitCommand
{
public:
    GitCommand();
    virtual ~GitCommand();
    
    typedef SmartPtr<GitCommand> Ptr_t;
    
};

#endif // GITCOMMAND_H
