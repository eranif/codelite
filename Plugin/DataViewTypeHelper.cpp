#include "DataViewTypeHelper.h"

#include <wx/dvrenderers.h>

namespace
{
constexpr int SEARCH_RESET_TIMEOUT_MS = 1200;
}

DataViewTypeHelper::DataViewTypeHelper(wxDataViewCtrl* ctrl, SearchMethod searchMethod)
    : m_ctrl(ctrl)
    , m_timer(this)
    , m_searchMethod(searchMethod)
{
    m_ctrl->Bind(wxEVT_KEY_DOWN, &DataViewTypeHelper::OnKeyDown, this);
    Bind(wxEVT_TIMER, &DataViewTypeHelper::OnTimer, this, m_timer.GetId());
}

DataViewTypeHelper::~DataViewTypeHelper()
{
    m_timer.Stop();
    Unbind(wxEVT_TIMER, &DataViewTypeHelper::OnTimer, this, m_timer.GetId());
    m_ctrl->Unbind(wxEVT_KEY_DOWN, &DataViewTypeHelper::OnKeyDown, this);
}

void DataViewTypeHelper::OnKeyDown(wxKeyEvent& event)
{
    wxChar ch = event.GetUnicodeKey();
    if (ch == WXK_NONE || event.HasModifiers() || wxIscntrl(ch)) {
        m_searchBuffer.clear();
        event.Skip();
        return;
    }

    m_searchBuffer << ch;
    SelectMatch(m_searchBuffer);
    m_timer.StartOnce(SEARCH_RESET_TIMEOUT_MS);
}

void DataViewTypeHelper::OnTimer(wxTimerEvent& WXUNUSED(event)) { m_searchBuffer.clear(); }

wxString DataViewTypeHelper::GetRowText(const wxDataViewItem& item) const
{
    const wxDataViewModel* model = m_ctrl->GetModel();
    if (!model) {
        return wxEmptyString;
    }

    unsigned int colCount = m_ctrl->GetColumnCount();
    wxString text;
    for (unsigned int col = 0; col < colCount; ++col) {
        wxVariant value;
        model->GetValue(value, item, col);

        wxString variantType = value.GetType();
        if (variantType == "string") {
            if (!text.empty())
                text << " ";
            text << value.GetString();
        } else if (variantType == "wxDataViewIconText") {
            wxDataViewIconText iconText;
            iconText << value;
            if (!text.empty())
                text << " ";
            text << iconText.GetText();
        }
    }
    return text;
}

void DataViewTypeHelper::CollectVisibleItems(const wxDataViewItem& parent, wxDataViewItemArray& items) const
{
    const wxDataViewModel* model = m_ctrl->GetModel();
    wxDataViewItemArray children;
    model->GetChildren(parent, children);
    for (const wxDataViewItem& child : children) {
        items.push_back(child);
        if (model->IsContainer(child) && m_ctrl->IsExpanded(child)) {
            CollectVisibleItems(child, items);
        }
    }
}

void DataViewTypeHelper::SelectMatch(const wxString& searchText)
{
    if (!m_ctrl->GetModel()) {
        return;
    }

    wxDataViewItemArray items;
    CollectVisibleItems(wxDataViewItem(), items);
    if (items.empty()) {
        return;
    }

    // Start searching at the current selection (if any) and wrap around. If
    // the current selection still matches the (possibly longer) search text,
    // it is found first and stays selected; otherwise the search advances
    // forward from it, rather than always jumping back to the first match.
    size_t startIndex = 0;
    wxDataViewItem current = m_ctrl->GetSelection();
    if (current.IsOk()) {
        int idx = items.Index(current);
        if (idx != wxNOT_FOUND) {
            startIndex = static_cast<size_t>(idx);
        }
    }

    for (size_t i = 0; i < items.size(); ++i) {
        const wxDataViewItem& item = items[(startIndex + i) % items.size()];
        wxString text = GetRowText(item);
        bool isMatch{false};
        switch (m_searchMethod) {
        case SearchMethod::kStartsWith:
            isMatch = text.Lower().StartsWith(searchText.Lower());
            break;
        case SearchMethod::kContains:
            isMatch = text.Lower().Contains(searchText.Lower());
            break;
        }
        if (isMatch) {
            m_ctrl->UnselectAll();
            m_ctrl->Select(item);
            m_ctrl->EnsureVisible(item);
            wxDataViewEvent evt(wxEVT_DATAVIEW_SELECTION_CHANGED, m_ctrl, item);
            m_ctrl->ProcessWindowEvent(evt);
            return;
        }
    }
}
