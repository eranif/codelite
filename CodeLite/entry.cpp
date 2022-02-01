//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : entry.cpp
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

#include "entry.h"

#include "CompletionHelper.hpp"
#include "CxxScannerTokens.h"
#include "CxxTokenizer.h"
#include "code_completion_api.h"
#include "comment_parser.h"
#include "ctags_manager.h"
#include "language.h"
#include "macros.h"
#include "pptable.h"
#include "precompiled_header.h"
#include "tokenizer.h"
#include "wxStringHash.h"

#include <wx/regex.h>
#include <wx/tokenzr.h>

TagEntry::TagEntry()
    : m_path(wxEmptyString)
    , m_file(wxEmptyString)
    , m_lineNumber(-1)
    , m_pattern(wxEmptyString)
    , m_kind("<unknown>")
    , m_parent(wxEmptyString)
    , m_name(wxEmptyString)
    , m_id(wxNOT_FOUND)
    , m_scope(wxEmptyString)
    , m_flags(0)
{
}

TagEntry::~TagEntry() {}

TagEntry::TagEntry(const TagEntry& rhs) { *this = rhs; }

TagEntry& TagEntry::operator=(const TagEntry& rhs)
{
    m_id = rhs.m_id;
    m_file = rhs.m_file.c_str();
    m_kind = rhs.m_kind.c_str();
    m_parent = rhs.m_parent.c_str();
    m_pattern = rhs.m_pattern.c_str();
    m_lineNumber = rhs.m_lineNumber;
    m_name = rhs.m_name.c_str();
    m_path = rhs.m_path.c_str();
#if wxUSE_GUI
    m_hti = rhs.m_hti;
#endif
    m_scope = rhs.m_scope.c_str();
    m_flags = rhs.m_flags;

    // loop over the map and copy item by item
    // we use the c_str() method to force our own copy of the string and to avoid
    // ref counting which may cause crash when sharing wxString among threads
    m_extFields.clear();
    wxStringMap_t::const_iterator iter = rhs.m_extFields.begin();
    for(; iter != rhs.m_extFields.end(); iter++) {
        m_extFields[iter->first.c_str()] = iter->second.c_str();
    }
    m_comment = rhs.m_comment;
    return *this;
}

bool TagEntry::operator==(const TagEntry& rhs)
{
    // Note: tree item id is not used in this function!
    bool res = m_scope == rhs.m_scope && m_file == rhs.m_file && m_kind == rhs.m_kind && m_parent == rhs.m_parent &&
               m_pattern == rhs.m_pattern && m_name == rhs.m_name && m_path == rhs.m_path &&
               m_lineNumber == rhs.m_lineNumber && GetInheritsAsString() == rhs.GetInheritsAsString() &&
               GetAccess() == rhs.GetAccess() && GetSignature() == rhs.GetSignature();
    return res;
}

void TagEntry::Create(const wxString& fileName, const wxString& name, int lineNumber, const wxString& pattern,
                      const wxString& kind, wxStringMap_t& extFields)
{
    m_flags = 0;
    m_extFields = extFields;
    SetName(name);
    SetLine(lineNumber);
    SetKind(kind.IsEmpty() ? "<unknown>" : kind);
    SetPattern(pattern);
    SetFile(wxFileName(fileName).GetFullPath());
    SetId(-1);

    wxString path;

    // Check if we can get full name (including path)
    static vector<wxString> scope_fields = { "class", "struct", "namespace", "interface", "enum", "function" };
    for(const wxString& scope_field : scope_fields) {
        path = GetExtField(scope_field);
        if(!path.IsEmpty()) {
            UpdatePath(path);
            break;
        }
    }

    // still no path?
    // try union
    if(path.empty()) {
        path = GetExtField("union");
        wxString tmpname = path.AfterLast(':');
        if(!path.IsEmpty()) {
            if(!tmpname.StartsWith("__anon")) {
                UpdatePath(path);
            } else {
                // anonymouse union, remove the anonymous part from its name
                path = path.BeforeLast(':');
                path = path.BeforeLast(':');
                UpdatePath(path);
            }
        }
    }

    // update the method properties
    SetTagProperties(GetExtField("properties"));
    if(!path.IsEmpty()) {
        SetScope(path);
    } else {
        SetScope("<global>");
    }

    // If there is no path, path is set to name
    if(GetPath().IsEmpty())
        SetPath(GetName());

    // Get the parent name
    StringTokenizer tok(GetPath(), "::");
    wxString parent;

    (tok.Count() < 2) ? parent = "<global>" : parent = tok[tok.Count() - 2];
    SetParent(parent);

    // parse auto
    m_assignment = TypenameFromPattern(this);
    if(IsAuto(this)) {
        m_tag_properties_flags |= TAG_PROP_AUTO_VARIABLE;
    }
    if(IsFunction() && GetName().StartsWith("__anon")) {
        m_tag_properties_flags |= TAG_PROP_INLINE;
    }
}

void TagEntry::Print()
{
    std::cout << "======================================" << std::endl;
    std::cout << "Name:\t\t" << GetName() << std::endl;
    std::cout << "File:\t\t" << GetFile() << std::endl;
    std::cout << "Line:\t\t" << GetLine() << std::endl;
    std::cout << "Pattern\t\t" << GetPattern() << std::endl;
    std::cout << "Kind:\t\t" << GetKind() << std::endl;
    std::cout << "Parent:\t\t" << GetParent() << std::endl;

    std::cout << " ---- Ext fields: ---- " << std::endl;
    wxStringMap_t::const_iterator iter = m_extFields.begin();
    for(; iter != m_extFields.end(); iter++)
        std::cout << iter->first << ":\t\t" << iter->second << std::endl;
    std::cout << "======================================" << std::endl;
}

wxString TagEntry::Key() const
{
    wxString key;
    if(IsPrototype() || IsMacro()) {
        key << GetKind() << ": ";
    }

    key << GetPath() << GetSignature();
    return key;
}

wxString TagEntry::GetDisplayName() const
{
    wxString name;
    name << GetName() << GetSignature();
    return name;
}

wxString TagEntry::GetFullDisplayName() const
{
    wxString name;

    if(GetParent() == "<global>") {
        name << GetDisplayName();
    } else {
        name << GetParent() << "::" << GetName() << GetSignature();
    }

    return name;
}

bool TagEntry::IsClassTemplate() const
{
    return m_extFields.count("template") && !m_extFields.find("template")->second.empty();
}

//----------------------------------------------------------------------------
// Database operations
//----------------------------------------------------------------------------

wxString TagEntry::GetScopeName() const { return GetScope(); }

wxString TagEntry::GetKind() const
{
    wxString kind(m_kind);
    kind.Trim();
    return kind;
}

const bool TagEntry::IsContainer() const
{
    return IsClass() || IsStruct() || IsUnion() || IsNamespace() || IsEnumClass();
}

void TagEntry::UpdatePath(wxString& path)
{
    if(!path.IsEmpty()) {
        wxString name(path);
        name += "::";
        name += GetName();
        SetPath(name);
    }
}

const wxString& TagEntry::GetExtField(const wxString& extField) const
{
    static wxString empty_string;
    if(m_extFields.count(extField) == 0) {
        return empty_string;
    }
    return m_extFields.find(extField)->second;
}

wxString TagEntry::GetPattern() const
{
    wxString pattern(m_pattern);
    // since ctags's pattern is regex, forward slashes are escaped. ('/' becomes '\/')
    pattern.Replace("\\\\", "\\");
    pattern.Replace("\\/", "/");
    return pattern;
}

void TagEntry::FromLine(const wxString& line)
{
    // label	C:\src\wxCustomControls\clTreeCtrl\clChoice.cpp	/^        const wxString& label = m_choices[i];$/;"
    // local line:116	type:constwxString
    wxString pattern, kind;
    wxString strLine = line;
    long lineNumber = wxNOT_FOUND;
    wxStringMap_t extFields;

    // get the token name
    wxString name = strLine.BeforeFirst('\t');
    strLine = strLine.AfterFirst('\t');

    // get the file name
    wxString fileName = strLine.BeforeFirst('\t');
    strLine = strLine.AfterFirst('\t');

    // here we can get two options:
    // pattern followed by ;"
    // or
    // line number followed by ;"
    int end = strLine.Find(";\"");
    if(end == wxNOT_FOUND) {
        // invalid pattern found
        return;
    }

    if(strLine.StartsWith("/^")) {
        // regular expression pattern found
        pattern = strLine.Mid(0, end);
        strLine = strLine.Right(strLine.Length() - (end + 2));
    } else {
        // line number pattern found, this is usually the case when
        // dealing with macros in C++
        pattern = strLine.Mid(0, end);
        strLine = strLine.Right(strLine.Length() - (end + 2));

        pattern = pattern.Trim();
        pattern = pattern.Trim(false);
        pattern.ToLong(&lineNumber);
    }

    // next is the kind of the token
    if(strLine.StartsWith("\t")) {
        strLine = strLine.AfterFirst('\t');
    }

    kind = strLine.BeforeFirst('\t');
    strLine = strLine.AfterFirst('\t');

    if(strLine.IsEmpty() == false) {
        wxStringTokenizer tkz(strLine, '\t');
        while(tkz.HasMoreTokens()) {
            wxString token = tkz.NextToken();
            wxString key = token.BeforeFirst(':');
            wxString val = token.AfterFirst(':');
            key = key.Trim();
            key = key.Trim(false);

            val = val.Trim();
            val = val.Trim(false);
            if(key == "line" && !val.IsEmpty()) {
                val.ToLong(&lineNumber);
            } else {
                if(key == "union" || key == "struct") {

                    // remove the anonymous part of the struct / union
                    if(!val.StartsWith("__anon")) {
                        // an internal anonymous union / struct
                        // remove all parts of the
                        wxArrayString scopeArr;
                        wxString tmp, new_val;

                        scopeArr = wxStringTokenize(val, ":", wxTOKEN_STRTOK);
                        for(size_t i = 0; i < scopeArr.GetCount(); i++) {
                            if(scopeArr.Item(i).StartsWith("__anon") == false) {
                                tmp << scopeArr.Item(i) << "::";
                            }
                        }

                        tmp.EndsWith("::", &new_val);
                        val.swap(new_val);
                    }
                }
                extFields.insert({ key, val });
            }
        }
    }

    kind = kind.Trim();
    name = name.Trim();
    fileName = fileName.Trim();
    pattern = pattern.Trim();
    Create(fileName, name, lineNumber, pattern, kind, extFields);
}

bool TagEntry::IsConstructor() const
{
    if(GetKind() != "function" && GetKind() != "prototype")
        return false;

    return GetName() == GetScope();
}

bool TagEntry::IsDestructor() const
{
    if(GetKind() != "function" && GetKind() != "prototype")
        return false;

    return GetName().StartsWith("~");
}

wxString TagEntry::GetTemplateDefinition() const
{
    // template:<class T>
    wxString definition = GetExtField("template");
    definition.Trim().Trim(false);
    return definition;
}

void TagEntry::SetTypename(const wxString& val) { m_extFields["typeref"] = "typename:" + val; }

wxString TagEntry::GetTypename() const
{
    // typeref:typename:void
    const wxString& returnValue = GetExtField("typeref");
    return returnValue.AfterFirst(':');
}

bool TagEntry::IsKeyword() const { return m_tag_kind == eTagKind::TAG_KIND_KEYWORD; }
bool TagEntry::IsEnum() const { return m_tag_kind == eTagKind::TAG_KIND_ENUM; }
bool TagEntry::IsEnumerator() const { return m_tag_kind == eTagKind::TAG_KIND_ENUMERATOR; }
bool TagEntry::IsParameter() const { return m_tag_kind == eTagKind::TAG_KIND_PARAMETER; }
bool TagEntry::IsVariable() const { return m_tag_kind == eTagKind::TAG_KIND_VARIABLE; }
bool TagEntry::IsEnumClass() const { return m_tag_kind == eTagKind::TAG_KIND_CENUM; }
bool TagEntry::IsLocalVariable() const { return m_tag_kind == eTagKind::TAG_KIND_LOCAL; }
bool TagEntry::IsMember() const { return m_tag_kind == eTagKind::TAG_KIND_MEMBER; }
bool TagEntry::IsNamespace() const { return m_tag_kind == eTagKind::TAG_KIND_NAMESPACE; }
bool TagEntry::IsFunction() const { return m_tag_kind == eTagKind::TAG_KIND_FUNCTION; }
bool TagEntry::IsPrototype() const { return m_tag_kind == eTagKind::TAG_KIND_PROTOTYPE; }
bool TagEntry::IsClass() const { return m_tag_kind == eTagKind::TAG_KIND_CLASS; }
bool TagEntry::IsMacro() const { return m_tag_kind == eTagKind::TAG_KIND_MACRO; }
bool TagEntry::IsStruct() const { return m_tag_kind == eTagKind::TAG_KIND_STRUCT; }
bool TagEntry::IsUnion() const { return m_tag_kind == eTagKind::TAG_KIND_UNION; }
bool TagEntry::IsTypedef() const { return m_tag_kind == eTagKind::TAG_KIND_TYPEDEF; }
bool TagEntry::IsMethod() const { return IsPrototype() || IsFunction(); }
bool TagEntry::IsScopeGlobal() const { return GetScope().IsEmpty() || GetScope() == "<global>"; }

wxString TagEntry::GetInheritsAsString() const { return GetExtField(_T("inherits")); }

wxArrayString TagEntry::GetInheritsAsArrayNoTemplates() const
{
    // Parse the input string
    wxString inherits = GetInheritsAsString();
    wxString parent;
    wxArrayString parentsArr;

    int depth(0);
    for(size_t i = 0; i < inherits.Length(); i++) {
        wxChar ch = inherits.GetChar(i);

        switch(ch) {
        case '<':
            if(depth == 0 && parent.IsEmpty() == false) {
                parent.Trim().Trim(false);
                parentsArr.Add(parent);
                parent.Clear();
            }
            depth++;
            break;

        case '>':
            depth--;
            break;

        case ',':
            if(depth == 0 && parent.IsEmpty() == false) {
                parent.Trim().Trim(false);
                parentsArr.Add(parent);
                parent.Clear();
            }
            break;

        default:
            if(depth == 0) {
                parent << ch;
            }
            break;
        }
    }

    if(parent.IsEmpty() == false) {
        parent.Trim().Trim(false);
        parentsArr.Add(parent);
    }
    return parentsArr;
}

wxArrayString TagEntry::GetInheritsAsArrayWithTemplates() const
{
    // Parse the input string
    wxString inherits = GetInheritsAsString();
    wxString parent;
    wxArrayString parentsArr;

    int depth(0);
    for(size_t i = 0; i < inherits.Length(); i++) {
        wxChar ch = inherits.GetChar(i);

        switch(ch) {
        case '<':
            depth++;
            parent << ch;
            break;

        case '>':
            depth--;
            parent << ch;
            break;

        case ',':
            if(depth == 0 && parent.IsEmpty() == false) {
                parent.Trim().Trim(false);
                parentsArr.Add(parent);
                parent.Clear();

            } else if(depth != 0) {
                parent << ch;
            }
            break;

        default:
            parent << ch;
            break;
        }
    }

    if(parent.IsEmpty() == false) {
        parent.Trim().Trim(false);
        parentsArr.Add(parent);
    }
    return parentsArr;
}

TagEntryPtr TagEntry::ReplaceSimpleMacro()
{
    if(IsMacro()) {
        PPToken tok = TagsManagerST::Get()->GetDatabase()->GetMacro(GetName());
        if(tok.flags & PPToken::IsValid && !(tok.flags & PPToken::IsFunctionLike)) {
            std::vector<TagEntryPtr> tags;
            TagsManagerST::Get()->FindByNameAndScope(tok.replacement, GetScopeName(), tags);
            if(tags.size() == 1) {
                // replace the current tag content with the new match
                return tags.at(0);
            }
        }
    }
    return NULL;
}

bool TagEntry::IsTemplateFunction() const
{
    wxString pattern = GetPatternClean();
    pattern.Trim().Trim(false);
    return IsMethod() && pattern.StartsWith("template ");
}

wxString TagEntry::GetPatternClean() const
{
    wxString p = GetPattern();
    p.Trim();
    if(p.StartsWith("/^")) {
        p.Replace("/^", "");
    }

    if(p.EndsWith("$/")) {
        p.Replace("$/", "");
    }
    return p;
}

wxString TagEntry::GetLocalType() const { return GetExtField("type"); }

namespace
{
inline void enable_function_flag_if_exists(const wxStringSet_t& S, const wxString& propname, const size_t flag,
                                           size_t& flags)
{
    if(S.count(propname)) {
        flags |= flag;
    } else {
        flags &= ~flag;
    }
}
} // namespace

void TagEntry::SetTagProperties(const wxString& props)
{
    m_tag_properties = props;
    auto tokens = wxStringTokenize(m_tag_properties, ",", wxTOKEN_STRTOK);
    wxStringSet_t S;
    for(auto& token : tokens) {
        token.Trim().Trim(false);
        S.insert(token);
    }

    enable_function_flag_if_exists(S, "const", TAG_PROP_CONST, m_tag_properties_flags);
    enable_function_flag_if_exists(S, "virtual", TAG_PROP_VIRTUAL, m_tag_properties_flags);
    enable_function_flag_if_exists(S, "default", TAG_PROP_DEFAULT, m_tag_properties_flags);
    enable_function_flag_if_exists(S, "delete", TAG_PROP_DELETED, m_tag_properties_flags);
    enable_function_flag_if_exists(S, "static", TAG_PROP_STATIC, m_tag_properties_flags);
    enable_function_flag_if_exists(S, "inline", TAG_PROP_INLINE, m_tag_properties_flags);
    enable_function_flag_if_exists(S, "override", TAG_PROP_OVERRIDE, m_tag_properties_flags);
    enable_function_flag_if_exists(S, "pure", TAG_PROP_PURE, m_tag_properties_flags);
    enable_function_flag_if_exists(S, "scopedenum", TAG_PROP_SCOPEDENUM, m_tag_properties_flags);

    // change the kind to "enum class"
    if(is_scoped_enum()) {
        m_tag_kind = eTagKind::TAG_KIND_CENUM;
    }
}

bool TagEntry::is_func_virtual() const { return m_tag_properties_flags & TAG_PROP_VIRTUAL; }
bool TagEntry::is_func_default() const { return m_tag_properties_flags & TAG_PROP_DEFAULT; }
bool TagEntry::is_func_override() const { return m_tag_properties_flags & TAG_PROP_OVERRIDE; }
bool TagEntry::is_func_deleted() const { return m_tag_properties_flags & TAG_PROP_DELETED; }
bool TagEntry::is_func_inline() const { return m_tag_properties_flags & TAG_PROP_INLINE; }
bool TagEntry::is_func_pure() const { return m_tag_properties_flags & TAG_PROP_PURE; }
bool TagEntry::is_scoped_enum() const { return m_tag_properties_flags & TAG_PROP_SCOPEDENUM; }
bool TagEntry::is_const() const { return m_tag_properties_flags & TAG_PROP_CONST; }
bool TagEntry::is_static() const { return m_tag_properties_flags & TAG_PROP_STATIC; }
bool TagEntry::is_auto() const { return m_tag_properties_flags & TAG_PROP_AUTO_VARIABLE; }
bool TagEntry::is_lambda() const { return m_tag_properties_flags & TAG_PROP_LAMBDA; }

wxString TagEntry::GetFunctionDeclaration() const
{
    if(!IsMethod()) {
        return wxEmptyString;
    }
    wxString decl;
    if(is_func_inline()) {
        decl << "inline ";
    }
    if(is_func_virtual()) {
        decl << "virtual ";
    }
    decl << GetTypename() << " ";
    if(!GetScope().empty()) {
        decl << GetScope() << "::";
    }
    decl << GetName() << GetSignature();
    if(is_const()) {
        decl << " const";
    }
    if(is_func_pure()) {
        decl << " = 0";
    }
    decl << ";";
    return decl;
}

wxString TagEntry::GetFunctionDefinition() const
{
    wxString impl;
    if(!IsMethod()) {
        return wxEmptyString;
    }
    impl << GetTypename() << " ";
    if(!GetScope().empty()) {
        impl << GetScope() << "::";
    }

    CompletionHelper helper;
    impl << helper.normalize_function(this, CompletionHelper::STRIP_NO_DEFAULT_VALUES);
    // release the pointer from the smart-ptr
    // this ensure that `this` is not deleted
    return impl;
}

namespace
{
std::unordered_map<wxString, eTagKind> g_kind_table = {
    { "class", eTagKind::TAG_KIND_CLASS },         { "struct", eTagKind::TAG_KIND_STRUCT },
    { "namespace", eTagKind::TAG_KIND_NAMESPACE }, { "union", eTagKind::TAG_KIND_UNION },
    { "enum", eTagKind::TAG_KIND_ENUM },           { "member", eTagKind::TAG_KIND_MEMBER },
    { "variable", eTagKind::TAG_KIND_VARIABLE },   { "macro", eTagKind::TAG_KIND_MACRO },
    { "typedef", eTagKind::TAG_KIND_TYPEDEF },     { "local", eTagKind::TAG_KIND_LOCAL },
    { "parameter", eTagKind::TAG_KIND_PARAMETER }, { "prototype", eTagKind::TAG_KIND_PROTOTYPE },
    { "cpp_keyword", eTagKind::TAG_KIND_KEYWORD }, { "keyword", eTagKind::TAG_KIND_KEYWORD },
    { "function", eTagKind::TAG_KIND_FUNCTION },   { "enumerator", eTagKind::TAG_KIND_ENUMERATOR },
};
}

void TagEntry::SetKind(const wxString& kind)
{
    // set the string kind
    m_kind = kind;
    // turn on bits
    m_tag_kind = eTagKind::TAG_KIND_UNKNOWN;
    if(g_kind_table.count(m_kind)) {
        m_tag_kind = g_kind_table[m_kind];
    }
}

namespace
{
void read_until_find(CxxTokenizer& tokenizer, CxxLexerToken& token, int type_1, int type_2, int* what_was_found,
                     wxString* consumed)
{
    // search until we find the `=`
    int depth = 0;
    consumed->clear();
    *what_was_found = 0;
    consumed->reserve(256); // 256 bytes should be enough for most cases

    while(tokenizer.NextToken(token)) {
        if(depth == 0 && token.GetType() == type_1) {
            *what_was_found = type_1;
            consumed->Trim().Trim(false);
            return;
        } else if(depth == 0 && token.GetType() == type_2) {
            *what_was_found = type_2;
            consumed->Trim().Trim(false);
            return;
        }

        if(token.is_keyword() || token.is_builtin_type()) {
            consumed->Append(token.GetWXString() + " ");
            continue;
        } else if(token.is_pp_keyword()) {
            continue;
        }

        // append it
        consumed->Append(token.GetWXString());
        switch(token.GetType()) {
        case '<':
        case '{':
        case '[':
        case '(':
            depth++;
            break;
        case '>':
        case '}':
        case ']':
        case ')':
            depth--;
            break;
        default:
            break;
        }
    }

    // eof
    consumed->Trim().Trim(false);
}
} // namespace

#define CHECK_FOUND(What, Expected) \
    if(What != Expected)            \
    return wxEmptyString

wxString TagEntry::TypenameFromPattern(const TagEntry* tag)
{
    if(!tag->IsLocalVariable() && !tag->IsVariable()) {
        return wxEmptyString;
    }
    CxxTokenizer tokenizer;
    CxxLexerToken token;

    tokenizer.Reset(tag->GetPatternClean());

    // search until we find the `=`
    int what_was_found = 0;
    wxString consumed;
    read_until_find(tokenizer, token, T_FOR, '=', &what_was_found, &consumed);
    if(what_was_found == 0) {
        return wxEmptyString;
    }

    if(what_was_found == '=') {
        // read until we reach eof or ';'
        read_until_find(tokenizer, token, ';', 0, &what_was_found, &consumed);
        return consumed;
    } else {
        // we are in a for loop
        // search for the ':'
        read_until_find(tokenizer, token, '(', 0, &what_was_found, &consumed);
        CHECK_FOUND('(', what_was_found);

        read_until_find(tokenizer, token, ':', 0, &what_was_found, &consumed);
        CHECK_FOUND(':', what_was_found);

        // we found the ':', now read until we find the ')'
        read_until_find(tokenizer, token, ')', 0, &what_was_found, &consumed);
        CHECK_FOUND(')', what_was_found);
        // for(... : expression)
        // we return expression.begin()
        consumed += ".begin()";
        return consumed;
    }
    return wxEmptyString;
}

bool TagEntry::IsAuto(const TagEntry* tag)
{
    CxxTokenizer tokenizer;
    CxxLexerToken token;

    tokenizer.Reset(tag->GetTypename());
    int found = 0;
    wxString consumed;
    read_until_find(tokenizer, token, T_AUTO, 0, &found, &consumed);
    return found == T_AUTO;
}

wxString TagEntry::GetMacrodef() const { return GetExtField("macrodef"); }

void TagEntry::SetMacrodef(const wxString& value) { set_extra_field("macrodef", value); }

void TagEntry::set_extra_field(const wxString& name, const wxString& value)
{
    if(m_extFields.count(name)) {
        m_extFields.erase(name);
    }
    m_extFields.insert({ name, value });
}
