#include "commentconfigdata.h"

CommentConfigData::CommentConfigData()
		: m_addStarOnCComment(true)
		, m_continueCppComment(false)
		, m_useSlash2Stars(true)
		, m_useShtroodel(false)
{
	m_classPattern << wxT(" * \\class $(Name)\n");
	m_classPattern << wxT(" * \\author $(User)\n");
	m_classPattern << wxT(" * \\date $(Date)\n");
	m_classPattern << wxT(" * \\file $(CurrentFileName).$(CurrentFileExt)\n");
	m_classPattern << wxT(" * \\brief \n");

	m_functionPattern << wxT(" * \\brief \n");
}

CommentConfigData::~CommentConfigData()
{
}

void CommentConfigData::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_addStarOnCComment"), m_addStarOnCComment);
	arch.Read(wxT("m_continueCppComment"), m_continueCppComment);
	arch.Read(wxT("m_useSlash2Stars"), m_useSlash2Stars);
	arch.Read(wxT("m_useShtroodel"), m_useShtroodel);
	
	arch.Read(wxT("m_classPattern"), m_classPattern);
	m_classPattern.Replace(wxT("|"), wxT("\n"));
	
	arch.Read(wxT("m_functionPattern"), m_functionPattern);
	m_functionPattern.Replace(wxT("|"), wxT("\n"));
}

void CommentConfigData::Serialize(Archive &arch)
{
	arch.Write(wxT("m_addStarOnCComment"), m_addStarOnCComment);
	arch.Write(wxT("m_continueCppComment"), m_continueCppComment);
	arch.Write(wxT("m_useSlash2Stars"), m_useSlash2Stars);
	arch.Write(wxT("m_useShtroodel"), m_useShtroodel);
	
	m_classPattern.Replace(wxT("\n"), wxT("|"));
	arch.Write(wxT("m_classPattern"), m_classPattern);
	m_functionPattern.Replace(wxT("\n"), wxT("|"));
	arch.Write(wxT("m_functionPattern"), m_functionPattern);
}
