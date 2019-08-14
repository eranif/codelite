#include "DiffFoldersFrame.h"
#include "DiffSelectFoldersDlg.h"
#include "clFilesCollector.h"
#include "fileextmanager.h"
#include <wx/dir.h>
#include <algorithm>
#include "globals.h"
#include <imanager.h>
#include "bitmap_loader.h"
#include <wxStringHash.h>
#include "clDiffFrame.h"
#include "cl_config.h"
#include "clToolBarButtonBase.h"
#include <wx/wupdlock.h>
#include <macros.h>
#include "globals.h"
#include <atomic>

static int nCallCounter = 0;
static std::atomic_bool checksumThreadStop;

DiffFoldersFrame::DiffFoldersFrame(wxWindow* parent)
    : DiffFoldersBaseDlg(parent)
{
    checksumThreadStop.store(false);
    m_toolbar->SetMiniToolBar(false);
    m_toolbar->AddTool(wxID_NEW, _("New comparison"), clGetManager()->GetStdIcons()->LoadBitmap("file_new"));
    m_toolbar->AddTool(wxID_CLOSE, _("Close"), clGetManager()->GetStdIcons()->LoadBitmap("file_close"));
    m_toolbar->AddTool(wxID_REFRESH, _("Refresh"), clGetManager()->GetStdIcons()->LoadBitmap("file_reload"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("diff-intersection"), _("Show similar files only"),
                       clGetManager()->GetStdIcons()->LoadBitmap("intersection"), "", wxITEM_CHECK);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("diff-up-folder"), _("Parent folder"), clGetManager()->GetStdIcons()->LoadBitmap("up"));

    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnNewCmparison, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnClose, this, wxID_CLOSE);
    m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnShowSimilarFiles, this, XRCID("diff-intersection"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DiffFoldersFrame::OnShowSimilarFilesUI, this, XRCID("diff-intersection"));
    m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnRefresh, this, wxID_REFRESH);
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DiffFoldersFrame::OnRefreshUI, this, wxID_REFRESH);
    m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnUpFolder, this, XRCID("diff-up-folder"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DiffFoldersFrame::OnUpFolderUI, this, XRCID("diff-up-folder"));

    ::clSetTLWindowBestSizeAndPosition(this);

    // Load persistent items
    m_showSimilarItems = clConfig::Get().Read("DiffFolders/ShowSimilarItems", false);
}

DiffFoldersFrame::~DiffFoldersFrame()
{
    clConfig::Get().Write("DiffFolders/ShowSimilarItems", m_showSimilarItems);
    StopChecksumThread();
}

void DiffFoldersFrame::OnClose(wxCommandEvent& event) { EndModal(wxID_OK); }

void DiffFoldersFrame::OnNewCmparison(wxCommandEvent& event)
{
    wxString left = clConfig::Get().Read("DiffFolders/Left", wxString());
    wxString right = clConfig::Get().Read("DiffFolders/Right", wxString());
    DiffSelectFoldersDlg dlg(this, left, right);
    if(dlg.ShowModal() == wxID_OK) {
        left = dlg.GetDirPickerLeft()->GetPath();
        right = dlg.GetDirPickerRight()->GetPath();
        clConfig::Get().Write("DiffFolders/Left", left);
        clConfig::Get().Write("DiffFolders/Right", right);
        m_depth = 0;
        CallAfter(&DiffFoldersFrame::BuildTrees, left, right);
    }
}

#define CLOSE_FP(fp)      \
    {                     \
        if(fp) {          \
            fclose(fp);   \
            fp = nullptr; \
        }                 \
    }

static bool CompareFilesCheckSum(const wxString& fn1, const wxString& fn2)
{
    // The sizes are the same
    unsigned char checksum1 = 0;
    unsigned char checksum2 = 0;

    FILE* fp1 = fopen(fn1.mb_str(), "rb");
    FILE* fp2 = fopen(fn2.mb_str(), "rb");
    if(!fp1 || !fp2) {
        CLOSE_FP(fp1);
        CLOSE_FP(fp2);
        return false;
    }

    while(!feof(fp1) && !ferror(fp1) && !feof(fp2) && !ferror(fp2)) {
        checksum1 ^= fgetc(fp1);
        checksum2 ^= fgetc(fp2);
        if(checksum1 != checksum2) { break; }
    }
    CLOSE_FP(fp1);
    CLOSE_FP(fp2);
    return (checksum1 == checksum2);
}

static void HelperThreadCalculateChecksum(int callId, const wxArrayString& items, const wxString& left,
                                          const wxString& right, DiffFoldersFrame* sink)
{
    wxArrayString results;
    for(size_t i = 0; i < items.size(); ++i) {
        if(checksumThreadStop.load()) { break; }
        wxFileName fnLeft(left, items.Item(i));
        wxFileName fnRight(right, items.Item(i));
        if(fnLeft.IsOk() && fnLeft.FileExists() && fnRight.IsOk() && fnRight.FileExists()) {
            if(fnLeft.GetSize() != fnRight.GetSize()) {
                // If the size is different, no need to go further
                results.Add("different");
            } else {
                bool isSame = CompareFilesCheckSum(fnLeft.GetFullPath(), fnRight.GetFullPath());
                results.Add(isSame ? "same" : "different");
            }
        } else {
            results.Add("n/a"); // Dont know
        }
    }
    if(!checksumThreadStop.load()) { sink->CallAfter(&DiffFoldersFrame::OnChecksum, callId, results); }
}

void DiffFoldersFrame::BuildTrees(const wxString& left, const wxString& right)
{
    StopChecksumThread();
    wxBusyCursor bc;
    m_dvListCtrl->DeleteAllItems();
    m_entries.clear();
    m_dvListCtrl->SetSortFunction(nullptr);
    m_leftFolder = left;
    m_rightFolder = right;
    m_dvListCtrl->GetColumn(0)->SetLabel(left);
    m_dvListCtrl->GetColumn(1)->SetLabel(right);
    m_dvListCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());

    // Set up the roots
    wxVector<wxVariant> cols;

    clFilesScanner::EntryData::Vec_t leftFiles;
    clFilesScanner::EntryData::Vec_t rightFiles;

    clFilesScanner scanner;
    scanner.ScanNoRecurse(left, leftFiles);
    scanner.ScanNoRecurse(right, rightFiles);

    // Get list of all files in the given folders
    DiffView viewList;

    // Add all the files
    size_t count = wxMax(leftFiles.size(), rightFiles.size());
    for(size_t i = 0; i < count; ++i) {
        if(i < leftFiles.size()) {
            const clFilesScanner::EntryData& entry = leftFiles[i];
            wxFileName fn(entry.fullpath);
            wxString fullname = fn.GetFullName();
            if(!viewList.HasFile(fullname)) { viewList.CreateEntry(entry, true); }
            viewList.GetEntry(fullname).SetLeft(entry);
        }

        if(i < rightFiles.size()) {
            const clFilesScanner::EntryData& entry = rightFiles[i];
            wxFileName fn(entry.fullpath);
            wxString fullname = fn.GetFullName();
            if(!viewList.HasFile(fullname)) { viewList.CreateEntry(entry, false); }
            viewList.GetEntry(fullname).SetRight(entry);
        }
    }

    // Sort the merged list
    m_entries = viewList.ToSortedVector();
    wxArrayString displayedItems;
    for(size_t i = 0; i < m_entries.size(); ++i) {
        cols.clear();
        const DiffViewEntry& entry = m_entries[i];

        // If the "show similar files" button is clicked, display only files that exists in both lists
        if(m_showSimilarItems && !entry.IsExistsInBoth()) { continue; }

        // This will be passed to the checksum thread
        displayedItems.Add(entry.GetFullName());

        if(entry.IsExistsInLeft()) {
            cols.push_back(::MakeBitmapIndexText(entry.GetLeft().fullpath, entry.GetImageId(true)));
        } else {
            cols.push_back(::MakeBitmapIndexText("", wxNOT_FOUND));
        }

        if(entry.IsExistsInRight()) {
            cols.push_back(::MakeBitmapIndexText(entry.GetRight().fullpath, entry.GetImageId(false)));
        } else {
            cols.push_back(::MakeBitmapIndexText("", wxNOT_FOUND));
        }
        m_dvListCtrl->AppendItem(cols, (wxUIntPtr)&entry);
    }

    m_checksumThread = new std::thread(&HelperThreadCalculateChecksum, (++nCallCounter), displayedItems, m_leftFolder,
                                       m_rightFolder, this);
}

void DiffFoldersFrame::OnItemActivated(wxDataViewEvent& event)
{
    DiffViewEntry* entry = reinterpret_cast<DiffViewEntry*>(m_dvListCtrl->GetItemData(event.GetItem()));
    if(!entry) { return; }

    if(entry->IsExistsInBoth() && (entry->GetLeft().flags & clFilesScanner::kIsFolder) &&
       (entry->GetRight().flags & clFilesScanner::kIsFolder)) {
        // Refresh the view to the current folder
        wxFileName left(m_leftFolder, "");
        wxFileName right(m_rightFolder, "");
        left.AppendDir(entry->GetFullName());
        right.AppendDir(entry->GetFullName());
        m_leftFolder = left.GetPath();
        m_rightFolder = right.GetPath();
        ++m_depth;
        CallAfter(&DiffFoldersFrame::BuildTrees, m_leftFolder, m_rightFolder);
    } else {
        DoOpenDiff(event.GetItem());
    }
}

void DiffFoldersFrame::OnItemContextMenu(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    wxString left = m_dvListCtrl->GetItemText(item, 0);
    wxString right = m_dvListCtrl->GetItemText(item, 1);

    wxMenu menu;
    if(!right.IsEmpty()) {
        menu.Append(XRCID("diff-copy-right-to-left"), _("Copy from Right to Left"));
        menu.Bind(wxEVT_MENU, &DiffFoldersFrame::OnCopyToLeft, this, XRCID("diff-copy-right-to-left"));
    }

    if(!left.IsEmpty()) {
        menu.Append(XRCID("diff-copy-left-to-right"), _("Copy from Left to Right"));
        menu.Bind(wxEVT_MENU, &DiffFoldersFrame::OnCopyToRight, this, XRCID("diff-copy-left-to-right"));
    }
    if(menu.GetMenuItemCount()) { menu.AppendSeparator(); }

    if(!right.IsEmpty() && !left.IsEmpty()) {
        menu.Append(XRCID("diff-open-diff"), _("Diff"));
        menu.Bind(wxEVT_MENU, &DiffFoldersFrame::OnMenuDiff, this, XRCID("diff-open-diff"));
    }
    if(menu.GetMenuItemCount() == 0) { return; }
    m_dvListCtrl->PopupMenu(&menu);
}

void DiffFoldersFrame::OnShowSimilarFiles(wxCommandEvent& event)
{
    event.Skip();
    m_showSimilarItems = event.IsChecked();
    BuildTrees(m_leftFolder, m_rightFolder);
}

void DiffFoldersFrame::OnShowSimilarFilesUI(wxUpdateUIEvent& event)
{
    if(m_leftFolder.IsEmpty() || m_rightFolder.IsEmpty()) {
        event.Check(false);
        event.Enable(false);
    } else {

        event.Enable(true);
        event.Check(m_showSimilarItems);
    }
}

void DiffFoldersFrame::DoOpenDiff(const wxDataViewItem& item)
{
    if(!item.IsOk()) { return; }
    wxString leftFile = m_dvListCtrl->GetItemText(item, 0);
    wxString rightFile = m_dvListCtrl->GetItemText(item, 1);
    if(leftFile.IsEmpty() || rightFile.IsEmpty()) { return; }

    wxFileName fnLeft(leftFile);
    wxFileName fnRight(rightFile);
    clDiffFrame *diffFiles = new clDiffFrame(this, fnLeft, fnRight, false);
    diffFiles->Show();
}

void DiffFoldersFrame::OnMenuDiff(wxCommandEvent& event) { DoOpenDiff(m_dvListCtrl->GetSelection()); }

void DiffFoldersFrame::OnCopyToRight(wxCommandEvent& event)
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    CHECK_ITEM_RET(item);
    wxString fullname = m_dvListCtrl->GetItemText(item, 0);
    wxFileName source(m_leftFolder, fullname);
    wxFileName target(m_rightFolder, fullname);
    if(::wxCopyFile(source.GetFullPath(), target.GetFullPath())) { m_dvListCtrl->SetItemText(item, fullname, 1); }
}

void DiffFoldersFrame::OnCopyToLeft(wxCommandEvent& event)
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    CHECK_ITEM_RET(item);
    wxString fullname = m_dvListCtrl->GetItemText(item, 1);
    wxFileName source(m_rightFolder, fullname);
    wxFileName target(m_leftFolder, fullname);
    if(::wxCopyFile(source.GetFullPath(), target.GetFullPath())) { m_dvListCtrl->SetItemText(item, fullname, 0); }
}

void DiffFoldersFrame::OnChecksum(int callId, const wxArrayString& checksumArray)
{
    if(callId != nCallCounter) { return; }
    bool isDark = DrawingUtils::IsDark(m_dvListCtrl->GetColours().GetBgColour());
    wxColour modifiedColour = isDark ? wxColour("rgb(255, 128, 64)") : *wxRED;
    for(size_t i = 0; i < checksumArray.size(); ++i) {
        const wxString& answer = checksumArray.Item(i);
        if(answer == "different") {
            wxDataViewItem item = m_dvListCtrl->RowToItem(i);
            if(item.IsOk()) {
                m_dvListCtrl->SetItemTextColour(item, modifiedColour, 0);
                m_dvListCtrl->SetItemTextColour(item, modifiedColour, 1);
            }
        }
    }
}

void DiffFoldersFrame::OnRefresh(wxCommandEvent& event)
{
    wxUnusedVar(event);
    BuildTrees(m_leftFolder, m_rightFolder);
}

void DiffFoldersFrame::OnRefreshUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_leftFolder.IsEmpty() && !m_rightFolder.IsEmpty());
}

void DiffFoldersFrame::StopChecksumThread()
{
    checksumThreadStop.store(false);
    if(m_checksumThread) { m_checksumThread->join(); }
    checksumThreadStop.store(false);
    wxDELETE(m_checksumThread);
}

void DiffFoldersFrame::OnUpFolder(wxCommandEvent& event)
{
    if(!CanUp()) { return; }

    wxFileName fnLeft(m_leftFolder, "");
    wxFileName fnRight(m_rightFolder, "");

    fnLeft.RemoveLastDir();
    fnRight.RemoveLastDir();
    --m_depth;

    BuildTrees(fnLeft.GetPath(), fnRight.GetPath());
}

void DiffFoldersFrame::OnUpFolderUI(wxUpdateUIEvent& event) { event.Enable(CanUp()); }

bool DiffFoldersFrame::CanUp() const
{
    wxFileName fnLeft(m_leftFolder, "");
    wxFileName fnRight(m_rightFolder, "");
    return m_depth && fnLeft.GetDirCount() && fnRight.GetDirCount();
}
