#include "clPropertiesPage.hpp"

#include "EditDlg.h"
#include "clDataViewListCtrl.h"
#include "clSystemSettings.h"
#include "clThemedListCtrl.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "macros.h"

#include <wx/colordlg.h>

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

    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clPropertiesPage::OnInitDone, this);
    GetSizer()->Layout();
}

clPropertiesPage::~clPropertiesPage()
{
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &clPropertiesPage::OnInitDone, this);
    if(m_events_connected) {
        EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &clPropertiesPage::OnThemeChanged, this);
    }
    m_view->DeleteAllItems([](wxUIntPtr d) {
        wxArrayString* choices = reinterpret_cast<wxArrayString*>(d);
        wxDELETE(choices);
    });
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
    m_view->AppendItem(cols, (wxUIntPtr) new wxArrayString(choices));
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
}

void clPropertiesPage::OnActionButton(wxDataViewEvent& e)
{
    auto item = e.GetItem();
    int col = e.GetColumn();
    int row = m_view->ItemToRow(item);

    auto cell_type = m_view->GetCellDataType(row, col);
    if(cell_type == CellType::UNKNOWN) {
        return;
    }

    switch(cell_type) {
    case CellType::TEXT_EDIT:
        ShowTextEditor(item, col);
        break;
    case CellType::CHECKBOX_TEXT:
        break;
    case CellType::TEXT_OPTIONS:
        ShowStringSelectionMenu(item, col);
        break;
    case CellType::COLOUR:
        ShowColourPicker(item, col);
        break;
    case CellType::UNKNOWN:
    default:
        break;
    }
}

void clPropertiesPage::ShowColourPicker(const wxDataViewItem& item, size_t col)
{
    CHECK_ITEM_RET(item);
    wxColour value(m_view->GetItemText(item, col));
    wxColour c = ::wxGetColourFromUser(this, value);
    if(!c.IsOk()) {
        return;
    }

    clDataViewColour v(c);
    wxVariant var;
    var << v;
    m_view->SetValue(var, m_view->ItemToRow(item), col);
    m_view->Refresh();
}

void clPropertiesPage::ShowTextEditor(const wxDataViewItem& item, size_t col)
{
    CHECK_ITEM_RET(item);
    wxString curtext = m_view->GetItemText(item, col);
    wxString new_text = ::clGetStringFromUser(curtext, wxGetTopLevelParent(this));
    if(new_text.empty()) {
        return;
    }

    // update the cell value
    clDataViewButton c(new_text, eCellButtonType::BT_ELLIPSIS, wxNOT_FOUND);
    wxVariant v;
    v << c;
    m_view->SetValue(v, m_view->ItemToRow(item), col);
}

void clPropertiesPage::ShowStringSelectionMenu(const wxDataViewItem& item, size_t col)
{
    CHECK_ITEM_RET(item);
    wxArrayString* choices = reinterpret_cast<wxArrayString*>(m_view->GetItemData(item));
    if(!choices) {
        return;
    }
    m_view->ShowStringSelectionMenu(item, *choices, col);
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

void clPropertiesPage::SetHeaderColours(const wxDataViewItem& item)
{
    const auto& colours = m_view->GetColours();

    wxColour header_bg_colour;
    wxColour header_text_colour;
    if(colours.IsLightTheme()) {
        header_bg_colour = "GREY";
        header_text_colour = "BLACK";
    } else {
        header_bg_colour = "BLACK";
        header_text_colour = "WHITE";
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
}

void clPropertiesPage::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &clPropertiesPage::OnThemeChanged, this);
    m_events_connected = true;
}