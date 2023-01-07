#include "DeleteCustomControlDlg.h"
#include "wxc_settings.h"
#include <windowattrmanager.h>
#include <wx/arrstr.h>
#include <wx/msgdlg.h>

DeleteCustomControlDlg::DeleteCustomControlDlg(wxWindow* parent)
    : DeleteCustomControlDlgBaseClass(parent)
{
    DoPopulate();
    SetName("DeleteCustomControlDlg");
    WindowAttrManager::Load(this);
}

DeleteCustomControlDlg::~DeleteCustomControlDlg() {}

void DeleteCustomControlDlg::OnDeleteUI(wxUpdateUIEvent& event) { event.Enable(!m_controlsToDelete.IsEmpty()); }

void DeleteCustomControlDlg::OnDeleteControls(wxCommandEvent& event)
{
    wxString msg;
    msg << _("Are you sure you want to delete the following custom controls:\n");

    for(size_t i = 0; i < m_controlsToDelete.GetCount(); ++i) {
        msg << m_controlsToDelete.Item(i) << "\n";
    }

    if(::wxMessageBox(msg, wxT("wxCrafter"), wxICON_QUESTION | wxCENTER | wxYES_NO) == wxYES) {
        for(size_t i = 0; i < m_controlsToDelete.GetCount(); ++i) {
            wxcSettings::Get().DeleteCustomControl(m_controlsToDelete.Item(i));
        }
        wxcSettings::Get().Save();
        DoPopulate();
    }
}

void DeleteCustomControlDlg::OnItemValueChanged(wxDataViewEvent& event)
{
    unsigned row = m_dvListCtrl->GetStore()->GetRow(event.GetItem());
    wxString controlName = m_dvListCtrl->GetTextValue(row, 1);

    wxVariant v;
    m_dvListCtrl->GetValue(v, row, 0);

    if(v.GetBool()) {
        if(m_controlsToDelete.Index(controlName) == wxNOT_FOUND) m_controlsToDelete.Add(controlName);

    } else {
        int where = m_controlsToDelete.Index(controlName);
        if(where != wxNOT_FOUND) m_controlsToDelete.RemoveAt(where);
    }
}

void DeleteCustomControlDlg::DoPopulate()
{
    m_dvListCtrl->DeleteAllItems();
    m_controlsToDelete.Clear();
    wxArrayString controlsArr;
    const CustomControlTemplateMap_t& m = wxcSettings::Get().GetTemplateClasses();
    CustomControlTemplateMap_t::const_iterator iter = m.begin();
    for(; iter != m.end(); ++iter) {
        wxVector<wxVariant> cols;
        cols.push_back(false); // do not delete
        cols.push_back(iter->second.GetClassName());
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)NULL);
    }
}
