#include "PHPRefactoring.h"

#include "PHP/PHPEntityClass.h"
#include "PHP/PHPEntityVariable.h"
#include "PHP/PHPSourceFile.h"
#include "ieditor.h"
#include "php_code_completion.h"

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
