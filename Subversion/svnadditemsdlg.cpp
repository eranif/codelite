//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svnadditemsdlg.cpp              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
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
 #include "svnadditemsdlg.h"
#include "iconfigtool.h"
#include "wx/tokenzr.h"

SvnAddItemsDlg::SvnAddItemsDlg( wxWindow* parent, const wxArrayString &files, IConfigTool *confTool )
: SvnAddItemsBaseDlg( parent )
, m_configTool(confTool)
, m_files(files)
{
	InitDialog(true);
}


void SvnAddItemsDlg::InitDialog(bool loadPattern)
{
	if(loadPattern){
		m_configTool->ReadObject(wxT("SubversionOptions"), &m_data);
	}

	m_textIgnoreFilePatterns->SetValue(m_data.GetPattern());

	m_checkListFiles->Freeze();
	m_checkListFiles->Clear();
	for(size_t i=0; i<m_files.GetCount(); i++){
		m_checkListFiles->Append(m_files.Item(i));
		
		if(!IsIgnoredFile(m_files.Item(i))){
			m_checkListFiles->Check((unsigned int)i, true);
		}else{
			m_checkListFiles->Check((unsigned int)i, false);
		}
	}
	m_checkListFiles->Thaw();
}

bool SvnAddItemsDlg::IsIgnoredFile(const wxString &file)
{
	wxStringTokenizer tkz(m_textIgnoreFilePatterns->GetValue(), wxT(";"), wxTOKEN_STRTOK);
	while(tkz.HasMoreTokens()){
		if(wxMatchWild(tkz.NextToken(), file)){
			return true;
		}
	}
	return false;
}

void SvnAddItemsDlg::OnButtonCancel(wxCommandEvent &e)
{
	wxUnusedVar(e);
	EndModal(wxID_CANCEL);
}

void SvnAddItemsDlg::OnButtonOK(wxCommandEvent &e)
{
	wxUnusedVar(e);
	//save the file patterns
	m_configTool->WriteObject(wxT("SubversionOptions"), &m_data);
	EndModal(wxID_OK);
}

void SvnAddItemsDlg::OnApplyPattern(wxCommandEvent &e)
{
	wxUnusedVar(e);
	m_data.SetPattern(m_textIgnoreFilePatterns->GetValue());
	InitDialog();
}

wxString SvnAddItemsDlg::GetFiles()
{
	wxString files;
	for(size_t i=0; i<m_checkListFiles->GetCount(); i++){
		if(m_checkListFiles->IsChecked((unsigned int)i)){
			files << wxT(" \"") << m_checkListFiles->GetString((unsigned int)i) << wxT("\" ");
		}
	}
	return files;
}

