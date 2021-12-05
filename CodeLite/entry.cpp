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

#include "CompletionHelper.hpp"
#include "ctags_manager.h"
#include "pptable.h"
#include "precompiled_header.h"

#include "code_completion_api.h"
#include "comment_parser.h"
#include "entry.h"
#include "language.h"
#include "macros.h"
#include "tokenizer.h"
#include "wxStringHash.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>

TagEntry::TagEntry(const tagEntry& entry)
    : m_isClangTag(false)
    , m_flags(0)
    , m_isCommentForamtted(false)
{
    Create(entry);
}

TagEntry::TagEntry()
    : m_path(wxEmptyString)
    , m_file(wxEmptyString)
    , m_lineNumber(-1)
    , m_pattern(wxEmptyString)
    , m_kind(wxT("<unknown>"))
    , m_parent(wxEmptyString)
    , m_name(wxEmptyString)
    , m_id(wxNOT_FOUND)
    , m_scope(wxEmptyString)
    , m_differOnByLineNumber(false)
    , m_isClangTag(false)
    , m_flags(0)
    , m_isCommentForamtted(false)
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
    m_isClangTag = rhs.m_isClangTag;
    m_differOnByLineNumber = rhs.m_differOnByLineNumber;
    m_flags = rhs.m_flags;
    m_formattedComment = rhs.m_formattedComment;
    m_isCommentForamtted = rhs.m_isCommentForamtted;

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
               GetAccess() == rhs.GetAccess() && GetSignature() == rhs.GetSignature() &&
               GetTyperef() == rhs.GetTyperef();

    bool res2 = m_scope == rhs.m_scope && m_file == rhs.m_file && m_kind == rhs.m_kind && m_parent == rhs.m_parent &&
                m_pattern == rhs.m_pattern && m_name == rhs.m_name && m_path == rhs.m_path &&
                GetInheritsAsString() == rhs.GetInheritsAsString() && GetAccess() == rhs.GetAccess() &&
                GetSignature() == rhs.GetSignature() && GetTyperef() == rhs.GetTyperef();

    if(res2 && !res) {
        // the entries are differs only in the line numbers
        m_differOnByLineNumber = true;
    }
    return res;
}

void TagEntry::Create(const wxString& fileName, const wxString& name, int lineNumber, const wxString& pattern,
                      const wxString& kind, wxStringMap_t& extFields)
{
    m_isCommentForamtted = false;
    m_flags = 0;
    m_isClangTag = false;
    SetName(name);
    SetLine(lineNumber);
    SetKind(kind.IsEmpty() ? wxT("<unknown>") : kind);
    SetPattern(pattern);
    SetFile(fileName);
    SetId(-1);
    m_extFields = extFields;
    wxString path;

    // Check if we can get full name (including path)
    path = GetExtField(wxT("class"));
    if(!path.IsEmpty()) {
        UpdatePath(path);
    } else {
        path = GetExtField(wxT("struct"));
        if(!path.IsEmpty()) {
            UpdatePath(path);
        } else {
            path = GetExtField(wxT("namespace"));
            if(!path.IsEmpty()) {
                UpdatePath(path);
            } else {
                path = GetExtField(wxT("interface"));
                if(!path.IsEmpty()) {
                    UpdatePath(path);
                } else {
                    path = GetExtField(wxT("enum"));
                    if(!path.IsEmpty()) {
                        UpdatePath(path);
                    } else {
                        path = GetExtField(wxT("cenum"));
                        if(!path.IsEmpty()) {
                            UpdatePath(path);
                        } else {
                            path = GetExtField(wxT("union"));
                            wxString tmpname = path.AfterLast(wxT(':'));
                            if(!path.IsEmpty()) {
                                if(!tmpname.StartsWith(wxT("__anon"))) {
                                    UpdatePath(path);
                                } else {
                                    // anonymouse union, remove the anonymous part from its name
                                    path = path.BeforeLast(wxT(':'));
                                    path = path.BeforeLast(wxT(':'));
                                    UpdatePath(path);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if(!path.IsEmpty()) {
        SetScope(path);
    } else {
        SetScope(wxT("<global>"));
    }

    // If there is no path, path is set to name
    if(GetPath().IsEmpty())
        SetPath(GetName());

    // Get the parent name
    StringTokenizer tok(GetPath(), wxT("::"));
    wxString parent;

    (tok.Count() < 2) ? parent = wxT("<global>") : parent = tok[tok.Count() - 2];
    SetParent(parent);
}

void TagEntry::Create(const tagEntry& entry)
{
    m_isCommentForamtted = false;
    m_isClangTag = false;
    // Get other information from the string data and store it into map
    for(int i = 0; i < entry.fields.count; ++i) {
        wxString key = _U(entry.fields.list[i].key);
        wxString value = _U(entry.fields.list[i].value);
        m_extFields[key] = value;
    }
    Create(_U(entry.file), _U(entry.name), entry.address.lineNumber, _U(entry.address.pattern), _U(entry.kind),
           m_extFields);
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
    if(GetKind() == wxT("prototype") || GetKind() == wxT("macro")) {
        key << GetKind() << wxT(": ");
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

    if(GetParent() == wxT("<global>")) {
        name << GetDisplayName();
    } else {
        name << GetParent() << wxT("::") << GetName() << GetSignature();
    }

    return name;
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
    return GetKind() == wxT("class") || GetKind() == wxT("struct") || GetKind() == wxT("union") ||
           GetKind() == wxT("namespace") || GetKind() == wxT("project") || GetKind() == "cenum";
}

void TagEntry::UpdatePath(wxString& path)
{
    if(!path.IsEmpty()) {
        wxString name(path);
        name += wxT("::");
        name += GetName();
        SetPath(name);
    }
}

wxString TagEntry::TypeFromTyperef() const
{
    wxString typeref = GetTyperef();
    if(typeref.IsEmpty() == false) {
        wxString name = typeref.BeforeFirst(wxT(':'));
        return name;
    }
    return wxEmptyString;
}

static bool GetMacroArgList(CppScanner& scanner, wxArrayString& argList)
{
    int depth(0);
    int type(0);
    bool cont(true);
    bool isOk(false);

    wxString word;

    while(cont) {
        type = scanner.yylex();
        if(type == 0) {
            // eof
            break;
        }

        switch(type) {
        case(int)'(':
            isOk = true;
            depth++;

            if(word.empty() == false)
                word << wxT("(");

            break;

        case(int)')':
            depth--;
            if(depth == 0) {
                cont = false;
                break;
            } else {
                word << wxT(")");
            }
            break;

        case(int)',':
            word.Trim().Trim(false);
            if(!word.empty()) {
                argList.Add(word);
            }
            word.clear();
            break;

        default:
            word << wxString::From8BitData(scanner.YYText()) << wxT(" ");
            break;
        }
    }

    if(word.empty() == false) {
        argList.Add(word);
    }

    return (depth == 0) && isOk;
}

wxString TagEntry::NameFromTyperef(wxString& templateInitList, bool nameIncludeTemplate)
{
    wxString typeref = GetTyperef();
    if(typeref.IsEmpty() == false) {
        wxString name = typeref.AfterFirst(wxT(':'));
        return name;
    }

    // incase our entry is a typedef, and it is not marked as typeref,
    // try to get the real name from the pattern
    if(GetKind() == wxT("typedef")) {

        wxString pat(GetPattern());
        if(!GetPattern().Contains(wxT("typedef")) && !GetPattern().Contains(wxT("using"))) {
            // The pattern does not contain 'typedef' however this *is* a typedef
            // try to see if this is a macro
            pat.StartsWith(wxT("/^"), &pat);
            pat.Trim().Trim(false);

            // we take the first token
            CppScanner scanner;
            scanner.SetText(pat.To8BitData());
            int type = scanner.yylex();
            if(type == IDENTIFIER) {
                wxString token = wxString::From8BitData(scanner.YYText());

                PPToken tok = TagsManagerST::Get()->GetDatabase()->GetMacro(token);
                if(tok.flags & PPToken::IsValid) {
                    // we found a match!
                    if(tok.flags & PPToken::IsFunctionLike) {
                        wxArrayString argList;
                        if(GetMacroArgList(scanner, argList)) {
                            tok.expandOnce(argList);
                        }
                    }
                    pat = tok.replacement;
                    pat << wxT(";");

                    // Remove double spaces
                    while(pat.Replace(wxT("  "), wxT(" "))) {}
                }
            }
        }

        wxString name;
        if(TypedefFromPattern(pat, GetName(), name, templateInitList, nameIncludeTemplate))
            return name;
    }

    return wxEmptyString;
}

bool TagEntry::TypedefFromPattern(const wxString& tagPattern, const wxString& typedefName, wxString& name,
                                  wxString& templateInit, bool nameIncludeTemplate)
{
    wxString pattern(tagPattern);

    pattern.StartsWith(wxT("/^"), &pattern);
    const wxCharBuffer cdata = pattern.mb_str(wxConvUTF8);

    clTypedefList li;
    get_typedefs(cdata.data(), li);

    if(li.size() == 1) {
        clTypedef td = *li.begin();
        templateInit = _U(td.m_realType.m_templateDecl.c_str());
        if(td.m_realType.m_typeScope.empty() == false) {
            name << _U(td.m_realType.m_typeScope.c_str());
            if(nameIncludeTemplate) {
                name << templateInit;
            }
            name << wxT("::");
        }

        name << _U(td.m_realType.m_type.c_str());
        return true;
    }
    return false;
}

wxString TagEntry::GetPattern() const
{
    wxString pattern(m_pattern);
    // since ctags's pattern is regex, forward slashes are escaped. ('/' becomes '\/')
    pattern.Replace(wxT("\\\\"), wxT("\\"));
    pattern.Replace(wxT("\\/"), wxT("/"));
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
    wxString name = strLine.BeforeFirst(wxT('\t'));
    strLine = strLine.AfterFirst(wxT('\t'));

    // get the file name
    wxString fileName = strLine.BeforeFirst(wxT('\t'));
    strLine = strLine.AfterFirst(wxT('\t'));

    // here we can get two options:
    // pattern followed by ;"
    // or
    // line number followed by ;"
    int end = strLine.Find(wxT(";\""));
    if(end == wxNOT_FOUND) {
        // invalid pattern found
        return;
    }

    if(strLine.StartsWith(wxT("/^"))) {
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
    if(strLine.StartsWith(wxT("\t"))) {
        strLine = strLine.AfterFirst(wxT('\t'));
    }

    kind = strLine.BeforeFirst(wxT('\t'));
    strLine = strLine.AfterFirst(wxT('\t'));

    if(strLine.IsEmpty() == false) {
        wxStringTokenizer tkz(strLine, wxT('\t'));
        while(tkz.HasMoreTokens()) {
            wxString token = tkz.NextToken();
            wxString key = token.BeforeFirst(wxT(':'));
            wxString val = token.AfterFirst(wxT(':'));
            key = key.Trim();
            key = key.Trim(false);

            val = val.Trim();
            val = val.Trim(false);
            if(key == wxT("line") && !val.IsEmpty()) {
                val.ToLong(&lineNumber);
            } else {
                if(key == wxT("union") || key == wxT("struct")) {

                    // remove the anonymous part of the struct / union
                    if(!val.StartsWith(wxT("__anon"))) {
                        // an internal anonymous union / struct
                        // remove all parts of the
                        wxArrayString scopeArr;
                        wxString tmp, new_val;

                        scopeArr = wxStringTokenize(val, wxT(":"), wxTOKEN_STRTOK);
                        for(size_t i = 0; i < scopeArr.GetCount(); i++) {
                            if(scopeArr.Item(i).StartsWith(wxT("__anon")) == false) {
                                tmp << scopeArr.Item(i) << wxT("::");
                            }
                        }

                        tmp.EndsWith(wxT("::"), &new_val);
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

    if(kind == "enumerator" && extFields.count("enum")) {
        // Remove the last parent
        wxString& scope = extFields["enum"];
        size_t where = scope.rfind("::");
        if(where != wxString::npos) {
            scope = scope.Mid(0, where);
        } else {
            // Global enum, remove this ext field
            extFields.erase("enum");
        }
    }

    //    if(kind == wxT("enumerator")) {
    //        // enums are specials, they are a scope, when they declared as "enum class ..." (C++11),
    //        // but not a scope when declared as "enum ...". So, for "enum class ..." declaration
    //        //(and anonymous enums) we appear enumerators when typed:
    //        // enumName::
    //        // Is global scope there aren't appears. For "enum ..." declaration we appear
    //        // enumerators when typed:
    //        // enumName::
    //        // and when it global (or same namespace) scope.
    //        wxStringMap_t::iterator enumField = extFields.find(wxT("enum"));
    //        if(enumField != extFields.end()) {
    //            wxString enumName = enumField->second;
    //            bool isAnonymous = enumName.AfterLast(wxT(':')).StartsWith(wxT("__anon"));
    //
    //            bool isInEnumNamespace = false;
    //            wxStringMap_t::const_iterator isInEnumNamespaceField = extFields.find(wxT("isInEnumNamespace"));
    //            if(isInEnumNamespaceField != extFields.end()) {
    //                wxString isInEnumNamespaceValue = isInEnumNamespaceField->second;
    //                isInEnumNamespace = isInEnumNamespaceValue.AfterLast(wxT(':')) == wxT("1") ? true : false;
    //            }
    //
    //            if(!isInEnumNamespace) {
    //                enumField->second = enumField->second.BeforeLast(wxT(':')).BeforeLast(wxT(':'));
    //                if(!isAnonymous) {
    //                    extFields[wxT("typeref")] = enumName;
    //                }
    //            }
    //        }
    //    }

    this->Create(fileName, name, lineNumber, pattern, kind, extFields);
}

bool TagEntry::IsConstructor() const
{
    if(GetKind() != wxT("function") && GetKind() != wxT("prototype"))
        return false;

    return GetName() == GetScope();
}

bool TagEntry::IsDestructor() const
{
    if(GetKind() != wxT("function") && GetKind() != wxT("prototype"))
        return false;

    return GetName().StartsWith(wxT("~"));
}

wxString TagEntry::GetReturnValue() const
{
    wxString returnValue = GetExtField(_T("returns"));
    returnValue.Trim().Trim(false);
    returnValue.Replace(wxT("virtual"), wxT(""));
    return returnValue;
}

bool TagEntry::IsFunction() const { return GetKind() == wxT("function"); }

bool TagEntry::IsMethod() const { return IsPrototype() || IsFunction(); }

bool TagEntry::IsPrototype() const { return GetKind() == wxT("prototype"); }

bool TagEntry::IsClass() const { return GetKind() == wxT("class"); }

bool TagEntry::IsMacro() const { return GetKind() == wxT("macro"); }

bool TagEntry::IsStruct() const { return GetKind() == wxT("struct"); }

bool TagEntry::IsScopeGlobal() const { return GetScope().IsEmpty() || GetScope() == wxT("<global>"); }

bool TagEntry::IsTypedef() const { return GetKind() == wxT("typedef"); }

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
        case wxT('<'):
            if(depth == 0 && parent.IsEmpty() == false) {
                parent.Trim().Trim(false);
                parentsArr.Add(parent);
                parent.Clear();
            }
            depth++;
            break;

        case wxT('>'):
            depth--;
            break;

        case wxT(','):
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
        case wxT('<'):
            depth++;
            parent << ch;
            break;

        case wxT('>'):
            depth--;
            parent << ch;
            break;

        case wxT(','):
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

int TagEntry::CompareDisplayString(const TagEntryPtr& rhs) const
{
    wxString d1, d2;

    d1 << GetReturnValue() << wxT(": ") << GetFullDisplayName() << wxT(":") << GetAccess();
    d2 << rhs->GetReturnValue() << wxT(": ") << rhs->GetFullDisplayName() << wxT(":") << rhs->GetAccess();
    return d1.Cmp(d2);
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
    if(p.StartsWith(wxT("/^"))) {
        p.Replace(wxT("/^"), wxT(""));
    }

    if(p.EndsWith(wxT("$/"))) {
        p.Replace(wxT("$/"), wxT(""));
    }
    return p;
}

wxString TagEntry::FormatComment()
{
    // Add comment section
    wxString tagComment;
    if(!GetFile().IsEmpty()) {

        CommentParseResult comments;
        ::ParseComments(GetFile().mb_str(wxConvUTF8).data(), comments);

        // search for comment in the current line, the line above it and 2 above it
        // use the first match we got
        for(size_t i = 0; i < 3; i++) {
            wxString comment = comments.getCommentForLine(GetLine() - i);
            if(!comment.IsEmpty()) {
                SetComment(comment);
                break;
            }
        }
    }

    CompletionHelper helper;
    tagComment = helper.format_comment(const_cast<TagEntry*>(this), GetComment());

    wxString return_value;
    return_value << " -> `" << GetReturnValue() << "`\n---\n";
    tagComment.Prepend(return_value);
    return tagComment;
}

bool TagEntry::IsLocalVariable() const { return GetKind() == "local"; }

wxString TagEntry::GetLocalType() const { return GetExtField("type"); }
