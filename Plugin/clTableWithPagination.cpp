#include "clTableLineEditorDlg.h"
#include "clTableWithPagination.h"
#include "clThemedListCtrl.h"
#include "globals.h"
#include "macros.h"
#include <wx/dataview.h>
#include <wx/sizer.h>

clTableWithPagination::clTableWithPagination(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size,
                                             long style, const wxString& name)
    : wxPanel(parent, wxID_ANY, pos, size, style, name)
    , m_linesPerPage(100)
    , m_currentPage(0)
    , m_ctrl(NULL)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
    GetSizer()->Add(hSizer, 1, wxEXPAND | wxALL, 5);
    m_staticText = new wxStaticText(this, wxID_ANY, "");
    GetSizer()->Add(m_staticText, 0, wxEXPAND | wxALIGN_CENTER, 5);

    m_ctrl = new clThemedListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES);
    hSizer->Add(m_ctrl, 1, wxEXPAND);
    wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);
    hSizer->Add(vSizer, 0, wxEXPAND);

    m_btnNextPage = new wxButton(this, wxID_FORWARD);
    vSizer->Add(m_btnNextPage, 0, wxALIGN_RIGHT | wxALL | wxEXPAND, 5);

    m_btnPrevPage = new wxButton(this, wxID_BACKWARD);
    vSizer->Add(m_btnPrevPage, 0, wxALIGN_RIGHT | wxALL | wxEXPAND, 5);
    // Bind the command events
    m_btnNextPage->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
        wxUnusedVar(event);
        m_currentPage++;
        ShowPage(m_currentPage);
    });
    m_btnPrevPage->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
        wxUnusedVar(event);
        m_currentPage--;
        ShowPage(m_currentPage);
    });

    // And the 'update-ui' events
    m_btnNextPage->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(CanNext()); });
    m_btnPrevPage->Bind(wxEVT_UPDATE_UI, [&](wxUpdateUIEvent& event) { event.Enable(CanPrev()); });
    m_ctrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &clTableWithPagination::OnLineActivated, this);
    GetSizer()->Fit(this);
}

clTableWithPagination::~clTableWithPagination() {}

void clTableWithPagination::SetLinesPerPage(int numLines) { m_linesPerPage = numLines; }

void clTableWithPagination::SetColumns(const wxArrayString& columns)
{
    m_columns = columns;
    ClearAll();
    for(size_t i = 0; i < m_columns.size(); ++i) {
        m_ctrl->AppendTextColumn(m_columns.Item(i), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);
    }
}

void clTableWithPagination::SetData(std::vector<wxArrayString>& data)
{
    m_data.clear();
    m_data.swap(data);
    ShowPage(0);
}

void clTableWithPagination::ClearAll()
{
    m_data.clear();
    m_ctrl->DeleteAllItems();
    m_ctrl->ClearColumns();
}

void clTableWithPagination::ShowPage(int nPage)
{
    m_ctrl->DeleteAllItems();
    if(m_data.empty()) return;
    int startIndex = (nPage * m_linesPerPage);
    int lastIndex = startIndex + m_linesPerPage - 1; // last index, including
    if(lastIndex >= (int)m_data.size()) { lastIndex = (m_data.size() - 1); }
    m_currentPage = nPage;
    for(int i = startIndex; i <= lastIndex; ++i) {
        wxVector<wxVariant> cols;
        const wxArrayString& items = m_data[i];
        for(size_t j = 0; j < items.size(); ++j) {
            const wxString& cellContent = items.Item(j);
            cols.push_back(wxVariant(MakeDisplayString(cellContent)));
        }
        m_ctrl->AppendItem(cols, (wxUIntPtr)&items);
    }

    m_staticText->SetLabel(wxString() << _("Showing entries from: ") << startIndex << _(":") << lastIndex
                                      << " Total of: " << m_data.size() << _(" entries"));
}

bool clTableWithPagination::CanNext() const
{
    int startIndex = ((m_currentPage + 1) * m_linesPerPage);
    return startIndex < (int)m_data.size();
}

bool clTableWithPagination::CanPrev() const { return (((m_currentPage - 1) >= 0) && !m_data.empty()); }

void clTableWithPagination::ClearAllItems()
{
    // truncate the string to a reasonable string
    m_ctrl->DeleteAllItems();
}

wxString clTableWithPagination::MakeDisplayString(const wxString& str) const
{
    wxString truncatedString = str;
    if(truncatedString.Length() > 100) {
        truncatedString = truncatedString.Mid(0, 100);
        truncatedString.Append(wxT("..."));
    }

    // Convert all whitespace chars into visible ones
    truncatedString.Replace(wxT("\n"), wxT("\\n"));
    truncatedString.Replace(wxT("\r"), wxT("\\r"));
    truncatedString.Replace(wxT("\t"), wxT("\\t"));
    return truncatedString;
}

void clTableWithPagination::OnLineActivated(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    CHECK_ITEM_RET(item);

    wxArrayString* data = reinterpret_cast<wxArrayString*>(m_ctrl->GetItemData(item));
    CHECK_PTR_RET(data);

    clTableLineEditorDlg* dlg = new clTableLineEditorDlg(::wxGetTopLevelParent(this), m_columns, *data);
    dlg->Show();
}
