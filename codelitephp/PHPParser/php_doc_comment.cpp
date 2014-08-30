#include "php_doc_comment.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>

PHPDocComment::PHPDocComment(const wxString& comment)
    : m_comment(comment)
{
    static wxRegEx reReturnStatement(wxT("@(return)[ \t]+([\\a-zA-Z_]{1}[\\a-zA-Z0-9_]*)"));
    if(reReturnStatement.IsValid() && reReturnStatement.Matches(m_comment)) {
        m_returnValue = reReturnStatement.GetMatch(m_comment, 2);
    }

    static wxRegEx reVarType(wxT("@(var|variable)[ \t]+([\\a-zA-Z_]{1}[\\a-zA-Z0-9_]*)"));
    if(reVarType.IsValid() && reVarType.Matches(m_comment)) {
        m_varType = reVarType.GetMatch(m_comment, 2);
    }
    
    static wxRegEx reVarType2(wxT("@(var|variable)[ \t]+([\\$]{1}[\\a-zA-Z0-9_]*)[ \t]+([\\a-zA-Z0-9_]+)"));
    if(reVarType2.IsValid() && reVarType2.Matches(m_comment)) {
        m_varType = reVarType2.GetMatch(m_comment, 3);
    }
    
    // @param $name PDO
    static wxRegEx reParam(wxT("@(param|parameter)[ \t]+([\\$]{1}[\\a-zA-Z0-9_]*)[ \t]+([\\a-zA-Z0-9_]+)"));
    wxArrayString lines = wxStringTokenize(m_comment, wxT("\n"), wxTOKEN_STRTOK);
    if(reParam.IsValid()) {
        for(size_t i=0; i<lines.GetCount(); i++) { 
            wxString line = lines.Item(i).Trim().Trim(false);
            if(reParam.Matches(line)) {
                m_params.Add(reParam.GetMatch(line, 3));
            }
        }
    } 
    
    // @param PDO $name
    static wxRegEx reParam2(wxT("@(param|parameter)[ \t]+([\\a-zA-Z0-9_]*)[ \t]+([\\$]{1}[\\a-zA-Z0-9_]+)"));
    wxArrayString lines2 = wxStringTokenize(m_comment, wxT("\n"), wxTOKEN_STRTOK);
    if(reParam2.IsValid()) {
        for(size_t i=0; i<lines2.GetCount(); i++) {
            wxString line = lines2.Item(i).Trim().Trim(false);
            if(reParam2.Matches(line)) {
                m_params.Add(reParam2.GetMatch(line, 2));
            }
        }
    }
}

PHPDocComment::~PHPDocComment()
{
}

const wxString& PHPDocComment::getParam(size_t n) const
{
    if(n >= m_params.GetCount()) {
        static wxString emptyString;
        return emptyString;
    }
    return m_params.Item(n);
}

const wxString& PHPDocComment::getReturn() const
{
    return m_returnValue;
}

const wxString& PHPDocComment::getVar() const
{
    return m_varType;
}
