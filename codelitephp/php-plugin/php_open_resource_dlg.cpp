#include "php_open_resource_dlg.h"
#include <globals.h>
#include <windowattrmanager.h>
#include <imanager.h>
#include <wx/imaglist.h>
#include <ieditor.h>
#include <bitmap_loader.h>
#include "php_workspace.h"
#include "php_storage.h"
#include <macros.h>

static int TIMER_ID        = 5647;
static wxBitmap CLASS_IMG_ID    = wxNullBitmap;
static wxBitmap FILE_IMG_ID     = wxNullBitmap;
static wxBitmap FUNC_IMG_ID     = wxNullBitmap;
static wxBitmap CONST_IMG_ID    = wxNullBitmap;
static wxBitmap DEFINE_IMG_ID   = wxNullBitmap;
static wxBitmap VARIABLE_IMG_ID = wxNullBitmap;

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
    WindowAttrManager::Load(this, "PHPOpenResourceDlg", NULL);
}

OpenResourceDlg::OpenResourceDlg( wxWindow* parent, IManager* manager )
    : OpenResourceDlgBase( parent )
    , m_mgr(manager)
    , m_timer(NULL)
{
    wxStringSet_t files;
    PHPWorkspace::Get()->GetWorkspaceFiles( files );

    m_allFiles.reserve(files.size());
    std::set<wxString>::iterator iter = files.begin();
    for(; iter != files.end(); iter++) {
        wxFileName fn((*iter));
        ResourceItem fileItem;
        fileItem.displayName = fn.GetFullName();
        fileItem.filename    = fn;
        fileItem.line        = -1;
        fileItem.type        = PHP_Kind_File;
        m_allFiles.push_back(fileItem);
    }

    DoInitialize();
    m_timer = new wxTimer(this, TIMER_ID);
    m_timer->Start(500, true);
    WindowAttrManager::Load(this, "PHPOpenResourceDlg", NULL);
}

void OpenResourceDlg::DoInitialize()
{
    BitmapLoader *bmpLoader = m_mgr->GetStdIcons();

    CLASS_IMG_ID    = bmpLoader->LoadBitmap(wxT("cc/16/class"));
    FILE_IMG_ID     = bmpLoader->LoadBitmap(wxT("mime/16/php"));
    FUNC_IMG_ID     = bmpLoader->LoadBitmap(wxT("cc/16/function_public"));
    CONST_IMG_ID    = bmpLoader->LoadBitmap(wxT("cc/16/enumerator"));
    DEFINE_IMG_ID   = bmpLoader->LoadBitmap(wxT("cc/16/macro"));
    VARIABLE_IMG_ID = bmpLoader->LoadBitmap(wxT("cc/16/member_public"));

    WindowAttrManager::Load(this, wxT("OpenResourceDlg"), NULL);
    SetSelectedItem(NULL);
}

OpenResourceDlg::~OpenResourceDlg()
{
    wxDELETE(m_timer);

    // list control does not own the client data, we need to free it ourselves
    for(int i=0; i<m_dvListCtrl->GetItemCount(); ++i) {
        ResourceItem *data = (ResourceItem *) m_dvListCtrl->GetItemData( m_dvListCtrl->RowToItem(i) );
        wxDELETE(data);
    }
    m_dvListCtrl->DeleteAllItems();
    WindowAttrManager::Save(this, wxT("OpenResourceDlg"), NULL);
}

void OpenResourceDlg::OnFilterEnter(wxCommandEvent& event)
{
    wxDataViewItem sel = m_dvListCtrl->GetSelection();
    if ( sel.IsOk() ) {
        SetSelectedItem( DoGetItemData(sel) );
        EndModal(wxID_OK);
    }
}

void OpenResourceDlg::OnFilterText(wxCommandEvent& event)
{
    event.Skip();
}

void OpenResourceDlg::OnTimer(wxTimerEvent& event)
{
    event.Skip();
    wxString currentFilter = m_textCtrlFilter->GetValue();
    if(currentFilter != m_lastFilter) {
        m_lastFilter = currentFilter;

        m_dvListCtrl->DeleteAllItems();
        if(m_lastFilter.IsEmpty()) {
            m_timer->Start(500, true);
            return;
        }

        ResourceVector_t filesVec;
        ResourceVector_t allVec;

        // Refresh the list
        filesVec = DoGetFiles(m_lastFilter);
        DoGetResources(m_lastFilter);

        allVec.insert(allVec.end(), filesVec.begin(), filesVec.end());
        allVec.insert(allVec.end(), m_resources.begin(), m_resources.end());
        DoPopulateListCtrl(allVec);

    }
    m_timer->Start(500, true);
}

void OpenResourceDlg::DoPopulateListCtrl(const ResourceVector_t& items)
{
    wxDataViewItem selection;
    for(size_t i=0; i<items.size(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back( ::MakeIconText(items.at(i).displayName, DoGetImgIdx(&items.at(i))) );
        cols.push_back( items.at(i).typeAsString() );
        cols.push_back( items.at(i).filename.GetFullPath() );
        m_dvListCtrl->AppendItem( cols, (wxUIntPtr)(new ResourceItem(items.at(i))) );
        if ( !selection.IsOk() ) {
            selection = m_dvListCtrl->RowToItem(0);
        }
    }

    if( selection.IsOk() ) {
        m_dvListCtrl->Select( selection );
        m_dvListCtrl->EnsureVisible( selection );
    }
}

void OpenResourceDlg::DoGetResources(const wxString& filter)
{
    m_resources.clear();
    PHPStorage::Instance()->GetAllResources(m_resources, filter);
}

ResourceVector_t OpenResourceDlg::DoGetFiles(const wxString& filter)
{
    ResourceVector_t resources;
    // filter non matched entries
    wxString lcFilter = filter;
    lcFilter.MakeLower();

    for(size_t i=0; i<m_allFiles.size(); i++) {
        wxString displayName = m_allFiles.at(i).displayName;
        displayName.MakeLower();
        if(displayName.Contains(lcFilter)) {
            resources.push_back(m_allFiles.at(i));
            // Don't return too many matches...
            if(resources.size() == 150)
                break;
        }
    }
    return resources;
}

ResourceItem* OpenResourceDlg::DoGetItemData(const wxDataViewItem& item)
{
    ResourceItem *data = (ResourceItem *) m_dvListCtrl->GetItemData( item );
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
            m_dvListCtrl->SelectRow( row );
            m_dvListCtrl->EnsureVisible( m_dvListCtrl->RowToItem(row) ) ;
        }
    }
}

void OpenResourceDlg::DoSelectPrev()
{
    wxDataViewItem selecteditem = m_dvListCtrl->GetSelection();
    if(selecteditem.IsOk()) {
        long row = m_dvListCtrl->ItemToRow(selecteditem);
        --row;
        if( row >= 0 ) {
            m_dvListCtrl->SelectRow( row );
            m_dvListCtrl->EnsureVisible( m_dvListCtrl->RowToItem(row) ) ;
        }
    }
}

wxBitmap OpenResourceDlg::DoGetImgIdx(const ResourceItem* item)
{
    switch(item->type) {
    case PHP_Kind_Class:
        return CLASS_IMG_ID;
    case PHP_Kind_Constant:
        return CONST_IMG_ID;
    case PHP_Kind_File:
        return FILE_IMG_ID;
    case PHP_Kind_Function:
        return FUNC_IMG_ID;
    default:
    case PHP_Kind_Member:
    case PHP_Kind_Variable:
        return VARIABLE_IMG_ID;
    }
    return DEFINE_IMG_ID;
}

void OpenResourceDlg::OnDVItemActivated(wxDataViewEvent& event)
{
    SetSelectedItem( DoGetItemData( event.GetItem() ) );
    EndModal(wxID_OK);
}
