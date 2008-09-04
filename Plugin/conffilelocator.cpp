#include "conffilelocator.h"
#include <wx/filename.h>

ConfFileLocator* ConfFileLocator::ms_instance = 0;

ConfFileLocator::ConfFileLocator()
{
}

ConfFileLocator::~ConfFileLocator()
{
}

ConfFileLocator* ConfFileLocator::Instance()
{
	if(ms_instance == 0){
		ms_instance = new ConfFileLocator();
	}
	return ms_instance;
}

void ConfFileLocator::Release()
{
	if(ms_instance){
		delete ms_instance;
	}
	ms_instance = 0;
}

void ConfFileLocator::Initialize(const wxString& installpath, const wxString& startuppath)
{
	m_installPath = installpath;
	m_startupPath = startuppath;
}

wxString ConfFileLocator::Locate(const wxString& baseName)
{
	wxFileName privateFile(GetLocalCopy(baseName));
	wxFileName defaultFile(GetDefaultCopy(baseName));
	
	if(privateFile.FileExists()){
		return privateFile.GetFullPath();
	} else {
		return defaultFile.GetFullPath();
	}
}

wxString ConfFileLocator::GetLocalCopy(const wxString& baseName)
{
	return m_startupPath + wxT("/") + baseName;
}

wxString ConfFileLocator::GetDefaultCopy(const wxString& baseName)
{
	return m_installPath + wxT("/") + baseName + wxT(".default");
}
