#include "view.h"

XS_IMPLEMENT_CLONABLE_CLASS(View,xsSerializable);

View::View():xsSerializable()
{
	initSerializable();
}
View::View(const View& obj):xsSerializable(obj)
{
	m_name = obj.m_name;
	m_parentName = obj.m_parentName;
	m_select = obj.m_select;
	initSerializable();
}

View::View(IDbAdapter* dbAdapter,const wxString& name, const wxString& parentName, const wxString& select):xsSerializable()
{
	m_name = name;
	m_parentName = parentName;
	m_select = select;
	m_pDbAdapter = dbAdapter;
	initSerializable();
}
View::~View()
{
}



void View::initSerializable()
{
	XS_SERIALIZE(m_name,wxT("m_name")); 
	XS_SERIALIZE(m_parentName,wxT("m_parentName")); 
	XS_SERIALIZE(m_select,wxT("m_select")); 
}

