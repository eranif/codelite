#ifndef PHPREFACTORING_H
#define PHPREFACTORING_H

#include "php_entry.h"
#include "PHPSetterGetterEntry.h"

class IEditor;
class PHPRefactoring
{
    PHPRefactoring();
protected:
    void GetMembersAndFunctions(const PHPEntry &cls, PHPEntry::Vector_t &members, PHPEntry::Vector_t &functions) const;
    bool FindByName(const PHPEntry::Vector_t& entries, const wxString &name) const;
    
public:
    virtual ~PHPRefactoring();
    static PHPRefactoring& Get();
    
    /**
     * @brief return list of setters for 'cls' at the current editor position
     */
    PHPSetterGetterEntry::Vec_t GetSetters(IEditor* editor) const;
    
    /**
     * @brief return list of getters for 'cls' at the current editor position
     */
    PHPSetterGetterEntry::Vec_t GetGetters(IEditor* editor) const;
};

#endif // PHPREFACTORING_H
