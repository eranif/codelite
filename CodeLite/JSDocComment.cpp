#include "JSDocComment.h"
#include "JSFunction.h"
#include <wx/regex.h>
#include <wx/tokenzr.h>

JSDocComment::JSDocComment() {}

JSDocComment::~JSDocComment() {}

void JSDocComment::ProcessVarDoc(JSObject::Ptr_t obj)
{
    static wxRegEx reVarType(wxT("@(var|variable)[ \t]+([$a-zA-Z_]+)[ \t]+([$a-zA-Z_]+)"));
    static wxRegEx reVarType2(wxT("@(var|variable)[ \t]+([$a-zA-Z_]+)"));
    if(reVarType.IsValid() && reVarType.Matches(obj->GetComment())) {
        wxString type = reVarType.GetMatch(obj->GetComment(), 2);
        obj->AddType(type, true);
    } else if(reVarType2.IsValid() && reVarType2.Matches(obj->GetComment())) {
        wxString type = reVarType2.GetMatch(obj->GetComment(), 2);
        obj->AddType(type, true);
    }
}

void JSDocComment::ProcessFunction(JSObject::Ptr_t func)
{
    if(!func->IsFunction()) return;
    
    JSObject::Map_t &variables = func->As<JSFunction>()->GetVariables();
    
    // @param Array arr
    const wxString& comment = func->GetComment();
    
    static wxRegEx reParam(wxT("@(param|parameter)[ \t]+([$\\a-zA-Z0-9_]+)[ \t]+([$\\a-zA-Z0-9_]+)"));
    wxArrayString lines = ::wxStringTokenize(comment, wxT("\n"), wxTOKEN_STRTOK);
    
    if(!variables.empty() && reParam.IsValid()) {
        // parse @param syntax
        for(size_t i = 0; i < lines.GetCount(); ++i) {
            wxString line = lines.Item(i).Trim().Trim(false);
            if(reParam.Matches(line)) {
                wxString paramHint = reParam.GetMatch(line, 2);
                wxString paramName = reParam.GetMatch(line, 3);
                JSObject::Map_t::iterator iter = variables.find(paramName);
                if(iter != variables.end()) {
                    iter->second->AddType(paramHint, true);
                }
            }
        }
    }
    
    // Parse @return Array
    static wxRegEx reReturnStatement(wxT("@(return)[ \t]+([$\\a-zA-Z0-9_]*)"));
    if(reReturnStatement.IsValid() && reReturnStatement.Matches(comment)) {
       func->AddType(reReturnStatement.GetMatch(comment, 2), true);
    }
}

void JSDocComment::Process(JSObject::Ptr_t obj)
{
    if(obj->IsFunction()) {
        ProcessFunction(obj);
    } else {
        ProcessVarDoc(obj);
    }
}
