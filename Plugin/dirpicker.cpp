#include "dirpicker.h"
#include "wx/sizer.h"
#include "macros.h"
#include <wx/dirdlg.h>

BEGIN_EVENT_TABLE(DirPicker, wxPanel)
EVT_TEXT(wxID_ANY, DirPicker::OnText)
END_EVENT_TABLE()

DirPicker::DirPicker(wxWindow *parent, wxWindowID id, const wxString &buttonCaption, const wxString &defaultPos, const wxString &message, const wxPoint& pos, const wxSize& size, long style)
: wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER)
, m_path(NULL)
, m_combo(NULL)
, m_buttonCaption(buttonCaption)
, m_dlgCaption(message)
, m_style(style)
, m_defaultPos(defaultPos)
{
	CreateControls();
	ConnectEvents();
}

DirPicker::~DirPicker()
{
}

void DirPicker::OnText(wxCommandEvent &event)
{
	GetParent()->ProcessEvent(event);
}

void DirPicker::CreateControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(mainSizer);

	if(m_style & wxDP_USE_TEXTCTRL){
		m_path = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
		mainSizer->Add(m_path, 1, wxLEFT | wxTOP | wxBOTTOM, 0);
	}else{
		m_combo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
		mainSizer->Add(m_combo, 1, wxLEFT | wxTOP | wxBOTTOM, 0);
	}

	m_button = new wxButton(this, wxID_ANY, m_buttonCaption, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	mainSizer->Add(m_button, 0, wxTOP | wxBOTTOM, 0);

	Layout();
}

void DirPicker::ConnectEvents()
{
	ConnectButton(m_button, DirPicker::OnButtonClicked);
}

void DirPicker::OnButtonClicked(wxCommandEvent &event)
{
	wxUnusedVar(event);

	if(m_defaultPos.IsEmpty())
	{
		m_defaultPos = wxGetCwd();
	}

	wxDirDialog *dlg = new wxDirDialog(this, m_dlgCaption, m_defaultPos);
	if(dlg->ShowModal() == wxID_OK)
	{
		// Get the dirname
		wxString path = dlg->GetPath();
		SetPath(path);
	}
	dlg->Destroy();
}

wxString DirPicker::GetPath()
{
	if(m_style & wxDP_USE_TEXTCTRL)
		return m_path->GetValue();
	else{
		wxString path(m_combo->GetValue());
		//if the path is not whitin the combobox list, add it
		if(m_combo->FindString(path) == wxNOT_FOUND){
			SetPath(path);
		}
		return path;
	}
}

void DirPicker::SetPath(const wxString &path)
{
	if(m_style & wxDP_USE_TEXTCTRL)
		m_path->SetValue(path);
	else
	{
		int where = m_combo->FindString(path);
		if(where != wxNOT_FOUND)
		{
			m_combo->SetSelection(where);
		}
		else
		{
			where = m_combo->Append(path);
			m_combo->SetSelection(where);
		}
	}
}

void DirPicker::SetValues(const wxArrayString &values, int sel)
{
	wxASSERT_MSG(m_style & wxDP_USE_COMBOBOX, wxT("SetValues is available only for wxDP_USE_COMBOBOX style"));
	m_combo->Append(values);
	m_combo->SetSelection(sel);
}
