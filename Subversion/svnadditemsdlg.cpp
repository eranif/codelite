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
		m_configTool->ReadObject(wxT("SvnIgnorePatternData"), &m_data);
	}

	m_textIgnoreFilePatterns->SetValue(m_data.GetIgnorePattern());

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
	m_configTool->WriteObject(wxT("SvnIgnorePatternData"), &m_data);
	EndModal(wxID_OK);
}

void SvnAddItemsDlg::OnApplyPattern(wxCommandEvent &e)
{
	wxUnusedVar(e);
	m_data.SetIgnorePattern(m_textIgnoreFilePatterns->GetValue());
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

//-------------------------------------------------
// Serialized data object
//-------------------------------------------------

SvnIgnorePatternData::SvnIgnorePatternData()
: m_pattern(wxT("*.o;*.obj;*.exe;*.lib;*.so;*.dll;*.a;*.dynlib;*.exp;*.ilk;*.pdb;*.d;*.tags;*.suo;*.ncb;"))
{
}

SvnIgnorePatternData::~SvnIgnorePatternData()
{
}

void SvnIgnorePatternData::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_pattern"), m_pattern);
}

void SvnIgnorePatternData::Serialize(Archive &arch)
{
	arch.Write(wxT("m_pattern"), m_pattern);
}

