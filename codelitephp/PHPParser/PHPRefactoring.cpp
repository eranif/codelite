#include "PHPRefactoring.h"
#include "php_code_completion.h"
#include <ieditor.h>
#include "php_entry.h"
#include "php_storage.h"

PHPRefactoring::PHPRefactoring()
{
}

PHPRefactoring::~PHPRefactoring()
{
}

PHPRefactoring& PHPRefactoring::Get()
{
    static PHPRefactoring theRefactorer;
    return theRefactorer;
}

PHPSetterGetterEntry::Vec_t PHPRefactoring::GetSetters(IEditor* editor) const
{
    PHPSetterGetterEntry::Vec_t setters;
    if ( !editor ) {
        return setters;
    }
    
    
    // retrieve the scope
    PHPEntry::Vector_t members, functions;
    PHPEntry scopeEntry = PHPCodeCompletion::Instance()->GetClassNearPosition(editor, editor->GetCurrentPosition());
    GetMembersAndFunctions(scopeEntry, members, functions);
    
    if ( members.empty() ) {
        return setters;
    }
    
    // Prepare list of candidates
    PHPSetterGetterEntry::Vec_t candidates;
    for(size_t i=0; i<members.size(); ++i) {
        PHPSetterGetterEntry candidate(members.at(i));
        
        // make sure we don't add setters that already exist
        if ( FindByName(functions, candidate.GetSetter(kSG_NameOnly) ) ) {
            continue;
        }
        candidates.push_back( candidate );
    }
    
    setters.swap( candidates );
    return setters;
}

void PHPRefactoring::GetMembersAndFunctions(const PHPEntry& cls, PHPEntry::Vector_t& members, PHPEntry::Vector_t& functions) const 
{
    members.clear();
    functions.clear();
    if ( cls.isOk() && cls.isClass() ) {
        PHPEntry::Vector_t children = PHPStorage::Instance()->FindEntriesByParentId(cls.getDbId(), PHPStorage::FetchFlags_Self_All);
        
        // collect members and functions
        for(size_t i=0; i<children.size(); ++i) {
            if ( children.at(i).isMember() && !children.at(i).isStatic() ) {
                members.push_back( children.at(i) );

            } else if ( children.at(i).isFunction() ) {
                functions.push_back( children.at(i) );
            }
        }
    }
}

bool PHPRefactoring::FindByName(const PHPEntry::Vector_t& entries, const wxString& name) const
{
    for(size_t i=0; i<entries.size(); ++i) {
        if ( entries.at(i).getName() == name ) {
            return true;
        }
    }
    return false;
}

PHPSetterGetterEntry::Vec_t PHPRefactoring::GetGetters(IEditor* editor) const
{
    PHPSetterGetterEntry::Vec_t getters;
    if ( !editor ) {
        return getters;
    }
    
    
    // retrieve the scope
    PHPEntry::Vector_t members, functions;
    PHPEntry scopeEntry = PHPCodeCompletion::Instance()->GetClassNearPosition(editor, editor->GetCurrentPosition());
    GetMembersAndFunctions(scopeEntry, members, functions);
    
    if ( members.empty() ) {
        return getters;
    }
    
    // Prepare list of candidates
    PHPSetterGetterEntry::Vec_t candidates;
    for(size_t i=0; i<members.size(); ++i) {
        PHPSetterGetterEntry candidate(members.at(i));
        
        // make sure we don't add getters that already exist
        if ( FindByName(functions, candidate.GetGetter(kSG_NameOnly) ) ) {
            continue;
        }
        candidates.push_back( candidate );
    }
    
    getters.swap( candidates );
    return getters;
}
