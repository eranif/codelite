#include "EditCustomControlDlg.h"
#include "NewCustomEventDlg.h"
#include "wxc_settings.h"
#include <windowattrmanager.h>
#include <wx/app.h>
#include "wxgui_helpers.h"

EditCustomControlDlg::EditCustomControlDlg(wxWindow* parent)
    : EditCustomControlDlgBaseClass(parent)
    , m_isModified(false)
{
    const CustomControlTemplateMap_t& c = wxcSettings::Get().GetTemplateClasses();
    CustomControlTemplateMap_t::const_iterator iter = c.begin();
    for(; iter != c.end(); ++iter) {
        m_choiceControls->Append(iter->first);
    }

    if(c.empty() == false) {
        m_choiceControls->SetSelection(0);

        wxCommandEvent dummy;
        OnSelectControl(dummy);
    }
    SetName("EditCustomControlDlg");
    WindowAttrManager::Load(this);
}

EditCustomControlDlg::~EditCustomControlDlg() {}

void EditCustomControlDlg::OnSelectControl(wxCommandEvent& event)
{
    if(m_isModified) { OnSave(event); }

    wxString controlName = m_choiceControls->GetStringSelection();
    CustomControlTemplate cct = wxcSettings::Get().FindByControlName(controlName);
    if(cct.IsValid()) {
        m_textCtrlInstantiationLine->ChangeValue(cct.GetAllocationLine());
        m_textCtrlIncludeFile->ChangeValue(cct.GetIncludeFile());
        m_textCtrlXRCClass->ChangeValue(cct.GetXrcPreviewClass());

        m_dvListCtrlEvents->DeleteAllItems();
        const wxStringMap_t& events = cct.GetEvents();
        wxStringMap_t::const_iterator iter = events.begin();
        for(; iter != events.end(); ++iter) {
            wxVector<wxVariant> cols;
            cols.push_back(iter->first);
            cols.push_back(iter->second);
            m_dvListCtrlEvents->AppendItem(cols);
        }
    }
    m_isModified = false;
}

void EditCustomControlDlg::OnSave(wxCommandEvent& event)
{
    CustomControlTemplate cct;
    cct.SetClassName(m_choiceControls->GetStringSelection());
    cct.SetAllocationLine(m_textCtrlInstantiationLine->GetValue());
    cct.SetIncludeFile(m_textCtrlIncludeFile->GetValue());
    cct.SetXrcPreviewClass(m_textCtrlXRCClass->GetValue());

    wxStringMap_t events;
    int count = m_dvListCtrlEvents->GetItemCount();
    for(int i = 0; i < count; ++i) {
        wxString eventtype = m_dvListCtrlEvents->GetTextValue(i, 0);
        wxString eventclss = m_dvListCtrlEvents->GetTextValue(i, 1);

        events.insert(std::make_pair(eventtype, eventclss));
    }
    cct.SetEvents(events);
    wxcSettings::Get().RegisterCustomControl(cct);
    wxcSettings::Get().Save();
    m_isModified = false;
}

void EditCustomControlDlg::OnControlModified(wxCommandEvent& event) { m_isModified = true; }

void EditCustomControlDlg::OnSaveUI(wxUpdateUIEvent& event) { event.Enable(m_isModified); }

void EditCustomControlDlg::OnDeleteEvent(wxCommandEvent& event)
{
    m_dvListCtrlEvents->DeleteItem(m_dvListCtrlEvents->GetSelectedRow());
    m_isModified = true;
}

void EditCustomControlDlg::OnNewEvent(wxCommandEvent& event)
{
    NewCustomEventDlg dlg(wxCrafter::TopFrame());
    if(dlg.ShowModal() == wxID_OK) {

        wxVector<wxVariant> cols;
        cols.push_back(dlg.GetEventType());
        cols.push_back(dlg.GetEventClass());
        m_dvListCtrlEvents->AppendItem(cols);
        m_isModified = true;
    }
}

void EditCustomControlDlg::OnDeleteEventUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlEvents->GetSelection().IsOk());
}

void EditCustomControlDlg::OnEventEditDone(wxDataViewEvent& event)
{
    event.Skip();
    m_isModified = true;
}
