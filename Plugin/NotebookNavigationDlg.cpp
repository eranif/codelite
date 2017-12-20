#include "NotebookNavigationDlg.h"
#include <list>
#include "Notebook.h"
#include <algorithm>
#include "globals.h"
#include <wx/app.h>
#include <wx/dynarray.h>
#include <map>
#include <imanager.h>
#include "bitmap_loader.h"
#include "file_logger.h"

struct TabData {
    wxString label;
    wxBitmap bmp;
    wxFileName filename;
    int index;
    bool isFile;

    TabData()
        : index(wxNOT_FOUND)
        , isFile(false)
    {
    }
};

NotebookNavigationDlg::NotebookNavigationDlg(wxWindow* parent, Notebook* book)
    : NotebookNavigationDlgBase(parent)
    , m_book(book)
    , m_selection(wxNOT_FOUND)
{
    clTab::Vec_t allTabs;
    clGetManager()->GetAllTabs(allTabs);

    BitmapLoader::BitmapMap_t bmps = clGetManager()->GetStdIcons()->MakeStandardMimeMap();

#ifdef __WXOSX__
    wxBitmap saveBmp = wxNullBitmap;
#else
    wxBitmap saveBmp = clGetManager()->GetStdIcons()->LoadBitmap("file_save");
#endif
    std::map<void*, clTab> tabsInfoMap;
    for(size_t i = 0; i < allTabs.size(); ++i) {
        tabsInfoMap.insert(std::make_pair((void*)allTabs.at(i).window, allTabs.at(i)));
    }

    clTabHistory::Ptr_t history = m_book->GetHistory();
    const std::vector<wxWindow*>& windows = history->GetHistory();
    // Populate the list
    for(size_t i = 0; i < windows.size(); ++i) {
        int index = m_book->FindPage(windows[i]);
        if(index != wxNOT_FOUND) {
            wxString label = m_book->GetPageText(index);
            wxBitmap bmp = m_book->GetPageBitmap(index);

            wxVector<wxVariant> cols;
            TabData* d = new TabData;
            d->bmp = bmp;
            d->label = label;
            d->index = index;

            // add extra info
            wxVariant modifiedItem;
            wxVariant nullBmp;
            unsigned char zeros[] = "\0\0";
            nullBmp << wxBitmap{wxImage{1, 1, zeros, zeros, true}};
            std::map<void*, clTab>::iterator iter = tabsInfoMap.find(windows[i]);
            if(iter != tabsInfoMap.end()) {
                d->isFile = iter->second.isFile;
                d->filename = iter->second.filename;
                if(iter->second.isModified) {
                    modifiedItem << saveBmp;
                    cols.push_back(modifiedItem);
                } else {
                    cols.push_back(nullBmp);
                }
            } else {
                cols.push_back(nullBmp);
            }

            // Prepare the display item
            wxString text;
            if(d->isFile && d->filename.GetDirCount()) {
                wxFileName fn(d->filename.GetFullName());
                fn.AppendDir(d->filename.GetDirs().Last());
                text << fn.GetFullPath();
            } else {
                text << d->label;
            }

            // If the tab has a bitmap - use it
            // otherwise, try to assign one
            if(!d->bmp.IsOk()) {
                if(d->isFile) {
                    FileExtManager::FileType type =
                        FileExtManager::GetType(d->filename.GetFullName(), FileExtManager::TypeText);
                    if(bmps.count(type)) {
                        d->bmp = bmps.find(type)->second;
                    }
                }
            }

#ifdef __WXOSX__
            if(iter != tabsInfoMap.end() && iter->second.isModified) {
                text.Prepend("*");
            }
#endif
            cols.push_back(::MakeIconText(text, d->bmp));
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr)d);
        }
    }

    if(m_dvListCtrl->GetItemCount() > 1) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(1));
    } else if(m_dvListCtrl->GetItemCount() == 1) {
        m_dvListCtrl->Select(m_dvListCtrl->RowToItem(0));
    }

    m_dvListCtrl->CallAfter(&wxDataViewCtrl::SetFocus);
    SetMinClientSize(wxSize(500, 300));
#ifdef __WXOSX__
    SetSize(wxSize(500, 300));
#endif
    CentreOnParent();

    wxTheApp->Bind(wxEVT_KEY_DOWN, &NotebookNavigationDlg::OnKeyDown, this);
    wxTheApp->Bind(wxEVT_KEY_UP, &NotebookNavigationDlg::OnKeyUp, this);
    m_dvListCtrl->SetFocus();
}

NotebookNavigationDlg::~NotebookNavigationDlg()
{
    wxTheApp->Unbind(wxEVT_KEY_DOWN, &NotebookNavigationDlg::OnKeyDown, this);
    wxTheApp->Unbind(wxEVT_KEY_UP, &NotebookNavigationDlg::OnKeyUp, this);

    CL_DEBUG("NotebookNavigationDlg::~NotebookNavigationDlg");
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        TabData* d = (TabData*)m_dvListCtrl->GetItemData(m_dvListCtrl->RowToItem(i));
        wxDELETE(d);
    }
    m_dvListCtrl->DeleteAllItems();
}

void NotebookNavigationDlg::CloseDialog()
{
    CL_DEBUG("NotebookNavigationDlg::CloseDialog");
    wxDataViewItem selection = m_dvListCtrl->GetSelection();
    if(selection.IsOk()) {
        TabData* d = (TabData*)m_dvListCtrl->GetItemData(selection);
        m_selection = d->index;
    }
    EndModal(wxID_OK);
}

void NotebookNavigationDlg::OnKeyDown(wxKeyEvent& event)
{
#ifdef __WXOSX__
    if(event.GetUnicodeKey() == WXK_ESCAPE) {
        CallAfter(&NotebookNavigationDlg::CloseDialog);
    } else {
        event.Skip();
    }
#else
    if((event.GetUnicodeKey() == WXK_TAB) && (event.CmdDown() && event.ShiftDown())) {
        // Navigate Up
        wxDataViewItem item = m_dvListCtrl->GetSelection();
        if(item.IsOk()) {
            int row = m_dvListCtrl->ItemToRow(item);
            if(row > 0) {
                --row;
                item = m_dvListCtrl->RowToItem(row);
                m_dvListCtrl->Select(item);
                m_dvListCtrl->EnsureVisible(item);

            } else {
                // Select the last item
                row = m_dvListCtrl->GetItemCount() - 1;
                item = m_dvListCtrl->RowToItem(row);
                m_dvListCtrl->Select(item);
                m_dvListCtrl->EnsureVisible(item);
            }
        }
    } else if((event.GetUnicodeKey() == WXK_TAB) && event.CmdDown()) {
        // Navigate Down
        wxDataViewItem item = m_dvListCtrl->GetSelection();
        if(item.IsOk()) {
            int row = m_dvListCtrl->ItemToRow(item);
            if(row < (m_dvListCtrl->GetItemCount() - 1)) {
                ++row;
                item = m_dvListCtrl->RowToItem(row);
                m_dvListCtrl->Select(item);
                m_dvListCtrl->EnsureVisible(item);
            } else {
                // Select the last item
                item = m_dvListCtrl->RowToItem(0);
                m_dvListCtrl->Select(item);
                m_dvListCtrl->EnsureVisible(item);
            }
        }

    } else {
        event.Skip();
    }
#endif
}

void NotebookNavigationDlg::OnKeyUp(wxKeyEvent& event)
{
    CL_DEBUG("NotebookNavigationDlg::OnKeyUp");
#ifdef __WXOSX__
    event.Skip();
#else
    if(event.GetKeyCode() == WXK_CONTROL) {
        CloseDialog();
    } else {
        event.Skip();
    }
#endif
}

void NotebookNavigationDlg::OnItemActivated(wxDataViewEvent& event)
{
    event.Skip();
    CL_DEBUG("NotebookNavigationDlg::OnItemActivated");
    CloseDialog();
}
