#include "php_open_resource_dlg.h"
#include <globals.h>
#include <windowattrmanager.h>
#include <imanager.h>
#include <wx/imaglist.h>
#include <ieditor.h>
#include <bitmap_loader.h>
#include "php_workspace.h"
#include <macros.h>
#include "PHPLookupTable.h"
#include <wx/tokenzr.h>
#include "FilesCollector.h"
#include "fileutils.h"
#include "cl_config.h"

static int TIMER_ID = 5647;
static wxBitmap CLASS_IMG_ID = wxNullBitmap;
static wxBitmap FUNC_IMG_ID = wxNullBitmap;
static wxBitmap CONST_IMG_ID = wxNullBitmap;
static wxBitmap DEFINE_IMG_ID = wxNullBitmap;
static wxBitmap VARIABLE_IMG_ID = wxNullBitmap;
static wxBitmap NAMESPACE_IMG_ID = wxNullBitmap;

BEGIN_EVENT_TABLE(OpenResourceDlg, OpenResourceDlgBase)
EVT_TIMER(TIMER_ID, OpenResourceDlg::OnTimer)
END_EVENT_TABLE()

OpenResourceDlg::OpenResourceDlg(wxWindow* parent, const ResourceVector_t& items, IManager* manager)
    : OpenResourceDlgBase(parent)
    , m_mgr(manager)
    , m_timer(NULL)
{
    m_resources = items;
    m_textCtrlFilter->Hide();
    m_staticText9->Hide();
    GetSizer()->Fit(this);

    DoInitialize();
    DoPopulateListCtrl(m_resources);
    SetName("OpenResourceDlg");
    WindowAttrManager::Load(this);
}

OpenResourceDlg::OpenResourceDlg(wxWindow* parent, IManager* manager)
    : OpenResourceDlgBase(parent)
    , m_mgr(manager)
    , m_timer(NULL)
{
    wxStringSet_t files;
    PHPWorkspace::Get()->GetWorkspaceFiles(files);
    m_table.Open(PHPWorkspace::Get()->GetFilename().GetPath());
    m_allFiles.reserve(files.size());
    wxStringSet_t::iterator iter = files.begin();
    for(; iter != files.end(); ++iter) {
        wxFileName fn((*iter));
        if(fn.GetFullName() == FOLDER_MARKER) {
            // fake item
            continue;
        }

        ResourceItem fileItem;
        fileItem.displayName = fn.GetFullName();
        fileItem.filename = fn;
        fileItem.line = -1;
        fileItem.type = ResourceItem::kRI_File;
        m_allFiles.push_back(fileItem);
    }

    DoInitialize();
    m_timer = new wxTimer(this, TIMER_ID);
    m_timer->Start(50, true);
    SetName("OpenResourceDlg");
    WindowAttrManager::Load(this);

    wxString lastStringTyped = clConfig::Get().Read("PHP/OpenResourceDialog/SearchString", wxString());

    if(m_mgr->GetActiveEditor() && !m_mgr->GetActiveEditor()->GetSelection().IsEmpty()) {
        wxString sel = m_mgr->GetActiveEditor()->GetSelection();
        m_textCtrlFilter->ChangeValue(sel);
        m_textCtrlFilter->SelectAll();

    } else if(!lastStringTyped.IsEmpty()) {
        m_textCtrlFilter->ChangeValue(lastStringTyped);
        m_textCtrlFilter->SelectAll();
    }
}

void OpenResourceDlg::DoInitialize()
{
    BitmapLoader* bmpLoader = m_mgr->GetStdIcons();
    m_fileImages = bmpLoader->MakeStandardMimeMap();

    CLASS_IMG_ID = bmpLoader->LoadBitmap(wxT("cc/16/class"));
    FUNC_IMG_ID = bmpLoader->LoadBitmap(wxT("cc/16/function_public"));
    CONST_IMG_ID = bmpLoader->LoadBitmap(wxT("cc/16/enumerator"));
    DEFINE_IMG_ID = bmpLoader->LoadBitmap(wxT("cc/16/macro"));
    VARIABLE_IMG_ID = bmpLoader->LoadBitmap(wxT("cc/16/member_public"));
    NAMESPACE_IMG_ID = bmpLoader->LoadBitmap(wxT("cc/16/namespace"));

    SetName("OpenResourceDlg");
    WindowAttrManager::Load(this);
    SetSelectedItem(NULL);
}

OpenResourceDlg::~OpenResourceDlg()
{
    wxDELETE(m_timer);

    // list control does not own the client data, we need to free it ourselves
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        ResourceItem* data = (ResourceItem*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i));
        wxDELETE(data);
    }
    m_dvListCtrl->DeleteAllItems();
    clConfig::Get().Write("PHP/OpenResourceDialog/SearchString", m_textCtrlFilter->GetValue());
}

void OpenResourceDlg::OnFilterEnter(wxCommandEvent& event)
{
    wxDataViewItem sel = m_dvListCtrl->GetSelection();
    if(sel.IsOk()) {
        SetSelectedItem(DoGetItemData(sel));
        EndModal(wxID_OK);
    }
}

void OpenResourceDlg::OnFilterText(wxCommandEvent& event) { event.Skip(); }

void OpenResourceDlg::OnTimer(wxTimerEvent& event)
{
    event.Skip();
    wxString currentFilter = m_textCtrlFilter->GetValue();
    if(currentFilter != m_lastFilter) {
        m_lastFilter = currentFilter;

        m_dvListCtrl->DeleteAllItems();
        if(m_lastFilter.IsEmpty()) {
            m_timer->Start(50, true);
            return;
        }

        ResourceVector_t filesVec;
        ResourceVector_t allVec;

        // Refresh the list
        filesVec = DoGetFiles(m_lastFilter);
        DoGetResources(m_lastFilter);

        allVec.insert(allVec.end(), filesVec.begin(), filesVec.end());
        allVec.insert(allVec.end(), m_resources.begin(), m_resources.end());

        // and sort the results
        wxString lcFilter = m_lastFilter.Lower();
        ResourceVector_t v1, v2, v3, v4, v5;
        std::for_each(allVec.begin(), allVec.end(), [&](const ResourceItem& a) {
            if(a.displayName == m_lastFilter) {
                v1.push_back(a); // Exact match
            } else if(a.displayName.Lower() == lcFilter) {
                v2.push_back(a); // case insenstive exact match
            } else if(a.displayName.StartsWith(m_lastFilter)) {
                v3.push_back(a); // starts with
            } else if(a.displayName.Lower().StartsWith(lcFilter)) {
                v4.push_back(a); // case insenstive starts with
            } else {
                // other
                v5.push_back(a);
            }
        });

        allVec.clear();
        allVec.insert(allVec.end(), v1.begin(), v1.end());
        allVec.insert(allVec.end(), v2.begin(), v2.end());
        allVec.insert(allVec.end(), v3.begin(), v3.end());
        allVec.insert(allVec.end(), v4.begin(), v4.end());
        allVec.insert(allVec.end(), v5.begin(), v5.end());
        DoPopulateListCtrl(allVec);
    }
    m_timer->Start(50, true);
}

void OpenResourceDlg::DoPopulateListCtrl(const ResourceVector_t& items)
{
    wxDataViewItem selection;
    for(size_t i = 0; i < items.size(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back(::MakeIconText(items.at(i).displayName, DoGetImgIdx(&items.at(i))));
        cols.push_back(items.at(i).TypeAsString());
        cols.push_back(items.at(i).filename.GetFullPath());
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)(new ResourceItem(items.at(i))));
        if(!selection.IsOk()) {
            selection = m_dvListCtrl->RowToItem(0);
        }
    }

    if(selection.IsOk()) {
        m_dvListCtrl->Select(selection);
        m_dvListCtrl->EnsureVisible(selection);
    }
}

void OpenResourceDlg::DoGetResources(const wxString& filter)
{
    m_resources.clear();

    PHPEntityBase::List_t matches;
    m_table.LoadAllByFilter(matches, filter);

    // Convert the PHP matches into resources
    PHPEntityBase::List_t::iterator iter = matches.begin();
    m_resources.reserve(matches.size());
    for(; iter != matches.end(); ++iter) {
        PHPEntityBase::Ptr_t match = *iter;
        if(FileUtils::FuzzyMatch(filter, match->GetFullName())) {
            ResourceItem resource;
            resource.displayName = match->GetDisplayName();
            resource.filename = match->GetFilename();
            resource.line = match->GetLine();
            resource.SetType(match);
            m_resources.push_back(resource);
        }
    }
}

ResourceVector_t OpenResourceDlg::DoGetFiles(const wxString& filter)
{
    ResourceVector_t resources;
    // filter non matched entries
    wxString lcFilter = filter;
    lcFilter.MakeLower();

    for(size_t i = 0; i < m_allFiles.size(); i++) {
        wxString filename = m_allFiles.at(i).filename.GetFullPath().Lower();
        if(FileUtils::FuzzyMatch(filter, filename)) {
            resources.push_back(m_allFiles.at(i));
            // Don't return too many matches...
            if(resources.size() == 300) break;
        }
    }
    return resources;
}

ResourceItem* OpenResourceDlg::DoGetItemData(const wxDataViewItem& item)
{
    ResourceItem* data = (ResourceItem*)m_dvListCtrl->GetItemData(item);
    return data;
}

void OpenResourceDlg::OnKeyDown(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_DOWN) {

        // Select next item
        DoSelectNext();

    } else if(event.GetKeyCode() == WXK_UP) {

        // Select next item
        DoSelectPrev();

    } else if(event.GetKeyCode() == WXK_ESCAPE) {
        EndModal(wxID_CANCEL);

    } else {
        event.Skip();
    }
}

void OpenResourceDlg::DoSelectNext()
{
    wxDataViewItem selecteditem = m_dvListCtrl->GetSelection();
    if(selecteditem.IsOk()) {
        long row = m_dvListCtrl->ItemToRow(selecteditem);
        ++row;
        if(m_dvListCtrl->GetItemCount() > row) {
            m_dvListCtrl->SelectRow(row);
            m_dvListCtrl->EnsureVisible(m_dvListCtrl->RowToItem(row));
        }
    }
}

void OpenResourceDlg::DoSelectPrev()
{
    wxDataViewItem selecteditem = m_dvListCtrl->GetSelection();
    if(selecteditem.IsOk()) {
        long row = m_dvListCtrl->ItemToRow(selecteditem);
        --row;
        if(row >= 0) {
            m_dvListCtrl->SelectRow(row);
            m_dvListCtrl->EnsureVisible(m_dvListCtrl->RowToItem(row));
        }
    }
}

wxBitmap OpenResourceDlg::DoGetImgIdx(const ResourceItem* item)
{
    switch(item->type) {
    case ResourceItem::kRI_Namespace:
        return NAMESPACE_IMG_ID;
    case ResourceItem::kRI_Class:
        return CLASS_IMG_ID;
    case ResourceItem::kRI_Constant:
        return CONST_IMG_ID;
    case ResourceItem::kRI_File: {
        FileExtManager::FileType fileType = FileExtManager::GetType(item->filename.GetFullName());
        if(m_fileImages.count(fileType)) {
            // CodeLite has a knowledge about this file type, return the associated image
            return m_fileImages.find(fileType)->second;
        } else {
            // Default text file image
            return m_fileImages.find(FileExtManager::TypeText)->second;
        }
    }
    case ResourceItem::kRI_Function:
        return FUNC_IMG_ID;
    default:
    case ResourceItem::kRI_Member:
    case ResourceItem::kRI_Variable:
        return VARIABLE_IMG_ID;
    }
    return DEFINE_IMG_ID;
}

void OpenResourceDlg::OnDVItemActivated(wxDataViewEvent& event)
{
    SetSelectedItem(DoGetItemData(event.GetItem()));
    EndModal(wxID_OK);
}

bool OpenResourceDlg::IsMatchesFilter(const wxString& filter, const wxString& key)
{
    wxString lcKey = key.Lower();
    wxArrayString filters = ::wxStringTokenize(filter, " ", wxTOKEN_STRTOK);
    for(size_t i = 0; i < filters.GetCount(); ++i) {
        wxString lcFilter = filters.Item(i).Lower();
        if(lcKey.Contains(lcFilter))
            continue;
        else
            return false;
    }
    return true;
}
