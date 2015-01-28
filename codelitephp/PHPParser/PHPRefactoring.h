#ifndef PHPREFACTORING_H
#define PHPREFACTORING_H

#include "PHPSetterGetterEntry.h"
#include "PHPEntityBase.h"

class IEditor;
class PHPRefactoring
{
    PHPRefactoring();
protected:
    bool FindByName(const PHPEntityBase::List_t& entries, const wxString &name) const;
    
public:
    virtual ~PHPRefactoring();
    static PHPRefactoring& Get();
};

#endif // PHPREFACTORING_H
