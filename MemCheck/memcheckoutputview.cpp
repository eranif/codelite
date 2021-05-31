/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#include <wx/stc/stc.h>
#include <wx/busyinfo.h>
#include <wx/clipbrd.h>

#include "event_notifier.h"
#include "workspace.h"
#include "file_logger.h"
#include "dirsaver.h"
#include "stringsearcher.h"

#include "memcheckdefs.h"
#include "memcheckoutputview.h"
#include "memchecksettings.h"
#include "search_thread.h"

MemCheckOutputView::MemCheckOutputView(wxWindow* parent, MemCheckPlugin* plugin, IManager* mgr)
    : MemCheckOutputViewBase(parent)
    , m_plugin(plugin)
    , m_mgr(mgr)
    , pageValidator(&m_currentPage)
{
    int col = GetColumnByName(_("Label"));
    if(col == wxNOT_FOUND) {
        return;
    }
    m_dataViewCtrlErrors->SetExpanderColumn(m_dataViewCtrlErrors->GetColumn(col));
    m_listCtrlErrors->SetData(&m_filterResults);

    m_searchMenu = new wxMenu();
    m_searchMenu->Append(XRCID("memcheck_search_string"), _("Search string"));
    m_searchCtrlFilter->SetMenu(m_searchMenu);
    m_searchMenu->AppendSeparator();
    m_searchMenu->Append(XRCID("memcheck_search_nonworkspace"), _("Search 'nonworkspace'"));
    m_searchCtrlFilter->SetMenu(m_searchMenu);

    m_searchMenu->Connect(XRCID("memcheck_search_string"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnFilterErrors), NULL, this);
    m_searchMenu->Connect(XRCID("memcheck_search_string"), wxEVT_UPDATE_UI,
        wxUpdateUIEventHandler(MemCheckOutputView::OnSuppPanelUI), NULL, this);
    m_searchMenu->Connect(XRCID("memcheck_search_nonworkspace"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnSearchNonworkspace), NULL, this);
    m_searchMenu->Connect(XRCID("memcheck_search_nonworkspace"), wxEVT_UPDATE_UI,
        wxUpdateUIEventHandler(MemCheckOutputView::OnSuppPanelUI), NULL, this);
}

MemCheckOutputView::~MemCheckOutputView()
{
    m_searchMenu->Disconnect(XRCID("memcheck_search_string"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnFilterErrors), NULL, this);
    m_searchMenu->Disconnect(XRCID("memcheck_search_string"), wxEVT_UPDATE_UI,
        wxUpdateUIEventHandler(MemCheckOutputView::OnSuppPanelUI), NULL, this);
    m_searchMenu->Disconnect(XRCID("memcheck_search_nonworkspace"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnSearchNonworkspace), NULL, this);
    m_searchMenu->Disconnect(XRCID("memcheck_search_nonworkspace"), wxEVT_UPDATE_UI,
        wxUpdateUIEventHandler(MemCheckOutputView::OnSuppPanelUI), NULL, this);
}

void MemCheckOutputView::LoadErrors()
{
    CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::LoadErrors()"));

    if(m_mgr->IsWorkspaceOpen())
        m_workspacePath =
            m_mgr->GetWorkspace()->GetWorkspaceFileName().GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    else
        m_workspacePath = wxEmptyString;

    // common part for both pages
    m_choiceSuppFile->Set(m_plugin->GetProcessor()->GetSuppressionFiles());
    m_choiceSuppFile->SetSelection(0);

    // errors panel
    ResetItemsView();
    ShowPageView(1); // after reload start at page 1

    // supp panel
    ResetItemsSupp();
    ApplyFilterSupp(FILTER_CLEAR);
}

void MemCheckOutputView::ResetItemsView()
{
    ErrorList& errorList = m_plugin->GetProcessor()->GetErrors();

    unsigned int flags = 0;
    if(m_plugin->GetSettings()->GetOmitNonWorkspace()) flags |= MC_IT_OMIT_NONWORKSPACE;
    if(m_plugin->GetSettings()->GetOmitDuplications()) flags |= MC_IT_OMIT_DUPLICATIONS;
    if(m_plugin->GetSettings()->GetOmitSuppressed()) flags |= MC_IT_OMIT_SUPPRESSED;

    m_totalErrorsView = 0;
    for(MemCheckIterTools::ErrorListIterator it = MemCheckIterTools::Factory(errorList, m_workspacePath, flags);
        it != errorList.end(); ++it) {
        ++m_totalErrorsView;
    }

    if(m_totalErrorsView)
        m_pageMax = (m_totalErrorsView - 1) / m_plugin->GetSettings()->GetResultPageSize() + 1;
    else
        m_pageMax = 0;

    m_staticTextPageMax->SetLabel(wxString::Format(wxT("%lu"), m_pageMax));
    m_staticTextPageMax->GetParent()->Layout();

    pageValidator.SetRange(1, m_pageMax);
    m_textCtrlPageNumber->SetValidator(pageValidator);
    pageValidator.SetWindow(m_textCtrlPageNumber);
    itemsInvalidView = false;
}

void MemCheckOutputView::ResetItemsSupp()
{
    ErrorList& errorList = m_plugin->GetProcessor()->GetErrors();

    size_t iterFlags = 0;
    if(m_plugin->GetSettings()->GetOmitSuppressed()) iterFlags |= MC_IT_OMIT_SUPPRESSED;

    m_totalErrorsSupp = 0;
    for(MemCheckIterTools::ErrorListIterator it = MemCheckIterTools::Factory(errorList, wxEmptyString, iterFlags);
        it != errorList.end(); ++it)
        ++m_totalErrorsSupp;

    m_lastToolTipItem = wxNOT_FOUND;
}

void MemCheckOutputView::ShowPageView(size_t page)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::ShowPage()"));

    if(page < 1)
        m_currentPage = 1;
    else if(page > m_pageMax)
        m_currentPage = m_pageMax;
    else
        m_currentPage = page;

    if(m_currentPage == 0)
        m_textCtrlPageNumber->Clear();
    else
        pageValidator.TransferToWindow(); // it sets m_textCtrlPageNumber

    m_currentPageIsEmptyView = true;
    m_currentItem = wxDataViewItem(0);
    m_onValueChangedLocked = false;
    m_dataViewCtrlErrorsModel->Clear();

    if(m_totalErrorsView == 0) return;

    ErrorList& errorList = m_plugin->GetProcessor()->GetErrors();
    long iStart = (long)(m_currentPage - 1) * m_plugin->GetSettings()->GetResultPageSize();
    long iStop =
        (long)std::min(m_totalErrorsView - 1, m_currentPage * m_plugin->GetSettings()->GetResultPageSize() - 1);
    // CL_DEBUG1(PLUGIN_PREFIX("start - stop = %lu - %lu", iStart, iStop));
    m_currentPageIsEmptyView = (iStop - iStart) < 0;

    // this should never happen if m_totalErrorsView > 0, but...
    if(m_currentPageIsEmptyView) return;

    wxWindowDisabler disableAll;
    wxBusyInfo wait(BUSY_MESSAGE);
    m_mgr->GetTheApp()->Yield();

    unsigned int flags = 0;
    if(m_plugin->GetSettings()->GetOmitNonWorkspace()) flags |= MC_IT_OMIT_NONWORKSPACE;
    if(m_plugin->GetSettings()->GetOmitDuplications()) flags |= MC_IT_OMIT_DUPLICATIONS;
    if(m_plugin->GetSettings()->GetOmitSuppressed()) flags |= MC_IT_OMIT_SUPPRESSED;
    size_t i = 0;
    MemCheckIterTools::ErrorListIterator it = MemCheckIterTools::Factory(errorList, m_workspacePath, flags);
    for(; i < iStart && it != errorList.end(); ++i, ++it)
        ; // skipping item before start
    // CL_DEBUG1(PLUGIN_PREFIX("items skipped"));
    m_mgr->GetTheApp()->Yield();
    for(; i <= iStop; ++i, ++it) {
        if(it == errorList.end()) {
            CL_WARNING(PLUGIN_PREFIX("Some items skipped. Total errors count mismatches the iterator."));
            break;
        }
        AddTree(wxDataViewItem(0), *it); // CL_DEBUG1(PLUGIN_PREFIX("adding %lu", i));
        if(!(i % WAIT_UPDATE_PER_ITEMS)) m_mgr->GetTheApp()->Yield();
    }
}

void MemCheckOutputView::AddTree(const wxDataViewItem& parentItem, MemCheckError& error)
{
    // CL_DEBUG1(PLUGIN_PREFIX("error #\t'%s'", error.label));

    wxVariant variantBitmap;
    variantBitmap << wxXmlResource::Get()->LoadBitmap(wxT("memcheck_transparent"));

    wxVector<wxVariant> cols;
    cols.push_back(variantBitmap);
    cols.push_back(wxVariant(false));
    cols.push_back(MemCheckDVCErrorsModel::CreateIconTextVariant(error.label,
        (error.type == MemCheckError::TYPE_AUXILIARY ? wxXmlResource::Get()->LoadBitmap(wxT("memcheck_auxiliary")) :
                                                       wxXmlResource::Get()->LoadBitmap(wxT("memcheck_error")))));
    cols.push_back(wxString());
    cols.push_back(wxString());
    cols.push_back(wxString());
    wxDataViewItem errorItem =
        m_dataViewCtrlErrorsModel->AppendItem(parentItem, cols, new MemCheckErrorReferrer(error));

    for(ErrorList::iterator it = error.nestedErrors.begin(); it != error.nestedErrors.end(); ++it) {
        AddTree(errorItem, *it);
    }

    unsigned int flags = 0;
    if(m_plugin->GetSettings()->GetOmitNonWorkspace()) flags |= MC_IT_OMIT_NONWORKSPACE;
    if(m_plugin->GetSettings()->GetOmitDuplications()) flags |= MC_IT_OMIT_DUPLICATIONS;
    if(m_plugin->GetSettings()->GetOmitSuppressed()) flags |= MC_IT_OMIT_SUPPRESSED;

    wxBitmap bmpLocation = wxXmlResource::Get()->LoadBitmap(wxT("memcheck_location"));
    MemCheckIterTools::LocationListIterator it = MemCheckIterTools::Factory(error.locations, m_workspacePath, flags);
    for(; it != error.locations.end(); ++it) {
        MemCheckErrorLocation& location = *it;
        cols.clear();
        cols.push_back(variantBitmap);
        cols.push_back(wxVariant(false));
        cols.push_back(MemCheckDVCErrorsModel::CreateIconTextVariant(location.func, bmpLocation));
        cols.push_back(wxVariant(location.getFile(m_workspacePath)));

        wxString strLine;
        strLine << location.line;
        cols.push_back(strLine);
        cols.push_back(wxVariant(location.getObj(m_workspacePath)));
        m_dataViewCtrlErrorsModel->AppendItem(errorItem, cols,
            ((location.line > 0 && !location.file.IsEmpty()) ? new MemCheckErrorLocationReferrer(location) : NULL));
    }
}

void MemCheckOutputView::OnPageFirst(wxCommandEvent& event) { ShowPageView(1); }

void MemCheckOutputView::OnPagePrev(wxCommandEvent& event) { ShowPageView(m_currentPage - 1); }

void MemCheckOutputView::OnPageSelect(wxCommandEvent& event)
{
    pageValidator.TransferFromWindow();
    ShowPageView(m_currentPage);
}

void MemCheckOutputView::OnPageNext(wxCommandEvent& event) { ShowPageView(m_currentPage + 1); }

void MemCheckOutputView::OnPageLast(wxCommandEvent& event) { ShowPageView(m_pageMax); }

unsigned int MemCheckOutputView::GetColumnByName(const wxString& name)
{
    for(unsigned int i = 0; i < m_dataViewCtrlErrors->GetColumnCount(); i++)
        if(m_dataViewCtrlErrors->GetColumn(i)->GetTitle().IsSameAs(name, false)) return i;

    CL_ERROR(PLUGIN_PREFIX("Column named '%s' not found.", name));
    return -1;
}

void MemCheckOutputView::JumpToLocation(const wxDataViewItem& item)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::JumpToLocation()"));

    MemCheckErrorLocationReferrer* locationRef =
        dynamic_cast<MemCheckErrorLocationReferrer*>(m_dataViewCtrlErrorsModel->GetClientObject(item));
    if(!locationRef) return;

    int line = locationRef->Get().line - 1;
    wxString fileName = locationRef->Get().getFile();

    if(line < 0 || fileName.IsEmpty()) return;

    if(m_mgr->OpenFile(fileName, wxEmptyString, line)) {
        IEditor* editor = m_mgr->GetActiveEditor();
        if(editor) {
            int posStart = editor->GetCtrl()->PositionFromLine(line);
            int lineLen = editor->GetCtrl()->LineLength(line);
            editor->SelectText(posStart, lineLen - 1);
        }
    }
}

wxDataViewItem MemCheckOutputView::GetTopParent(wxDataViewItem item)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::GetTopParent()"));

    wxVector<wxVariant> cols;
    wxDataViewIconText x;

    if(!item.IsOk()) {
        CL_ERROR(PLUGIN_PREFIX("Virtual root item has no parent."));
        return item;
    }

    wxDataViewItem parent = m_dataViewCtrlErrorsModel->GetParent(item);
    while(parent.IsOk()) {
        item = parent;
        parent = m_dataViewCtrlErrorsModel->GetParent(item);
    }
    return item;
}

wxDataViewItem MemCheckOutputView::GetLeaf(const wxDataViewItem& item, bool first = true)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::GetLeaf()"));

    if(m_dataViewCtrlErrorsModel->IsContainer(item)) {
        m_dataViewCtrlErrors->Expand(item);
        wxDataViewItemArray subItems;
        m_dataViewCtrlErrorsModel->GetChildren(item, subItems);
        return GetLeaf(subItems.Item(first ? 0 : subItems.GetCount() - 1));
    } else {
        return item;
    }
}

void MemCheckOutputView::OnActivated(wxDataViewEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::OnActivated()"));

    wxDataViewItem item = GetLeaf(event.GetItem());
    SetCurrentItem(item);
    JumpToLocation(item);
}

void MemCheckOutputView::ExpandAll(const wxDataViewItem& item)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::ExpandAll()"));

    m_dataViewCtrlErrors->Expand(item);
    wxDataViewItemArray subItems;
    m_dataViewCtrlErrorsModel->GetChildren(item, subItems);
    for(wxDataViewItemArray::iterator it = subItems.begin(); it != subItems.end(); ++it) {
        ExpandAll(*it);
    }
}

void MemCheckOutputView::OnExpandAll(wxCommandEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::OnExpandAll()"));

    m_notebookOutputView->ChangeSelection(m_notebookOutputView->FindPage(m_panelErrors));
    // not implemented in wxDVC :(
    // m_dataViewCtrlErrors->ExpandAll();
    ExpandAll(wxDataViewItem(0));
}

void MemCheckOutputView::SetCurrentItem(const wxDataViewItem& item)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::SetCurrentItem()"));

    wxVariant variantBitmap;
    int col = GetColumnByName(_("Current"));
    if(col == wxNOT_FOUND) {
        return;
    }

    if(m_currentItem.IsOk()) {
        variantBitmap << wxXmlResource::Get()->LoadBitmap(wxT("memcheck_transparent"));
        m_dataViewCtrlErrorsModel->ChangeValue(variantBitmap, m_currentItem, col);
    }

    m_dataViewCtrlErrors->SetCurrentItem(item);
    m_dataViewCtrlErrors->EnsureVisible(item);
    m_currentItem = item;

    variantBitmap << wxXmlResource::Get()->LoadBitmap(wxT("memcheck_current"));
    m_dataViewCtrlErrorsModel->ChangeValue(variantBitmap, item, col);
}

wxDataViewItem MemCheckOutputView::GetAdjacentItem(const wxDataViewItem& item, bool forward = true)
{
    wxDataViewItem parent = m_dataViewCtrlErrorsModel->GetParent(item);
    wxDataViewItemArray siblings;
    m_dataViewCtrlErrorsModel->GetChildren(parent, siblings);
    int pos = siblings.Index(item) + (forward ? 1 : -1);

    if(pos < 0 || pos >= (int)siblings.GetCount()) {
        if(!parent.IsOk()) { // parent == virtual root => we are on top level
            return GetLeaf(siblings.Item(forward ? 0 : siblings.GetCount() - 1), forward);
        } else {
            return GetLeaf(GetAdjacentItem(parent, forward), forward);
        }
    } else {
        return GetLeaf(siblings.Item(pos), forward);
    }
}

void MemCheckOutputView::OnSelectionChanged(wxDataViewEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::OnSelectionChanged()"));
    if(m_dataViewCtrlErrors->GetCurrentItem().IsOk()) {
        SetCurrentItem(m_dataViewCtrlErrors->GetCurrentItem());
    }
}

void MemCheckOutputView::OnJumpToNext(wxCommandEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::OnJumpToNext()"));

    if(m_currentPageIsEmptyView) return;

    m_notebookOutputView->ChangeSelection(m_notebookOutputView->FindPage(m_panelErrors));

    wxDataViewItem item = m_dataViewCtrlErrors->GetCurrentItem();
    if(item.IsOk()) {
        item = GetAdjacentItem(item, true);
    } else {
        wxDataViewItemArray items;
        m_dataViewCtrlErrorsModel->GetChildren(wxDataViewItem(0), items);
        item = GetLeaf(items.Item(0), true);
    }

    SetCurrentItem(item);
    JumpToLocation(item);
}

void MemCheckOutputView::OnJumpToPrev(wxCommandEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::OnJumpToPrev()"));

    if(m_currentPageIsEmptyView) return;

    m_notebookOutputView->ChangeSelection(m_notebookOutputView->FindPage(m_panelErrors));

    wxDataViewItem item = m_dataViewCtrlErrors->GetCurrentItem();
    if(item.IsOk()) {
        item = GetAdjacentItem(item, false);
    } else {
        wxDataViewItemArray items;
        m_dataViewCtrlErrorsModel->GetChildren(wxDataViewItem(0), items);
        item = GetLeaf(items.Item(items.GetCount() - 1), false);
    }

    SetCurrentItem(item);
    JumpToLocation(item);
}

void MemCheckOutputView::MarkTree(const wxDataViewItem& item, bool checked)
{
    int col = GetColumnByName(_("Suppress"));
    if(col == wxNOT_FOUND) {
        return;
    }
    m_dataViewCtrlErrorsModel->ChangeValue(wxVariant(checked), item, col);

    if(m_dataViewCtrlErrorsModel->IsContainer(item)) {
        wxDataViewItemArray subItems;
        m_dataViewCtrlErrorsModel->GetChildren(item, subItems);
        for(size_t i = 0; i < subItems.GetCount(); ++i) MarkTree(subItems.Item(i), checked);
    }
}


void MemCheckOutputView::OnContextMenu(wxDataViewEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::OnContextMenu()"));

    if(m_currentPageIsEmptyView) return;

    bool unmarked, marked;
    GetStatusOfErrors(unmarked, marked);

    const wxDataViewItem& dataItem = event.GetItem();
    wxMenuItem* menuItem(NULL);
    wxMenu menu;

    menuItem = menu.Append(XRCID("memcheck_jump_to_location"), _("Jump to location"));
    menuItem->Enable(dataItem.IsOk() && !m_dataViewCtrlErrorsModel->IsContainer(dataItem));
    menu.AppendSeparator();
    menuItem = menu.Append(XRCID("memcheck_mark_all_errors"), _("Mark all"));
    menuItem->Enable(unmarked);
    menuItem = menu.Append(XRCID("memcheck_unmark_all_errors"), _("Unmark all"));
    menuItem->Enable(marked);
    menu.AppendSeparator();
    menuItem = menu.Append(XRCID("memcheck_suppress_error"), _("Suppress this error"));
    menuItem->Enable(dataItem.IsOk() && m_choiceSuppFile->GetSelection() != wxNOT_FOUND);
    menuItem = menu.Append(XRCID("memcheck_suppress_marked_errors"), _("Suppress all marked errors"));
    menuItem->Enable(marked && m_choiceSuppFile->GetSelection() != wxNOT_FOUND);
    menu.AppendSeparator();
    menuItem = menu.Append(XRCID("memcheck_row_to_clip"), _("Copy line as string to clipboard"));
    menuItem->Enable(dataItem.IsOk());
    menuItem = menu.Append(XRCID("memcheck_error_to_clip"), _("Copy error as string to clipboard"));
    menuItem->Enable(dataItem.IsOk());
    menuItem = menu.Append(XRCID("memcheck_marked_errors_to_clip"), _("Copy marked errors to clipboard"));
    menuItem->Enable(marked);

    menu.Connect(XRCID("memcheck_jump_to_location"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnJumpToLocation), new wxDataViewEvent(event), (wxEvtHandler*)this);
    menu.Connect(XRCID("memcheck_mark_all_errors"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnMarkAllErrors), new wxDataViewEvent(event), (wxEvtHandler*)this);
    menu.Connect(XRCID("memcheck_unmark_all_errors"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnUnmarkAllErrors), new wxDataViewEvent(event), (wxEvtHandler*)this);
    menu.Connect(XRCID("memcheck_suppress_error"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnSuppressError), new wxDataViewEvent(event), (wxEvtHandler*)this);
    menu.Connect(XRCID("memcheck_suppress_marked_errors"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnSuppressMarkedErrors), new wxDataViewEvent(event),
        (wxEvtHandler*)this);
    menu.Connect(XRCID("memcheck_row_to_clip"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnRowToClip), new wxDataViewEvent(event), (wxEvtHandler*)this);
    menu.Connect(XRCID("memcheck_error_to_clip"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnErrorToClip), new wxDataViewEvent(event), (wxEvtHandler*)this);
    menu.Connect(XRCID("memcheck_marked_errors_to_clip"), wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(MemCheckOutputView::OnMarkedErrorsToClip), new wxDataViewEvent(event),
        (wxEvtHandler*)this);

    m_dataViewCtrlErrors->PopupMenu(&menu);
}

void MemCheckOutputView::OnJumpToLocation(wxCommandEvent& event)
{
    wxDataViewEvent* menuEvent = dynamic_cast<wxDataViewEvent*>(event.GetEventUserData());
    if(!menuEvent) return;

    wxDataViewItem item = GetLeaf(menuEvent->GetItem());
    SetCurrentItem(item);
    JumpToLocation(item);
}

void MemCheckOutputView::OnMarkAllErrors(wxCommandEvent& event)
{
    MarkAllErrors(true);
}

void MemCheckOutputView::OnUnmarkAllErrors(wxCommandEvent& event)
{
    MarkAllErrors(false);
}

void MemCheckOutputView::MarkAllErrors(bool state)
{
    wxDataViewItemArray items;
    m_dataViewCtrlErrorsModel->GetChildren(wxDataViewItem(0), items);

    for(wxDataViewItemArray::iterator it = items.begin(); it != items.end(); ++it) {
        MarkTree(*it, state);
    }
}

void MemCheckOutputView::GetStatusOfErrors(bool& unmarked, bool& marked)
{
    wxDataViewItemArray items;
    wxVariant variant;
    int supColumn = GetColumnByName(_("Suppress"));
    if(supColumn == wxNOT_FOUND) {
        return;
    }
    m_dataViewCtrlErrorsModel->GetChildren(wxDataViewItem(0), items);
    for(wxDataViewItemArray::iterator it = items.begin(); it != items.end(); ++it) {
        m_dataViewCtrlErrorsModel->GetValue(variant, *it, supColumn);
        variant.GetBool() ? (marked = true):(unmarked = true);
    }
}

void MemCheckOutputView::OnSuppressError(wxCommandEvent& event)
{
    wxDataViewEvent* menuEvent = dynamic_cast<wxDataViewEvent*>(event.GetEventUserData());
    if(!menuEvent) return;

    wxDataViewItem clickedItem = GetTopParent(menuEvent->GetItem());
    SuppressErrors(SUPPRESS_CLICKED, &clickedItem);
}

void MemCheckOutputView::OnSuppressMarkedErrors(wxCommandEvent& event) { SuppressErrors(SUPPRESS_CHECKED); }

void MemCheckOutputView::OnRowToClip(wxCommandEvent& event)
{
    wxDataViewEvent* menuEvent = dynamic_cast<wxDataViewEvent*>(event.GetEventUserData());
    if(!menuEvent) return;

    if(wxTheClipboard->Open()) {
        MemCheckErrorReferrer* errorRef =
            dynamic_cast<MemCheckErrorReferrer*>(m_dataViewCtrlErrorsModel->GetClientObject(menuEvent->GetItem()));
        if(errorRef) wxTheClipboard->SetData(new wxTextDataObject(errorRef->Get().label));

        MemCheckErrorLocationReferrer* locationRef = dynamic_cast<MemCheckErrorLocationReferrer*>(
            m_dataViewCtrlErrorsModel->GetClientObject(menuEvent->GetItem()));
        if(locationRef) wxTheClipboard->SetData(new wxTextDataObject(locationRef->Get().toString()));

        wxTheClipboard->Close();
    }
}

void MemCheckOutputView::OnErrorToClip(wxCommandEvent& event)
{
    wxDataViewEvent* menuEvent = dynamic_cast<wxDataViewEvent*>(event.GetEventUserData());
    if(!menuEvent) return;

    MemCheckErrorReferrer* errorRef = dynamic_cast<MemCheckErrorReferrer*>(
        m_dataViewCtrlErrorsModel->GetClientObject(GetTopParent(menuEvent->GetItem())));
    if(!errorRef) return;

    if(wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(errorRef->Get().toString()));
        wxTheClipboard->Close();
    }
}

void MemCheckOutputView::OnMarkedErrorsToClip(wxCommandEvent& event)
{
    wxDataViewEvent* menuEvent = dynamic_cast<wxDataViewEvent*>(event.GetEventUserData());
    if(!menuEvent) return;

    wxString text;
    wxVariant variant;
    wxDataViewItemArray items;
    m_dataViewCtrlErrorsModel->GetChildren(wxDataViewItem(0), items);
    int supColumn = GetColumnByName(_("Suppress"));
    if(supColumn == wxNOT_FOUND) {
        return;
    }
    MemCheckErrorReferrer* errorRef;
    for(wxDataViewItemArray::iterator it = items.begin(); it != items.end(); ++it) {
        m_dataViewCtrlErrorsModel->GetValue(variant, *it, supColumn);
        if(variant.GetBool()) {
            errorRef = dynamic_cast<MemCheckErrorReferrer*>(m_dataViewCtrlErrorsModel->GetClientObject(*it));
            text.Append(errorRef->Get().toString());
            text.Append(wxT("\n\n"));
        }
    }
    text.Trim();

    if(wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
    }
}

void MemCheckOutputView::OnOpenPlain(wxCommandEvent& event)
{
    IMemCheckProcessor* processor = m_plugin->GetProcessor();
    if(processor) {
        wxString name = processor->GetOutputLogFileName();
        if(!name.IsEmpty()) m_mgr->OpenFile(name);
    }
}

//////////////////////////////////////////////////

void MemCheckOutputView::OutputViewPageChanged(wxNotebookEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::OutputViewPageChanged()"));

    if(m_notebookOutputView->GetCurrentPage() == m_panelErrors && itemsInvalidView) {
        ResetItemsView();
        ShowPageView(m_currentPage);
    } else if(m_notebookOutputView->GetCurrentPage() == m_panelSupp && itemsInvalidSupp) {
        ResetItemsSupp();
        ApplyFilterSupp(FILTER_STRING);
    }
}

void MemCheckOutputView::OnSuppFileSelected(wxCommandEvent& event)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::OnSuppFileSelected()"));
    m_mgr->OpenFile(m_choiceSuppFile->GetStringSelection());
    m_choiceSuppFile->SetSelection(0);
}

void MemCheckOutputView::OnSuppFileOpen(wxCommandEvent& event)
{
    m_mgr->OpenFile(m_choiceSuppFile->GetStringSelection());
}

void MemCheckOutputView::UpdateStatusSupp()
{
    m_staticTextSuppStatus->SetLabel(wxString::Format(_("Total: %lu  Filtered: %lu  Selected: %d"), m_totalErrorsSupp,
        m_filterResults.size(), m_listCtrlErrors->GetSelectedItemCount()));
    m_staticTextSuppStatus->GetParent()->Layout();
}

void MemCheckOutputView::SuppressErrors(unsigned int mode, wxDataViewItem* dvItem)
{
    if(m_mgr->OpenFile(m_choiceSuppFile->GetStringSelection())) {
        IEditor* editor = m_mgr->GetActiveEditor();
        if(editor) {
            editor->GetCtrl()->DocumentEnd();
            editor->GetCtrl()->Home();
            int posStart = editor->GetCtrl()->GetCurrentPos();
            editor->AppendText(wxString::Format("\n# Added %s", wxDateTime::Now().Format("%F %T")));

            switch(mode) {
            case SUPPRESS_CLICKED: {
                MemCheckErrorReferrer* errorRef =
                    dynamic_cast<MemCheckErrorReferrer*>(m_dataViewCtrlErrorsModel->GetClientObject(*dvItem));
                // TODO ? print error message?
                if(!errorRef) break;
                editor->AppendText(wxString::Format("\n%s", errorRef->Get().getSuppression()));
                errorRef->Get().suppressed = true;
            } break;

            case SUPPRESS_CHECKED: {
                wxVariant variant;
                wxDataViewItemArray items;
                m_dataViewCtrlErrorsModel->GetChildren(wxDataViewItem(0), items);
                int supColumn = GetColumnByName(_("Suppress"));
                if(supColumn == wxNOT_FOUND) {
                    return;
                }

                MemCheckErrorReferrer* errorRef;
                for(wxDataViewItemArray::iterator it = items.begin(); it != items.end(); ++it) {
                    m_dataViewCtrlErrorsModel->GetValue(variant, *it, supColumn);
                    if(variant.GetBool()) {
                        errorRef =
                            dynamic_cast<MemCheckErrorReferrer*>(m_dataViewCtrlErrorsModel->GetClientObject(*it));
                        editor->AppendText(wxString::Format("\n%s", errorRef->Get().getSuppression()));
                        errorRef->Get().suppressed = true;
                    }
                }
            } break;

            case SUPPRESS_ALL:
                for(size_t item = 0; item < m_filterResults.size(); ++item) {
                    editor->AppendText(wxString::Format("\n%s", m_filterResults[item]->getSuppression()));
                    m_filterResults[item]->suppressed = true;
                }
                break;

            case SUPPRESS_SELECTED:
                long item = -1;
                for(;;) {
                    item = m_listCtrlErrors->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                    if(item == -1) break;
                    editor->AppendText(wxString::Format("\n%s", m_filterResults[item]->getSuppression()));
                    m_filterResults[item]->suppressed = true;
                }
                break;
            }

            editor->AppendText(wxT("\n"));
            editor->GetCtrl()->DocumentEnd();
            int textLen = editor->GetCtrl()->GetCurrentPos() - posStart;
            editor->SelectText(posStart, textLen);
            wxCommandEvent saveEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("save_file"));
            m_mgr->GetTheApp()->GetTopWindow()->GetEventHandler()->ProcessEvent(saveEvent);

            // reload pages after suppression
            if(m_plugin->GetSettings()->GetOmitSuppressed()) {
                switch(mode) {
                case SUPPRESS_CLICKED:
                case SUPPRESS_CHECKED:
                    ResetItemsView();
                    ShowPageView(m_currentPage);
                    itemsInvalidSupp = true;
                    break;
                case SUPPRESS_ALL:
                case SUPPRESS_SELECTED:
                    ResetItemsSupp();
                    ApplyFilterSupp(FILTER_STRING);
                    itemsInvalidView = true;
                    break;
                }
            }
        }
    }
}

void MemCheckOutputView::ApplyFilterSupp(unsigned int mode)
{
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::ApplyFilterSupp()"));
    ErrorList& errorList = m_plugin->GetProcessor()->GetErrors();

    // change filter type
    if(mode == FILTER_STRING && m_searchCtrlFilter->GetValue().IsSameAs(wxT(FILTER_NONWORKSPACE_PLACEHOLDER)))
        mode = FILTER_WORKSPACE;
    if(mode == FILTER_STRING && m_searchCtrlFilter->GetValue().IsEmpty()) mode = FILTER_CLEAR;

    size_t iterFlags = 0;
    if(m_plugin->GetSettings()->GetOmitSuppressed()) iterFlags |= MC_IT_OMIT_SUPPRESSED;

    m_filterResults.clear();
    m_listCtrlErrors->SetItemCount(0);

    switch(mode) {
    case FILTER_CLEAR:
        m_searchCtrlFilter->Clear();
        for(MemCheckIterTools::ErrorListIterator it = MemCheckIterTools::Factory(errorList, wxEmptyString, iterFlags);
            it != errorList.end(); ++it)
            m_filterResults.push_back(&*it);
        m_totalErrorsSupp = m_filterResults.size();
        m_checkBoxInvert->SetValue(false);
        m_checkBoxCase->SetValue(false);
        m_checkBoxRegexp->SetValue(false);
        m_checkBoxWord->SetValue(false);
        break;

    case FILTER_WORKSPACE:
        CL_DEBUG1(PLUGIN_PREFIX("m_workspacePath %s", m_workspacePath));
        m_searchCtrlFilter->SetValue(wxT(FILTER_NONWORKSPACE_PLACEHOLDER));
        m_searchCtrlFilter->SelectAll();
        for(MemCheckIterTools::ErrorListIterator it = MemCheckIterTools::Factory(errorList, wxEmptyString, iterFlags);
            it != errorList.end(); ++it) {
            if(m_checkBoxInvert->IsChecked() == (*it).hasPath(m_workspacePath)) m_filterResults.push_back(&*it);
        }
        break;

    case FILTER_STRING:
        size_t flags = 0;
        if(m_checkBoxCase->IsChecked()) flags |= wxSD_MATCHCASE;
        if(m_checkBoxRegexp->IsChecked()) flags |= wxSD_REGULAREXPRESSION;
        if(m_checkBoxWord->IsChecked()) flags |= wxSD_MATCHWHOLEWORD;
        int offset = 0;
        int pos = 0, len = 0;
        if(m_totalErrorsSupp > ITEMS_FOR_WAIT_DIALOG) {
            wxWindowDisabler disableAll;
            wxBusyInfo wait(BUSY_MESSAGE);
            m_mgr->GetTheApp()->Yield();
        }
        size_t i = 0;
        for(MemCheckIterTools::ErrorListIterator it = MemCheckIterTools::Factory(errorList, wxEmptyString, iterFlags);
            it != errorList.end(); ++it) {
            if(m_checkBoxInvert->IsChecked() != StringFindReplacer::Search((*it).toString().wc_str(), offset,
                                                    m_searchCtrlFilter->GetValue().wc_str(), flags, pos, len))
                m_filterResults.push_back(&*it);
            if(m_totalErrorsSupp > ITEMS_FOR_WAIT_DIALOG) {
                ++i;
                if(!(i % WAIT_UPDATE_PER_ITEMS)) m_mgr->GetTheApp()->Yield();
            }
        }
        break;
    }
    m_listCtrlErrors->SetItemCount(m_filterResults.size());
    UpdateStatusSupp();
    itemsInvalidSupp = false;
    // If tooltip is shown, LEAVE event isnt raised. This should help in most cases, because if filter is reset, this
    // means mouse leaved list.
    // m_lastToolTipItem = wxNOT_FOUND;
}

void MemCheckOutputView::OnFilterErrors(wxCommandEvent& event) { ApplyFilterSupp(FILTER_STRING); }

void MemCheckOutputView::OnClearFilter(wxCommandEvent& event) { ApplyFilterSupp(FILTER_CLEAR); }

void MemCheckOutputView::OnSearchNonworkspace(wxCommandEvent& event) { ApplyFilterSupp(FILTER_WORKSPACE); }

void MemCheckOutputView::OnListCtrlErrorsActivated(wxListEvent& event) { /*SuppressErrors(SUPPRESS_SELECTED);*/}

void MemCheckOutputView::OnSuppressAll(wxCommandEvent& event) { SuppressErrors(SUPPRESS_ALL); }

void MemCheckOutputView::OnSuppressSelected(wxCommandEvent& event) { SuppressErrors(SUPPRESS_SELECTED); }

void MemCheckOutputView::OnListCtrlErrorsSelected(wxListEvent& event)
{
    event.Skip();
    UpdateStatusSupp();
}

void MemCheckOutputView::OnListCtrlErrorsDeselected(wxListEvent& event)
{
    event.Skip();
    UpdateStatusSupp();
}

void MemCheckOutputView::OnListCtrlErrorsChar(wxKeyEvent& event) { event.Skip(); }

void MemCheckOutputView::OnListCtrlErrorsKeyDown(wxKeyEvent& event)
{
    event.Skip();
    UpdateStatusSupp();
}

void MemCheckOutputView::OnListCtrlErrorsKeyUp(wxKeyEvent& event)
{
    event.Skip();
    UpdateStatusSupp();
}

void MemCheckOutputView::OnListCtrlErrorsLeftDown(wxMouseEvent& event)
{
    event.Skip();
    UpdateStatusSupp();
}

void MemCheckOutputView::OnListCtrlErrorsLeftUp(wxMouseEvent& event)
{
    event.Skip();
    UpdateStatusSupp();
}

void MemCheckOutputView::OnListCtrlErrorsResize(wxSizeEvent& event)
{
    event.Skip();
    m_listCtrlErrors->SetColumnWidth(0, m_listCtrlErrors->GetClientSize().GetWidth());
}

void MemCheckOutputView::ListCtrlErrorsShowTip(long item)
{
    m_listCtrlErrors->SetToolTip(m_filterResults.at(item)->toText());
}

void MemCheckOutputView::OnListCtrlErrorsMouseLeave(wxMouseEvent& event)
{
    event.Skip();
    // CL_DEBUG1(PLUGIN_PREFIX("MemCheckOutputView::OnListCtrlErrorsMouseLeave()"));
    m_lastToolTipItem = wxNOT_FOUND;
}

void MemCheckOutputView::OnListCtrlErrorsMouseMotion(wxMouseEvent& event)
{
    event.Skip();
    int flags = wxLIST_HITTEST_ONITEM;
    long item = m_listCtrlErrors->HitTest(event.GetPosition(), flags, NULL);

    if(item != m_lastToolTipItem) {
        m_lastToolTipItem = item;
        m_listCtrlErrors->UnsetToolTip();
        if(item != wxNOT_FOUND) CallAfter(&MemCheckOutputView::ListCtrlErrorsShowTip, item);
    }
}

void MemCheckOutputView::OnMemCheckUI(wxUpdateUIEvent& event)
{
    CHECK_CL_SHUTDOWN();

    bool ready = m_plugin->IsReady(event);
    //*
    int id = event.GetId();
    if(id == XRCID("memcheck_expand_all")) {
        ready &= m_notebookOutputView->GetCurrentPage() == m_panelErrors && !m_currentPageIsEmptyView;
    } else if(id == XRCID("memcheck_next")) {
        ready &= m_notebookOutputView->GetCurrentPage() == m_panelErrors && !m_currentPageIsEmptyView;
    } else if(id == XRCID("memcheck_prev")) {
        ready &= m_notebookOutputView->GetCurrentPage() == m_panelErrors && !m_currentPageIsEmptyView;
    } else if(id == XRCID("memcheck_open_plain")) {
        ready &= m_plugin->GetProcessor() && !m_plugin->GetProcessor()->GetOutputLogFileName().IsEmpty();
    }
    event.Enable(ready);
}

void MemCheckOutputView::OnErrorsPanelUI(wxUpdateUIEvent& event)
{
    CHECK_CL_SHUTDOWN();

    bool ready = m_plugin->IsReady(event);
    int id = event.GetId();
    if(id == XRCID("memcheck_page_first")) {
        ready &= !m_currentPageIsEmptyView && m_currentPage > 1;
    } else if(id == XRCID("memcheck_page_prev")) {
        ready &= !m_currentPageIsEmptyView && m_currentPage > 1;
    } else if(id == XRCID("memcheck_page_number")) {
        ready &= m_pageMax > 0;
    } else if(id == XRCID("memcheck_page_next")) {
        ready &= !m_currentPageIsEmptyView && m_currentPage < m_pageMax;
    } else if(id == XRCID("memcheck_page_last")) {
        ready &= !m_currentPageIsEmptyView && m_currentPage < m_pageMax;
    }
    event.Enable(ready);
}

void MemCheckOutputView::OnSuppPanelUI(wxUpdateUIEvent& event)
{
    CHECK_CL_SHUTDOWN();

    bool ready = m_plugin->IsReady(event);
    int id = event.GetId();
    if(id == XRCID("memcheck_supp_open")) {
        ready &= m_choiceSuppFile->GetSelection() != wxNOT_FOUND;
    } else if(id == XRCID("memcheck_filter_supp")) {
        ready &= m_totalErrorsSupp > 0;
    } else if(id == XRCID("memcheck_clear_filter")) {
        ready &= !m_searchCtrlFilter->GetValue().IsEmpty();
    } else if(id == XRCID("memcheck_search_string")) {
        ready &= m_totalErrorsSupp > 0;
    } else if(id == XRCID("memcheck_search_nonworkspace")) {
        ready &= m_totalErrorsSupp > 0;
    } else if(id == XRCID("memcheck_suppress_all")) {
        ready &= m_choiceSuppFile->GetSelection() != wxNOT_FOUND && m_listCtrlErrors->GetItemCount() > 0;
    } else if(id == XRCID("memcheck_suppress_selected")) {
        ready &= m_choiceSuppFile->GetSelection() != wxNOT_FOUND && m_listCtrlErrors->GetItemCount() > 0 &&
            m_listCtrlErrors->GetSelectedItemCount() > 0;
    }
    event.Enable(ready);
}

void MemCheckOutputView::Clear()
{
    m_dataViewCtrlErrorsModel->Clear();
    m_listCtrlErrors->DeleteAllItems();
}
void MemCheckOutputView::OnStop(wxCommandEvent& event) { m_plugin->StopProcess(); }
void MemCheckOutputView::OnStopUI(wxUpdateUIEvent& event) { event.Enable(m_plugin->IsRunning()); }

void MemCheckOutputView::OnClearOutput(wxCommandEvent& event)
{
    Clear();
}

void MemCheckOutputView::OnClearOutputUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_notebookOutputView->GetCurrentPage() == m_panelErrors && m_listCtrlErrors->GetItemCount() > 0);
}
