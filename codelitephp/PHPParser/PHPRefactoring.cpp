#include "PHPRefactoring.h"
#include "php_code_completion.h"
#include <ieditor.h>
#include "PHPEntityBase.h"
#include "PHPEntityVariable.h"
#include "PHPSourceFile.h"
#include "PHPEntityClass.h"

PHPRefactoring::PHPRefactoring() {}

PHPRefactoring::~PHPRefactoring() {}

PHPRefactoring& PHPRefactoring::Get()
{
    static PHPRefactoring theRefactorer;
    return theRefactorer;
}

bool PHPRefactoring::FindByName(const PHPEntityBase::List_t& entries, const wxString& name) const
{
    PHPEntityBase::List_t::const_iterator iter = entries.begin();
    for(; iter != entries.end(); ++iter) {
        if((*iter)->GetFullName() == name) {
            return true;
        }
    }
    return false;
}
