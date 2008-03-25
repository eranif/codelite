#include "pluginwizard.h"
#include <wx/xrc/xmlres.h>
#include "pluginwizard_pag2.h"
#include "pluginwizard_page1.h"
#include "newplugindata.h"

PluginWizard::PluginWizard(wxWindow *parent, wxWindowID id)
{
	wxBitmap bmp = wxXmlResource::Get()->LoadBitmap(wxT("new_plugin_wiz_bmp"));
	wxWizard::Create(parent, id, wxT("New Plugin Wizard"), bmp);

	//create the pages
	m_page1 = new PluginWizardPage1(this);
	m_page2 = new PluginWizardPage2(this);

	//chain the pages
	wxWizardPageSimple::Chain(m_page1, m_page2);
}

PluginWizard::~PluginWizard()
{ 
}

bool PluginWizard::Run(NewPluginData &data)
{
	wxSize sz1 = m_page1->GetSizer()->CalcMin();
	wxSize sz2 = m_page2->GetSizer()->CalcMin();

	wxSize maxSize = sz1;
	if(maxSize.GetWidth() < sz2.GetWidth()) maxSize = sz2;
	if(maxSize.GetWidth() < 400){
		maxSize.SetWidth(400);
	}
	
	SetPageSize(maxSize);
	if(RunWizard(m_page1)){
		data.SetPluginName(((PluginWizardPage1*) m_page1)->GetName());
		data.SetPluginDescription(((PluginWizardPage1*) m_page1)->GetDescription());
		data.SetProjectPath(((PluginWizardPage2*) m_page2)->GetProjectPath());
		data.SetCodelitePath(((PluginWizardPage2*) m_page2)->GetCodelitePath());
		return true;
	}
	return false;
}


