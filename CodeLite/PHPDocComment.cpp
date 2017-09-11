#include "PHPDocComment.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <set>

PHPDocComment::PHPDocComment(PHPSourceFile& sourceFile, const wxString& comment)
    : m_sourceFile(sourceFile)
    , m_comment(comment)
{
    static std::set<wxString> nativeTypes;
    if(nativeTypes.empty()) {
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
    }

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
    
    // @var Type 
    static wxRegEx reVarType(wxT("@(var|variable)[ \t]+([\\a-zA-Z_]{1}[\\a-zA-Z0-9_]*)"));
    // @var $Name Type
    static wxRegEx reVarType3(wxT("@(var|variable)[ \t]+([$]{1}[\\a-zA-Z0-9_]*)[ \t]+([\\a-zA-Z0-9_]+)"));
    if(reVarType.IsValid() && reVarType.Matches(m_comment)) {
        m_varType = reVarType.GetMatch(m_comment, 2);
        m_varType = sourceFile.MakeIdentifierAbsolute(m_varType);
        m_varName.Clear();
        
    } else if(reVarType3.IsValid() && reVarType3.Matches(m_comment)) {
        m_varType = reVarType3.GetMatch(m_comment, 3);
        m_varType = sourceFile.MakeIdentifierAbsolute(m_varType);
        m_varName = reVarType3.GetMatch(m_comment, 2);
    }

    // @param <TYPE> <NAME>
    if(m_comment.Contains("@param")) {
        static wxRegEx reParam2(wxT("@(param|parameter)[ \t]+([\\a-zA-Z0-9_]*)[ \t]+([\\$]{1}[\\a-zA-Z0-9_]+)"),
                                wxRE_ADVANCED);
        wxArrayString lines2 = ::wxStringTokenize(m_comment, wxT("\n"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines2.GetCount(); i++) {
            wxString line = lines2.Item(i).Trim().Trim(false);
            if(reParam2.IsValid() && reParam2.Matches(line)) {
                wxString paramName, paramHint;
                paramHint = sourceFile.MakeIdentifierAbsolute(reParam2.GetMatch(line, 2));
                paramName = reParam2.GetMatch(line, 3);
                m_paramsArr.Add(paramHint);
                m_params.insert(std::make_pair(paramName, paramHint));
            }
        }
    }

    // @property-read, @property-write, @property
    if(m_comment.Contains("@property")) {
        static wxRegEx reProprety("@property[ \t]+([\\a-zA-Z0-9_]*)[ \t]*([\\$]{1}[\\a-zA-Z0-9_]*)(.*?)",
                                  wxRE_ADVANCED);
        static wxRegEx rePropretyRead("@property\\-read[ \t]+([\\a-zA-Z0-9_]*)[ \t]*([\\$]{1}[\\a-zA-Z0-9_]*)(.*?)",
                                      wxRE_ADVANCED);
        static wxRegEx rePropretyWrite("@property\\-write[ \t]+([\\a-zA-Z0-9_]*)[ \t]*([\\$]{1}[\\a-zA-Z0-9_]*)(.*?)",
                                       wxRE_ADVANCED);
        wxArrayString lines2 = ::wxStringTokenize(m_comment, wxT("\n"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines2.GetCount(); i++) {
            wxString line = lines2.Item(i).Trim().Trim(false);
            if(reProprety.IsValid() && reProprety.Matches(line)) {
                PHPDocComment::Property prop;
                prop.type = sourceFile.MakeIdentifierAbsolute(reProprety.GetMatch(line, 1));
                prop.name = reProprety.GetMatch(line, 2);
                prop.desc = reProprety.GetMatch(line, 3);
                m_properties.insert(std::make_pair(prop.name, prop));

            } else if(rePropretyRead.IsValid() && rePropretyRead.Matches(line)) {
                PHPDocComment::Property prop;
                prop.type = sourceFile.MakeIdentifierAbsolute(rePropretyRead.GetMatch(line, 1));
                prop.name = rePropretyRead.GetMatch(line, 2);
                prop.desc = rePropretyRead.GetMatch(line, 3);
                m_properties.insert(std::make_pair(prop.name, prop));

            } else if(rePropretyWrite.IsValid() && rePropretyWrite.Matches(line)) {
                PHPDocComment::Property prop;
                prop.type = sourceFile.MakeIdentifierAbsolute(rePropretyWrite.GetMatch(line, 1));
                prop.name = rePropretyWrite.GetMatch(line, 2);
                prop.desc = rePropretyWrite.GetMatch(line, 3);
                m_properties.insert(std::make_pair(prop.name, prop));
            }
        }
    }

    // Attempt to parse and resolve @method entries in the PHPDoc
    ProcessMethods();
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

void PHPDocComment::ProcessMethods()
{
    if(!m_comment.Contains("@method")) return;
    wxArrayString lines2 = ::wxStringTokenize(m_comment, wxT("\n"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines2.GetCount(); ++i) {
        wxString ll = lines2.Item(i).Trim().Trim(false);
        if(ll.Contains("@method")) {
            ProcessMethod(ll);
        }
    }
}

void PHPDocComment::ProcessMethod(wxString& strLine)
{
    // The phpdoc for method does not confirm to the PHP syntax.
    // We need to alter the signature so we can use our parse to parse
    // the signature
    // @method syntax is:
    // @method [return type] [name]([[type] [parameter]<, ...>]) [<description>]
    // While PHP's syntax is:
    // function [name] ([[type] [parameter]<, ...>]) [ : return_type ]

    static wxRegEx reMethodWithReturnType("@method[ \t]+([\\a-zA-Z0-9_]+)[\t ]+([\\a-zA-Z0-9_]+)[ \t]*(\\(.*?\\))",
                                          wxRE_ADVANCED);
    static wxRegEx reMethodNoReturnType("@method[ \t]+([\\a-zA-Z0-9_]+)[ \t]*(\\(.*?\\))", wxRE_ADVANCED);

    wxString returnType;
    wxString methodName;
    wxString signature;
    if(reMethodWithReturnType.IsValid() && reMethodWithReturnType.Matches(strLine)) {
        returnType = reMethodWithReturnType.GetMatch(strLine, 1);
        methodName = reMethodWithReturnType.GetMatch(strLine, 2);
        signature = reMethodWithReturnType.GetMatch(strLine, 3);
    } else if(reMethodNoReturnType.IsValid() && reMethodNoReturnType.Matches(strLine)) {
        methodName = reMethodNoReturnType.GetMatch(strLine, 1);
        signature = reMethodNoReturnType.GetMatch(strLine, 2);
    }

    wxString strBuffer;
    strBuffer << "<?php function " << methodName << signature;
    if(!returnType.IsEmpty()) {
        strBuffer << " : " << returnType << " ";
    }
    strBuffer << " {} ";

    PHPSourceFile buffer(strBuffer);
    buffer.SetTypeAbsoluteConverter(&m_sourceFile);
    buffer.Parse();

    if(!buffer.CurrentScope()->GetChildren().empty()) {
        PHPEntityBase::Ptr_t func = *buffer.CurrentScope()->GetChildren().begin();
        if(func && func->Is(kEntityTypeFunction)) {
            if(func->Parent()) {
                func->Parent()->RemoveChild(func);
            }
            m_methods.push_back(func);
        }
    }
}
