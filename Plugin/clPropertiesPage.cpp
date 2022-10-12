#include "clPropertiesPage.hpp"

#include "EditDlg.h"
#include "clDataViewListCtrl.h"
#include "clSystemSettings.h"
#include "clThemedListCtrl.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "macros.h"

#include <wx/colordlg.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/numdlg.h>

wxDEFINE_EVENT(wxEVT_PROPERTIES_PAGE_MODIFIED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_PROPERTIES_PAGE_SAVED, clCommandEvent);

clPropertiesPage::clPropertiesPage(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_view = new clThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                  wxDV_NO_HEADER | wxDV_COLUMN_WIDTH_NEVER_SHRINKS);

    // remove the wxTR_ROW_LINES style (this is the underlying style in clTreeCtrl
    // the controls the zebra line colouring
    m_view->SetTreeStyle(m_view->GetTreeStyle() & ~wxTR_ROW_LINES);

    GetSizer()->Add(m_view, wxSizerFlags(1).Expand());
    m_view->AppendTextColumn(_("Property Name"));
    m_view->AppendTextColumn(_("Property Value"));

    m_view->Bind(wxEVT_DATAVIEW_ACTION_BUTTON, &clPropertiesPage::OnActionButton, this);
    m_view->Bind(wxEVT_DATAVIEW_CHOICE, &clPropertiesPage::OnChoice, this);
    m_view->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, &clPropertiesPage::OnValueChanged, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clPropertiesPage::OnInitDone, this);
    GetSizer()->Layout();
}

clPropertiesPage::~clPropertiesPage()
{
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &clPropertiesPage::OnInitDone, this);
    m_view->Unbind(wxEVT_DATAVIEW_CHOICE, &clPropertiesPage::OnChoice, this);
    m_view->Unbind(wxEVT_DATAVIEW_ACTION_BUTTON, &clPropertiesPage::OnActionButton, this);
    m_view->Unbind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, &clPropertiesPage::OnValueChanged, this);

    if(m_theme_event_connected) {
        EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clPropertiesPage::OnThemeChanged, this);
    }
    m_view->DeleteAllItems();
}

void clPropertiesPage::AddProperty(const wxString& label, const wxArrayString& choices,
                                   const wxString& string_selection, clPropertiesPage::Callback_t update_cb)
{
    size_t index = choices.Index(string_selection);
    if(index == wxString::npos) {
        index = 0;
    }
    AddProperty(label, choices, static_cast<int>(index), std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, const std::vector<wxString>& choices, size_t sel,
                                   clPropertiesPage::Callback_t update_cb)
{
    wxArrayString arr;
    arr.reserve(choices.size());
    for(const auto& s : choices) {
        arr.Add(s);
    }
    AddProperty(label, arr, sel, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, const std::vector<wxString>& choices,
                                   const wxString& selection, clPropertiesPage::Callback_t update_cb)
{
    wxArrayString arr;
    arr.reserve(choices.size());
    for(const auto& s : choices) {
        arr.Add(s);
    }
    AddProperty(label, arr, selection, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, const wxArrayString& choices, size_t sel,
                                   clPropertiesPage::Callback_t update_cb)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewTextWithButton choice(sel < choices.size() ? choices[sel] : wxString(),
                                    eCellButtonType::BT_DROPDOWN_ARROW, wxNOT_FOUND);
    wxVariant v;
    v << choice;
    cols.push_back(v);
    m_view->AppendItem(cols);
    UpdateLastLineData(LineKind::CHOICE, choices, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, bool checked, clPropertiesPage::Callback_t update_cb)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewCheckbox c(checked);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    UpdateLastLineData(LineKind::CHECKBOX, checked, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, long value, clPropertiesPage::Callback_t update_cb)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewTextWithButton c(wxString() << value, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    UpdateLastLineData(LineKind::INTEGER, value, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, const wxString& value, clPropertiesPage::Callback_t update_cb)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    // horizontal 3 dots symbol
    clDataViewTextWithButton c(value, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    UpdateLastLineData(LineKind::TEXT_EDIT, value, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, const wxColour& value, clPropertiesPage::Callback_t update_cb)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    clDataViewColour c(value);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    UpdateLastLineData(LineKind::COLOUR, value, std::move(update_cb));
}

void clPropertiesPage::AddPropertyFilePicker(const wxString& label, const wxString& path,
                                             clPropertiesPage::Callback_t update_cb)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    // horizontal 3 dots symbol
    clDataViewTextWithButton c(path, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    UpdateLastLineData(LineKind::FILE_PICKER, path, std::move(update_cb));
}

void clPropertiesPage::AddPropertyDirPicker(const wxString& label, const wxString& path,
                                            clPropertiesPage::Callback_t update_cb)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    // horizontal 3 dots symbol
    clDataViewTextWithButton c(path, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    UpdateLastLineData(LineKind::DIR_PICKER, path, std::move(update_cb));
}

void clPropertiesPage::AddPropertyLanguagePicker(const wxString& label, const wxArrayString& langs,
                                                 clPropertiesPage::Callback_t update_cb)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    // horizontal 3 dots symbol
    wxString languages_str = wxJoin(langs, ';');
    clDataViewTextWithButton c(languages_str, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    cols.push_back(v);
    m_view->AppendItem(cols);
    UpdateLastLineData(LineKind::LANGUAGE_PICKER, languages_str, std::move(update_cb));
}

void clPropertiesPage::AddPropertyButton(const wxString& label, const wxString& button_label,
                                         clPropertiesPage::Callback_t update_cb)
{
    wxVector<wxVariant> cols;
    cols.push_back(label);

    // horizontal 3 dots symbol
    clDataViewButton button(button_label, wxNOT_FOUND);
    wxVariant v;
    v << button;
    cols.push_back(v);
    m_view->AppendItem(cols);
    UpdateLastLineData(LineKind::BUTTON, nullptr, std::move(update_cb));
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
    case LineKind::LANGUAGE_PICKER: {
        wxString curlangs;
        if(!data->value.GetAs(&curlangs))
            return;
        ShowLanguagePicker(row, curlangs);
        break;
    }
    case LineKind::DIR_PICKER: {
        wxString path;
        if(!data->value.GetAs(&path))
            return;
        ShowDirPicker(row, path);
        break;
    }
    case LineKind::INTEGER: {
        long value;
        if(!data->value.GetAs(&value))
            return;
        ShowNumberPicker(row, value);
        break;
    }
    case LineKind::BUTTON: {
        DoButtonClicked(row);
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
    wxColour c = ::wxGetColourFromUser(wxGetTopLevelParent(this), colour);
    if(!c.IsOk()) {
        return;
    }

    clDataViewColour v(c);
    wxVariant var;
    var << v;
    m_view->SetValue(var, line, 1);
    UpdateLineData(line, LineKind::COLOUR, c, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::ShowTextEditor(size_t line, const wxString& text)
{
    // use EditDlg directly and not clGetTextFromUser() method
    // since empty string is a valid value
    EditDlg dlg(wxGetTopLevelParent(this), text);
    if(dlg.ShowModal() != wxID_OK) {
        return;
    }

    wxString new_text = dlg.GetText();

    // update the cell value
    clDataViewTextWithButton c(new_text, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    m_view->SetValue(v, line, 1);
    UpdateLineData(line, LineKind::TEXT_EDIT, new_text, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::ShowStringSelectionMenu(size_t line, const wxArrayString& options)
{
    m_view->ShowStringSelectionMenu(m_view->RowToItem(line), options, 1);
    // any change is handled inside the `OnChoice` method
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

    wxString new_path = wxFileSelector(_("Choose a file"), default_path, default_name, default_ext, wxEmptyString, 0,
                                       wxGetTopLevelParent(this));
    if(new_path.empty()) {
        return;
    }

    // update the view
    clDataViewTextWithButton c(new_path, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    m_view->SetValue(v, line, 1);
    UpdateLineData(line, LineKind::FILE_PICKER, new_path, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::ShowDirPicker(size_t line, const wxString& path)
{
    wxString new_path = wxDirSelector(_("Choose a directory"), path, 0, wxDefaultPosition, wxGetTopLevelParent(this));
    if(new_path.empty()) {
        return;
    }

    // update the view
    clDataViewTextWithButton c(new_path, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    m_view->SetValue(v, line, 1);
    UpdateLineData(line, LineKind::DIR_PICKER, new_path, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::ShowLanguagePicker(size_t line, const wxString& langs)
{
    wxArrayString selected;
    if(!::clShowFileTypeSelectionDialog(wxGetTopLevelParent(this), wxStringTokenize(langs, ";", wxTOKEN_STRTOK),
                                        &selected)) {
        return;
    }

    // update the view
    wxString new_label = wxJoin(selected, ';');
    clDataViewTextWithButton c(new_label, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    m_view->SetValue(v, line, 1);
    UpdateLineData(line, LineKind::LANGUAGE_PICKER, new_label, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::DoButtonClicked(size_t line)
{
    // call the user provided callback
    NotifyChange(line);
}

void clPropertiesPage::ShowNumberPicker(size_t line, long number)
{
    wxString label = m_view->GetItemText(m_view->RowToItem(line));
    wxNumberEntryDialog dlg(wxGetTopLevelParent(this), label, wxEmptyString, _("Choose a number"), number, -10000,
                            10000);
    if(dlg.ShowModal() != wxID_OK)
        return;

    int new_number = dlg.GetValue();
    clDataViewTextWithButton c(wxString() << new_number, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    m_view->SetValue(v, line, 1);
    UpdateLineData(line, LineKind::INTEGER, new_number, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::SetHeaderColours(const wxDataViewItem& item)
{
    // Notice that we can't use here m_view->GetColours()
    // since we can't really tell if we tree got the event `wxEVT_SYS_COLOURS_CHANGED` before
    // we did
    wxColour baseColour = clSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    wxColour header_bg_colour;
    wxColour header_text_colour;

    header_bg_colour = baseColour.ChangeLightness(80);
    if(DrawingUtils::IsDark(header_bg_colour)) {
        header_text_colour = "WHITE";
    } else {
        header_text_colour = "BLACK";
    }

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
    m_theme_event_connected = true;
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

void clPropertiesPage::OnChoice(wxDataViewEvent& event)
{
    event.Skip();
    size_t line = m_view->ItemToRow(event.GetItem());

    // Get the user callback and call it with the new value
    const LineData* line_data = nullptr;
    if(!GetLineData(line, &line_data))
        return;

    if(line_data->callback) {
        line_data->callback(m_view->GetItemText(m_view->RowToItem(line)), event.GetString());
    }
    SetModified();
}

void clPropertiesPage::OnValueChanged(wxDataViewEvent& event)
{
    event.Skip();
    size_t line = m_view->ItemToRow(event.GetItem());
    UpdateLineData(line, LineKind::CHECKBOX, m_view->IsItemChecked(event.GetItem(), 1), nullptr);

    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::SetModified()
{
    m_isModified = true;
    clCommandEvent modified_event(wxEVT_PROPERTIES_PAGE_MODIFIED);
    modified_event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(modified_event);
}

void clPropertiesPage::ClearModified()
{
    m_isModified = false;
    clCommandEvent modified_event(wxEVT_PROPERTIES_PAGE_SAVED);
    modified_event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(modified_event);
}

void clPropertiesPage::NotifyChange(size_t line)
{
    const LineData* line_data = nullptr;
    if(!GetLineData(line, &line_data))
        return;

    if(line_data->callback) {
        line_data->callback(m_view->GetItemText(m_view->RowToItem(line)), line_data->value);
    }
}

void clPropertiesPage::Finalize() { m_view->Commit(); }

void clPropertiesPage::Begin() { m_view->Begin(); }

void clPropertiesPage::Clear()
{
    CHECK_PTR_RET(m_view);
    m_view->DeleteAllItems();
    m_isModified = false;
    m_header_rows.clear();
    m_lines.clear();
}