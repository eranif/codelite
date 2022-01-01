//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : parsedtoken.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "parsedtoken.h"
#include "ctags_manager.h"

ParsedToken::ParsedToken()
    : m_isTemplate(false)
    , m_subscriptOperator(false)
    , m_next(NULL)
    , m_prev(NULL)
{
}

ParsedToken::~ParsedToken() {}

void ParsedToken::DeleteTokens(ParsedToken* head)
{
    if(!head) {
        return;
    }

    ParsedToken* n(head);
    ParsedToken* tmpPtr(NULL);
    while(n) {
        tmpPtr = n->GetNext();
        delete n;

        n = tmpPtr;
    }
}

wxString ParsedToken::GetPath() const
{
    wxString path;
    if(m_typeScope != wxT("<global>"))
        path << m_typeScope;

    if(!path.empty() && !m_type.empty()) {
        path << "::";
    }
    path << m_type;
    return path;
}

wxString ParsedToken::GetContextScope() const
{
    if(GetCurrentScopeName() == GetName() && GetPrev() == NULL) {
        // the scope name is equal the 'name'?
        // copy ctor
        return wxT("<global>");
    }

    if(GetPrev() == NULL) {
        // we are the first in chain, return the current scope name
        return GetCurrentScopeName();
    } else {
        return GetPrev()->GetPath();
    }
}

void ParsedToken::RemoveScopeFromType()
{
    // incase the realName already includes the scope, remove it from the typename
    if(!GetTypeScope().IsEmpty() && GetTypeName().StartsWith(GetTypeScope() + wxT("::"))) {
        wxString tt;
        GetTypeName().StartsWith(GetTypeScope() + wxT("::"), &tt);
        SetTypeName(tt);
    }

    // If the typeName already contains a scope in it, replace the
    // scopename with it
    if(GetTypeName().Contains(wxT("::"))) {
        m_typeScope.Clear();
        wxString tmpTypeName(m_type);
        m_type = tmpTypeName.AfterLast(wxT(':'));
        m_typeScope = tmpTypeName.BeforeLast(wxT(':'));

        if(m_typeScope.EndsWith(wxT(":"))) {
            m_typeScope.Truncate(m_typeScope.Length() - 1);
        }
    }
}

bool ParsedToken::ResovleTemplate(TagsManager* lookup)
{
    wxString oldName = m_type;
    if(!lookup->GetDatabase()->IsTypeAndScopeExistLimitOne(m_type, m_typeScope)) {

        // Loop until the end of the chain and see if any of the context is template
        ParsedToken* cur = this;
        while(cur) {
            if(cur->GetIsTemplate()) {
                // our context is template
                wxString newType = cur->TemplateToType(m_type);

                if(newType != m_type) {
                    m_type = newType;
                    RemoveScopeFromType();
                    return true;
                }
            }
            cur = cur->GetPrev();
        }
    }
    return false;
}

wxString ParsedToken::TemplateToType(const wxString& templateArg)
{
    int where = m_templateArgList.Index(templateArg);
    if(where != wxNOT_FOUND) {
        // it exists, return the name in the templateInstantiation list
        if(m_templateInitialization.GetCount() > (size_t)where && m_templateInitialization.Item(where) != templateArg)
            return m_templateInitialization.Item(where);
    }
    return templateArg;
}

void ParsedToken::ResolveTemplateType(TagsManager* lookup)
{
    for(size_t i = 0; i < m_templateInitialization.GetCount(); i++) {
        if(!lookup->GetDatabase()->IsTypeAndScopeExistLimitOne(m_templateInitialization.Item(i), wxT("<global>"))) {

            // Loop until the end of the chain and see if any of the context is template
            ParsedToken* cur = this;
            while(cur) {
                if(cur->GetIsTemplate()) {
                    // our context is template
                    wxString newType = cur->TemplateToType(m_templateInitialization.Item(i));

                    if(newType != m_templateInitialization.Item(i)) {
                        m_templateInitialization.Item(i) = newType;
                        break;
                    }
                }
                cur = cur->GetPrev();
            }
        }
    }
}

bool ParsedToken::IsThis() const { return m_name == wxT("this"); }

wxString ParsedToken::GetFullScope() const
{
    // rewind
    auto cur = this;
    while(cur && cur->m_prev) {
        cur = cur->m_prev;
    }
    // build the scope
    wxString scope;
    while(cur != this) {
        if(cur->GetTypeName() != "<global>" && !cur->GetTypeName().empty()) {
            if(!scope.empty()) {
                scope << "::";
            }
            scope << cur->GetTypeName();
        }
        cur = cur->m_next;
    }
    return scope;
}
