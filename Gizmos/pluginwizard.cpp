//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : pluginwizard.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
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


