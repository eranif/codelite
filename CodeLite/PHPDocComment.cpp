#include "PHPDocComment.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <set>

PHPDocComment::PHPDocComment(PHPSourceFile& sourceFile, const wxString& comment)
    : m_comment(comment)
{
    std::set<wxString> nativeTypes;
    nativeTypes.insert("int");
    nativeTypes.insert("integer");
    nativeTypes.insert("real");
    nativeTypes.insert("double");
    nativeTypes.insert("float");
    nativeTypes.insert("string");
    nativeTypes.insert("binary");
    nativeTypes.insert("array");
    nativeTypes.insert("object");
    nativeTypes.insert("bool");
    nativeTypes.insert("boolean");
    nativeTypes.insert("mixed");
    nativeTypes.insert("null");

    static wxRegEx reReturnStatement(wxT("@(return)[ \t]+([\\a-zA-Z_]{1}[\\|\\a-zA-Z0-9_]*)"));
    if(reReturnStatement.IsValid() && reReturnStatement.Matches(m_comment)) {
        wxString returnValue = reReturnStatement.GetMatch(m_comment, 2);
        wxArrayString types = ::wxStringTokenize(returnValue, "|", wxTOKEN_STRTOK);
        if(types.size() > 1) {
            // Multiple return types, guess the best match
            wxString bestMatch;
            for(size_t i = 0; i < types.size(); ++i) {
                if(bestMatch.IsEmpty() && (nativeTypes.count(types.Item(i)) == 0)) {
                    bestMatch = types.Item(i);
                    break;
                }
            }

            if(bestMatch.IsEmpty()) {
                bestMatch = types.Item(0); // just get the first match
            }
            m_returnValue = sourceFile.MakeIdentifierAbsolute(bestMatch);
        } else if(types.size() == 1) {
            m_returnValue = sourceFile.MakeIdentifierAbsolute(types.Item(0));
        }
    }

    static wxRegEx reVarType(wxT("@(var|variable)[ \t]+([\\a-zA-Z_]{1}[\\a-zA-Z0-9_]*)"));
    if(reVarType.IsValid() && reVarType.Matches(m_comment)) {
        m_varType = reVarType.GetMatch(m_comment, 2);
        m_varType = sourceFile.MakeIdentifierAbsolute(m_varType);
        m_varName.Clear();
    }

    static wxRegEx reVarType2(wxT("@(var|variable)[ \t]+([\\$]{1}[\\a-zA-Z0-9_]*)[ \t]+([\\a-zA-Z0-9_]+)"));
    if(reVarType2.IsValid() && reVarType2.Matches(m_comment)) {
        m_varType = reVarType2.GetMatch(m_comment, 3);
        m_varType = sourceFile.MakeIdentifierAbsolute(m_varType);
        m_varName = reVarType2.GetMatch(m_comment, 2);
    }

    // @param PDO $name
    static wxRegEx reParam2(wxT("@(param|parameter)[ \t]+([\\a-zA-Z0-9_]*)[ \t]+([\\$]{1}[\\a-zA-Z0-9_]+)"));
    wxArrayString lines2 = wxStringTokenize(m_comment, wxT("\n"), wxTOKEN_STRTOK);
    if(reParam2.IsValid()) {
        for(size_t i = 0; i < lines2.GetCount(); i++) {
            wxString line = lines2.Item(i).Trim().Trim(false);
            if(reParam2.Matches(line)) {
                wxString paramName, paramHint;
                paramHint = sourceFile.MakeIdentifierAbsolute(reParam2.GetMatch(line, 2));
                paramName = reParam2.GetMatch(line, 3);
                m_paramsArr.Add(paramHint);
                m_params.insert(std::make_pair(paramName, paramHint));
            }
        }
    }
}

PHPDocComment::~PHPDocComment() {}

const wxString& PHPDocComment::GetParam(size_t n) const
{
    if(n >= m_paramsArr.GetCount()) {
        static wxString emptyString;
        return emptyString;
    }
    return m_paramsArr.Item(n);
}

const wxString& PHPDocComment::GetReturn() const { return m_returnValue; }

const wxString& PHPDocComment::GetVar() const { return m_varType; }

const wxString& PHPDocComment::GetParam(const wxString& name) const
{
    if(m_params.count(name) == 0) {
        static wxString emptyString;
        return emptyString;
    }
    return m_params.find(name)->second;
}
