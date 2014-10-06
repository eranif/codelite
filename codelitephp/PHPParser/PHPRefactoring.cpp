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

PHPSetterGetterEntry::Vec_t PHPRefactoring::GetSetters(IEditor* editor) const
{
    PHPSetterGetterEntry::Vec_t setters, candidates;
    if(!editor) {
        return setters;
    }

    // Parse until the current position
    wxString text = editor->GetTextRange(0, editor->GetCurrentPosition());
    PHPSourceFile sourceFile(text);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.SetFilename(editor->GetFileName());
    sourceFile.Parse();

    const PHPEntityClass* scopeAtPoint = sourceFile.Class()->Cast<PHPEntityClass>();
    if(!scopeAtPoint) {
        return setters;
    }

    // filter out
    const PHPEntityBase::List_t& children = scopeAtPoint->GetChildren();
    PHPEntityBase::List_t::const_iterator iter = children.begin();

    PHPEntityBase::List_t members, functions;
    for(; iter != children.end(); ++iter) {
        PHPEntityBase::Ptr_t child = *iter;
        if(child->Is(kEntityTypeFunction)) {
            functions.push_back(child);
        } else if(child->Is(kEntityTypeVariable) && child->Cast<PHPEntityVariable>()->IsMember() &&
                  !child->Cast<PHPEntityVariable>()->IsConst() && !child->Cast<PHPEntityVariable>()->IsStatic()) {
            // a member of a class which is not a constant
            members.push_back(child);
        }
    }

    if(members.empty()) {
        return setters;
    }

    // Prepare list of candidates
    PHPEntityBase::List_t::iterator membersIter = members.begin();
    for(; membersIter != members.end(); ++membersIter) {
        PHPSetterGetterEntry candidate(*membersIter);

        // make sure we don't add setters that already exist
        if(FindByName(functions, candidate.GetSetter(kSG_NameOnly))) {
            continue;
        }
        candidates.push_back(candidate);
    }

    setters.swap(candidates);
    return setters;
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

PHPSetterGetterEntry::Vec_t PHPRefactoring::GetGetters(IEditor* editor) const
{
    PHPSetterGetterEntry::Vec_t getters, candidates;
    if(!editor) {
        return getters;
    }

    // Parse until the current position
    wxString text = editor->GetTextRange(0, editor->GetCurrentPosition());
    PHPSourceFile sourceFile(text);
    sourceFile.SetParseFunctionBody(true);
    sourceFile.SetFilename(editor->GetFileName());
    sourceFile.Parse();

    const PHPEntityClass* scopeAtPoint = sourceFile.Class()->Cast<PHPEntityClass>();
    if(!scopeAtPoint) {
        return getters;
    }

    // filter out
    const PHPEntityBase::List_t& children = scopeAtPoint->GetChildren();
    PHPEntityBase::List_t::const_iterator iter = children.begin();

    PHPEntityBase::List_t members, functions;
    for(; iter != children.end(); ++iter) {
        PHPEntityBase::Ptr_t child = *iter;
        if(child->Is(kEntityTypeFunction)) {
            functions.push_back(child);
        } else if(child->Is(kEntityTypeVariable) && child->Cast<PHPEntityVariable>()->IsMember() &&
                  !child->Cast<PHPEntityVariable>()->IsConst() && !child->Cast<PHPEntityVariable>()->IsStatic()) {
            // a member of a class
            members.push_back(child);
        }
    }

    if(members.empty()) {
        return getters;
    }

    // Prepare list of candidates
    PHPEntityBase::List_t::iterator membersIter = members.begin();
    for(; membersIter != members.end(); ++membersIter) {
        PHPSetterGetterEntry candidate(*membersIter);

        // make sure we don't add setters that already exist
        if(FindByName(functions, candidate.GetGetter(kSG_NameOnly))) {
            continue;
        }
        candidates.push_back(candidate);
    }

    getters.swap(candidates);
    return getters;
}
