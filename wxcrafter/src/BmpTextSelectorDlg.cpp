#include "BmpTextSelectorDlg.h"
#include "SingleBitmapAndTextDlg.h"
#include <json_node.h>
#include <windowattrmanager.h>

BmpTextSelectorDlg::BmpTextSelectorDlg(wxWindow* parent, const wxString& initialValue)
    : BmpTextSelectorDlgBase(parent)
{
    BmpTextVec_t vec = FromString(initialValue);
    for(size_t i = 0; i < vec.size(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back(vec.at(i).first);
        cols.push_back(vec.at(i).second);
        m_dvListCtrl->AppendItem(cols);
    }
    SetName("BmpTextSelectorDlg");
    WindowAttrManager::Load(this);
}

BmpTextSelectorDlg::~BmpTextSelectorDlg() {}

void BmpTextSelectorDlg::OnDelete(wxCommandEvent& event)
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    if(item.IsOk()) { m_dvListCtrl->DeleteItem(m_dvListCtrl->ItemToRow(item)); }
}

void BmpTextSelectorDlg::OnEdit(wxCommandEvent& event)
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    if(item.IsOk()) {
        int row = m_dvListCtrl->ItemToRow(item);
        wxVariant value;
        wxString bmp, label;
        m_dvListCtrl->GetValue(value, row, 0);
        bmp = value.GetString();
        m_dvListCtrl->GetValue(value, row, 1);
        label = value.GetString();
        SingleBitmapAndTextDlg dlg(this, bmp, label);
        if(dlg.ShowModal() == wxID_OK) {
            m_dvListCtrl->SetValue(dlg.GetTextCtrlBmp()->GetValue(), row, 0);
            m_dvListCtrl->SetValue(dlg.GetTextCtrlText()->GetValue(), row, 1);
        }
    }
}

void BmpTextSelectorDlg::OnNew(wxCommandEvent& event)
{
    SingleBitmapAndTextDlg dlg(this, "", "");
    if(dlg.ShowModal() == wxID_OK) {
        wxVector<wxVariant> cols;
        cols.push_back(dlg.GetTextCtrlBmp()->GetValue());
        cols.push_back(dlg.GetTextCtrlText()->GetValue());
        m_dvListCtrl->AppendItem(cols);
    }
}

BmpTextVec_t BmpTextSelectorDlg::FromString(const wxString& text)
{
    BmpTextVec_t vec;
    JSONRoot root(text);
    int size = root.toElement().arraySize();
    for(int i = 0; i < size; ++i) {
        JSONElement item = root.toElement().arrayItem(i);
        wxString bitmap = item.namedObject("bmp").toString();
        wxString label = item.namedObject("label").toString();
        vec.push_back(std::make_pair(bitmap, label));
    }
    return vec;
}

wxString BmpTextSelectorDlg::ToString(const BmpTextVec_t& vec)
{
    JSONRoot root(cJSON_Array);
    for(size_t i = 0; i < vec.size(); ++i) {
        JSONElement element = JSONElement::createObject();
        element.addProperty("bmp", vec.at(i).first);
        element.addProperty("label", vec.at(i).second);
        root.toElement().arrayAppend(element);
    }
    wxString asString(root.toElement().format());
    asString.Replace("\n", "");
    return asString;
}

wxString BmpTextSelectorDlg::GetValue()
{
    BmpTextVec_t v;
    int count = m_dvListCtrl->GetItemCount();
    for(int i = 0; i < count; ++i) {
        wxVariant value;
        wxString bmp, label;
        m_dvListCtrl->GetValue(value, i, 0);
        bmp = value.GetString();
        m_dvListCtrl->GetValue(value, i, 1);
        label = value.GetString();
        v.push_back(std::make_pair(bmp, label));
    }
    return ToString(v);
}
void BmpTextSelectorDlg::OnDeleteUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->GetSelection().IsOk()); }
void BmpTextSelectorDlg::OnEditUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->GetSelection().IsOk()); }
void BmpTextSelectorDlg::OnItemActivated(wxDataViewEvent& event)
{
    wxCommandEvent dummy;
    if(m_dvListCtrl->GetSelection().IsOk()) { OnEdit(dummy); }
}
