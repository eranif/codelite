#include "PHPDocComment.h"
#include "PHPDocParam.h"
#include "PHPDocProperty.h"
#include "PHPDocVar.h"
#include "wxStringHash.h"
#include <algorithm>
#include <wx/regex.h>
#include <wx/tokenzr.h>

PHPDocComment::PHPDocComment(PHPSourceFile& sourceFile, const wxString& comment)
    : m_sourceFile(sourceFile)
    , m_comment(comment)
    , m_returnNullable(false)
{
    static std::unordered_set<wxString> nativeTypes;
    if(nativeTypes.empty()) {
        // List taken from https://www.php.net/manual/en/language.types.intro.php
        // Native types
        nativeTypes.insert("bool");
        nativeTypes.insert("int");
        nativeTypes.insert("float");
        nativeTypes.insert("string");
        nativeTypes.insert("array");
        nativeTypes.insert("object");
        nativeTypes.insert("iterable");
        nativeTypes.insert("callable");
        nativeTypes.insert("null");
        nativeTypes.insert("mixed");
        nativeTypes.insert("void");
        // Types that are common in documentation
        nativeTypes.insert("boolean");
        nativeTypes.insert("integer");
        nativeTypes.insert("double");
        nativeTypes.insert("real");
        nativeTypes.insert("binery");
        nativeTypes.insert("resource");
        nativeTypes.insert("number");
        nativeTypes.insert("callback");
    }

    static wxRegEx reReturnStatement(wxT("@(return)[ \t]+([\\?\\a-zA-Z_]{1}[\\|\\a-zA-Z0-9_]*)"));
    if(reReturnStatement.IsValid() && reReturnStatement.Matches(m_comment)) {
        wxString returnValue = reReturnStatement.GetMatch(m_comment, 2);
        if(returnValue.StartsWith("?")) {
            returnValue.Remove(0, 1);
            m_returnNullable = true;
        }

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

    PHPDocVar var(sourceFile, m_comment);
    if(var.IsOk()) {
        m_varType = var.GetType();
        m_varName = var.GetName();
    }

    // @param <TYPE> <NAME>
    if(m_comment.Contains("@param")) {
        PHPDocParam params(sourceFile, m_comment);
        const PHPDocParam::Vec_t& paramsVec = params.Parse();
        std::for_each(paramsVec.begin(), paramsVec.end(), [&](const std::pair<wxString, wxString>& p) {
            m_paramsArr.Add(p.second);
            m_params.insert(p);
        });
    }

    // @property-read, @property-write, @property
    if(m_comment.Contains("@property")) {
        PHPDocProperty prop(sourceFile, m_comment);
        const PHPDocProperty::Tuple_t& properties = prop.ParseParams();
        std::for_each(properties.begin(), properties.end(), [&](const PHPDocProperty::Tuple_t::value_type& vt) {
            PHPDocComment::Property property;
            property.type = std::get<0>(vt);
            property.name = std::get<1>(vt);
            property.desc = std::get<2>(vt);
            m_properties.insert(std::make_pair(property.name, property));
        });
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

const bool PHPDocComment::IsReturnNullable() const { return m_returnNullable; }

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
    // The phpdoc for method does not conform to the PHP syntax.
    // We need to alter the signature so we can use our parse to parse
    // the signature
    // @method syntax is:
    // @method [return type] [name]([[type] [parameter]<, ...>]) [<description>]
    // While PHP's syntax is:
    // function [name] ([[type] [parameter]<, ...>]) [ : return_type ]
    PHPDocProperty property(m_sourceFile, m_comment);
    const PHPDocProperty::Tuple_t& methods = property.ParseMethods();
    std::for_each(methods.begin(), methods.end(), [&](const PHPDocProperty::Tuple_t::value_type& vt) {
        wxString returnType = std::get<0>(vt);
        wxString methodName = std::get<1>(vt);
        wxString signature = std::get<2>(vt);

        wxString strBuffer;
        strBuffer << "<?php function " << methodName << signature;
        if(!returnType.IsEmpty()) {
            strBuffer << " : " << returnType << " ";
        }
        strBuffer << " {} ";

        PHPSourceFile buffer(strBuffer, NULL);
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
    });
}
