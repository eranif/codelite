#include "precompiled_header.h"
#include "cpp_comment_creator.h"
#include <wx/datetime.h>
#include <vector>
#include "language.h"

#define trimMe(str){\
		str = str.Trim();\
		str = str.Trim(false);\
	}

CppCommentCreator::CppCommentCreator(TagEntryPtr tag)
: m_tag(tag)
{
}

CppCommentCreator::~CppCommentCreator()
{
}

wxString CppCommentCreator::CreateComment()
{
	if(m_tag->GetKind() == wxT("class"))
		return ClassComment();
	else if(m_tag->GetKind() == wxT("function"))
		return FunctionComment();
	else if(m_tag->GetKind() == wxT("prototype"))
		return FunctionComment();
	return wxEmptyString;
}

wxString CppCommentCreator::ClassComment()
{
	wxDateTime now = wxDateTime::Now();
	wxString comment;
	comment << wxT("/**\n");
	comment << wxT(" * \\class ") << m_tag->GetName() << wxT("\n");
	comment << wxT(" * \\brief \n");
	comment << wxT(" * \\author ") << wxGetUserName() << wxT("\n");
	comment << wxT(" * \\date ") << now.FormatDate() << wxT("\n");
	comment << wxT(" */\n");
	return comment;
}

wxString CppCommentCreator::FunctionComment()
{
	wxDateTime now = wxDateTime::Now();
	wxString comment;

	//parse the function signature
	std::vector<TagEntryPtr> tags;
	Language *lang = LanguageST::Get();
	lang->GetLocalVariables(m_tag->GetSignature(), tags);
	
	Variable var;
	lang->VariableFromPattern(m_tag->GetPattern(), var);
	
	comment << wxT("/**\n");
	comment << wxT(" * \\brief \n");
	for(size_t i=0; i<tags.size(); i++)
		comment << wxT(" * \\param ") << tags.at(i)->GetName() << wxT("\n");
	
	wxString type = _U(var.m_type.c_str());
	wxString name = _U(var.m_name.c_str());
	trimMe(type);
	trimMe(name);

	if(	type != wxT("void") //void has no return value
		&& name != type){	//constructor
		comment << wxT(" * \\return \n");
	}
	comment << wxT(" */\n");
	return comment;
}
