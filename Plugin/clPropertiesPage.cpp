#include "clPropertiesPage.hpp"

#include "EditDlg.h"
#include "clDataViewListCtrl.h"
#include "clSystemSettings.h"
#include "clThemedListCtrl.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "macros.h"

#include <wx/colordlg.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>

clPropertiesPage::clPropertiesPage(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));

    m_view = new clThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                  wxDV_ROW_LINES | wxDV_NO_HEADER | wxDV_COLUMN_WIDTH_NEVER_SHRINKS);

    GetSizer()->Add(m_view, wxSizerFlags(1).Expand());
    m_view->AppendTextColumn(_("Property Name"));
    m_view->AppendTextColumn(_("Property Value"));
    m_view->Bind(wxEVT_DATAVIEW_ACTION_BUTTON, &clPropertiesPage::OnActionButton, this);

    m_view->SetColumnWidth(1, 300);

    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clPropertiesPage::OnInitDone, this);
    GetSizer()->Layout();
}

clPropertiesPage::~clPropertiesPage()
{
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &clPropertiesPage::OnInitDone, this);
    if(m_events_connected) {
        EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clPropertiesPage::OnThemeChanged, this);
    }
    m_view->DeleteAllItems();
}

void clPropertiesPage::AddProperty(const wxString& label, const wxArrayString& choices, size_t sel)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewButton choice(sel < choices.size() ? choices[sel] : wxString(), eCellButtonType::BT_DROPDOWN_ARROW,
                            wxNOT_FOUND);
    wxVariant v;
    v << choice;
    cols.push_back(v);
    m_view->AppendItem(cols);
    SetLastLineData(LineKind::CHOICE, choices);
}

void clPropertiesPage::AddProperty(const wxString& label, bool checked)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewCheckbox c(checked);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    SetLastLineData(LineKind::CHECKBOX, checked);
}

void clPropertiesPage::AddProperty(const wxString& label, const wxString& value)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    // horizontal 3 dots symbol
    clDataViewButton c(value, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    SetLastLineData(LineKind::TEXT_EDIT, value);
}

void clPropertiesPage::AddProperty(const wxString& label, const wxColour& value)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewColour c(value);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    SetLastLineData(LineKind::COLOUR, value);
}

void clPropertiesPage::AddPropertyFilePicker(const wxString& label, const wxString& path)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    // horizontal 3 dots symbol
    clDataViewButton c(path, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    SetLastLineData(LineKind::FILE_PICKER, path);
}

void clPropertiesPage::AddPropertyDirPicker(const wxString& label, const wxString& path)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    // horizontal 3 dots symbol
    clDataViewButton c(path, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    SetLastLineData(LineKind::DIR_PICKER, path);
}

void clPropertiesPage::AddProperty(const wxString& label, const vector<wxString>& choices, size_t sel)
{
    wxArrayString arr;
    arr.reserve(choices.size());
    for(const auto& s : choices) {
        arr.Add(s);
    }
    AddProperty(label, arr, sel);
}

void clPropertiesPage::AddHeader(const wxString& label)
{
    // keep the header row number
    m_header_rows.push_back(m_view->GetItemCount());

    auto item = m_view->AppendItem(label);
    SetHeaderColours(item);
}

void clPropertiesPage::OnActionButton(wxDataViewEvent& e)
{
    auto item = e.GetItem();
    int row = m_view->ItemToRow(item);

    const LineData* data = nullptr;
    if(!GetLineData(row, &data))
        return;

    switch(data->line_kind) {
    case LineKind::TEXT_EDIT: {
        wxString value_str;
        if(!data->value.GetAs(&value_str))
            return;
        ShowTextEditor(row, value_str);
        break;
    }
    case LineKind::COLOUR: {
        wxColour colour_value;
        if(!data->value.GetAs(&colour_value))
            return;
        ShowColourPicker(row, colour_value);
        break;
    }
    case LineKind::CHOICE: {
        wxArrayString arr_value;
        if(!data->value.GetAs(&arr_value))
            return;
        ShowStringSelectionMenu(row, arr_value);
        break;
    }
    case LineKind::FILE_PICKER: {
        wxString path;
        if(!data->value.GetAs(&path))
            return;
        ShowFilePicker(row, path);
        break;
    }
    case LineKind::DIR_PICKER: {
        wxString path;
        if(!data->value.GetAs(&path))
            return;
        ShowDirPicker(row, path);
        break;
    }
    case LineKind::CHECKBOX:
    case LineKind::UNKNOWN:
    default:
        break;
    }
}

void clPropertiesPage::ShowColourPicker(size_t line, const wxColour& colour)
{
    wxColour c = ::wxGetColourFromUser(this, colour);
    if(!c.IsOk()) {
        return;
    }

    clDataViewColour v(c);
    wxVariant var;
    var << v;
    m_view->SetValue(var, line, 1);
    SetLineData(line, LineKind::COLOUR, c);
}

void clPropertiesPage::ShowTextEditor(size_t line, const wxString& text)
{
    wxString new_text = ::clGetStringFromUser(text, wxGetTopLevelParent(this));
    if(new_text.empty()) {
        return;
    }

    // update the cell value
    clDataViewButton c(new_text, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    m_view->SetValue(v, line, 1);
    SetLineData(line, LineKind::TEXT_EDIT, new_text);
}

void clPropertiesPage::ShowStringSelectionMenu(size_t line, const wxArrayString& options)
{
    m_view->ShowStringSelectionMenu(m_view->RowToItem(line), options, 1);
}

void clPropertiesPage::ShowFilePicker(size_t line, const wxString& path)
{
    wxString default_path = wxEmptyString;
    wxString default_name = wxEmptyString;
    wxString default_ext = wxEmptyString;
    if(!path.empty() && wxFileName(path).IsOk()) {
        default_path = wxFileName(path).GetPath();
        default_name = wxFileName(path).GetFullName();
        default_ext = wxFileName(path).GetExt();
    }

    wxString new_path = wxFileSelector(_("Choose a file"), default_path, default_name, default_ext);
    if(new_path.empty()) {
        return;
    }

    // update the view
    clDataViewButton c(new_path, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    m_view->SetValue(v, line, 1);
    SetLineData(line, LineKind::FILE_PICKER, new_path);
}

void clPropertiesPage::ShowDirPicker(size_t line, const wxString& path)
{
    wxString new_path = wxDirSelector(_("Choose a directory"), path);
    if(new_path.empty()) {
        return;
    }

    // update the view
    clDataViewButton c(new_path, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    m_view->SetValue(v, line, 1);
    SetLineData(line, LineKind::DIR_PICKER, new_path);
}

void clPropertiesPage::SetHeaderColours(const wxDataViewItem& item)
{
    // Notice that we can't use here m_view->GetColours()
    // since we can't really tell if we tree got the event `wxEVT_SYS_COLOURS_CHANGED` before
    // we did
    wxColour baseColour = clSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    wxColour header_bg_colour;
    wxColour header_text_colour;

    header_bg_colour = baseColour.ChangeLightness(50);
    header_text_colour = "WHITE";

    m_view->SetItemBold(item, true);
    m_view->SetItemBackgroundColour(item, header_bg_colour, 0);
    m_view->SetItemBackgroundColour(item, header_bg_colour, 1);

    m_view->SetItemTextColour(item, header_text_colour, 0);
    m_view->SetItemTextColour(item, header_text_colour, 1);
}

void clPropertiesPage::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    for(size_t row : m_header_rows) {
        wxDataViewItem item = m_view->RowToItem(row);
        SetHeaderColours(item);
    }
    m_view->Refresh();
}

void clPropertiesPage::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clPropertiesPage::OnThemeChanged, this);
    m_events_connected = true;
}

bool clPropertiesPage::GetLineData(size_t line, const LineData** data) const
{
    if(!data) {
        return false;
    }
    auto iter = m_lines.find(line);
    if(iter == m_lines.end()) {
        return false;
    }

    *data = &(iter->second);
    return true;
}
