#include "DiffFoldersFrame.h"
#include "DiffSelectFoldersDlg.h"
#include "clFilesCollector.h"
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

static int nCallCounter = 0;
static bool checksumThreadStop = false;

struct DiffViewEntry {
protected:
    bool m_existsInLeft = false;
    bool m_existsInRight = false;
    wxString filename;

public:
    void SetFilename(const wxString& filename)
    {
        wxFileName fn(filename);
        this->filename = fn.GetFullName();
    }

    void SetExistsInLeft(bool existsInLeft) { this->m_existsInLeft = existsInLeft; }
    void SetExistsInRight(bool existsInRight) { this->m_existsInRight = existsInRight; }
    const wxString& GetFilename() const { return filename; }
    bool IsExistsInLeft() const { return m_existsInLeft; }
    bool IsExistsInRight() const { return m_existsInRight; }
    bool IsExistsInBoth() const { return m_existsInRight && m_existsInLeft; }
    bool IsOK() const { return !filename.IsEmpty(); }
    int GetImageId() const { return clGetManager()->GetStdIcons()->GetMimeImageId(GetFilename()); }
    typedef std::vector<DiffViewEntry> Vect_t;
    typedef std::unordered_map<wxString, DiffViewEntry> Hash_t;
};

struct DiffView {
protected:
    DiffViewEntry::Hash_t m_table;

public:
    void AddFile(const wxString& filename)
    {
        DiffViewEntry entry;
        entry.SetFilename(filename);
        m_table.insert({ filename, entry });
    }

    DiffViewEntry& GetEntry(const wxString& filename)
    {
        static DiffViewEntry nullEntry;
        if(HasFile(filename)) { return m_table[filename]; }
        return nullEntry;
    }

    bool HasFile(const wxString& filename) const { return m_table.count(filename); }
    DiffViewEntry::Hash_t& GetTable() { return m_table; }
    DiffViewEntry::Vect_t ToSortedVector() const
    {
        DiffViewEntry::Vect_t V;
        std::for_each(m_table.begin(), m_table.end(),
                      [&](const DiffViewEntry::Hash_t::value_type& vt) { V.push_back(vt.second); });
        // sort the vector
        std::sort(V.begin(), V.end(), [&](const DiffViewEntry& a, const DiffViewEntry& b) {
            return a.GetFilename().CmpNoCase(b.GetFilename()) < 0;
        });
        return V;
    }
};

DiffFoldersFrame::DiffFoldersFrame(wxWindow* parent)
    : DiffFoldersBaseDlg(parent)
{
    m_toolbar->SetMiniToolBar(false);
    m_toolbar->AddTool(wxID_NEW, _("New comparison"), clGetManager()->GetStdIcons()->LoadBitmap("file_new"));
    m_toolbar->AddTool(wxID_CLOSE, _("Close"), clGetManager()->GetStdIcons()->LoadBitmap("file_close"));
    m_toolbar->AddTool(wxID_REFRESH, _("Refresh"), clGetManager()->GetStdIcons()->LoadBitmap("file_reload"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("diff-intersection"), _("Show similar files only"),
                       clGetManager()->GetStdIcons()->LoadBitmap("intersection"), "", wxITEM_CHECK);
    m_toolbar->Realize();
    m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnNewCmparison, this, wxID_NEW);
    m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnClose, this, wxID_CLOSE);
    m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnShowSimilarFiles, this, XRCID("diff-intersection"));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DiffFoldersFrame::OnShowSimilarFilesUI, this, XRCID("diff-intersection"));
    m_toolbar->Bind(wxEVT_TOOL, &DiffFoldersFrame::OnRefresh, this, wxID_REFRESH);
    m_toolbar->Bind(wxEVT_UPDATE_UI, &DiffFoldersFrame::OnRefreshUI, this, wxID_REFRESH);

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
        CallAfter(&DiffFoldersFrame::BuildTrees, left, right);
    }
}

static unsigned char GetSimpleChecksum(const wxString& fn)
{
    FILE* fp = fopen(fn.mb_str(), "rb");
    unsigned char checksum = 0;
    while(!feof(fp) && !ferror(fp)) {
        checksum ^= fgetc(fp);
    }
    fclose(fp);
    return checksum;
}

static void HelperThreadCalculateChecksum(int callId, const wxArrayString& items, const wxString& left,
                                          const wxString& right, DiffFoldersFrame* sink)
{
    wxArrayString results;
    for(size_t i = 0; i < items.size(); ++i) {
        if(checksumThreadStop) { break; }
        wxFileName fnLeft(left, items.Item(i));
        wxFileName fnRight(right, items.Item(i));
        if(fnLeft.IsOk() && fnLeft.FileExists() && fnRight.IsOk() && fnRight.FileExists()) {
            unsigned char cksum1 = GetSimpleChecksum(fnLeft.GetFullPath());
            unsigned char cksum2 = GetSimpleChecksum(fnRight.GetFullPath());
            results.Add(cksum1 == cksum2 ? "same" : "different");
        } else {
            results.Add("n/a"); // Dont know
        }
    }
    if(!checksumThreadStop) { sink->CallAfter(&DiffFoldersFrame::OnChecksum, callId, results); }
}

void DiffFoldersFrame::BuildTrees(const wxString& left, const wxString& right)
{
    wxWindowUpdateLocker locker(m_dvListCtrl);
    StopChecksumThread();

    wxBusyCursor bc;
    m_dvListCtrl->DeleteAllItems();
    m_dvListCtrl->SetSortFunction(nullptr);
    m_leftFolder = left;
    m_rightFolder = right;
    m_dvListCtrl->GetColumn(0)->SetLabel(left);
    m_dvListCtrl->GetColumn(1)->SetLabel(right);
    m_dvListCtrl->SetBitmaps(clGetManager()->GetStdIcons()->GetStandardMimeBitmapListPtr());

    // Set up the roots
    wxVector<wxVariant> cols;

    wxArrayString leftFiles;
    wxArrayString rightFiles;

    // Get list of all files in the given folders
    wxDir::GetAllFiles(left, &leftFiles, wxEmptyString, wxDIR_FILES);
    wxDir::GetAllFiles(right, &rightFiles, wxEmptyString, wxDIR_FILES);

    DiffView viewList;

    // Add all the files
    size_t count = wxMax(leftFiles.size(), rightFiles.size());
    for(size_t i = 0; i < count; ++i) {
        if(i < leftFiles.size()) {
            const wxString& filename = leftFiles.Item(i);
            wxString fullname = wxFileName(filename).GetFullName();
            if(viewList.HasFile(fullname)) {
                viewList.GetEntry(fullname).SetExistsInLeft(true);
            } else {
                viewList.AddFile(fullname);
                viewList.GetEntry(fullname).SetExistsInLeft(true);
            }
        }
        if(i < rightFiles.size()) {
            const wxString& filename = rightFiles.Item(i);
            wxString fullname = wxFileName(filename).GetFullName();
            if(viewList.HasFile(fullname)) {
                viewList.GetEntry(fullname).SetExistsInRight(true);
            } else {
                viewList.AddFile(fullname);
                viewList.GetEntry(fullname).SetExistsInRight(true);
            }
        }
    }

    // Sort the merged list
    DiffViewEntry::Vect_t V = viewList.ToSortedVector();
    wxArrayString displayedItems;
    for(size_t i = 0; i < V.size(); ++i) {
        cols.clear();
        const DiffViewEntry& entry = V[i];

        // If the "show similar files" button is clicked, display only files that exists in both lists
        if(m_showSimilarItems && !entry.IsExistsInBoth()) { continue; }
        displayedItems.Add(entry.GetFilename());

        if(entry.IsExistsInLeft()) {
            cols.push_back(::MakeBitmapIndexText(entry.GetFilename(), entry.GetImageId()));
        } else {
            cols.push_back(::MakeBitmapIndexText("", wxNOT_FOUND));
        }

        if(entry.IsExistsInRight()) {
            cols.push_back(::MakeBitmapIndexText(entry.GetFilename(), entry.GetImageId()));
        } else {
            cols.push_back(::MakeBitmapIndexText("", wxNOT_FOUND));
        }
        m_dvListCtrl->AppendItem(cols);
    }

    m_checksumThread = new std::thread(&HelperThreadCalculateChecksum, (++nCallCounter), displayedItems, m_leftFolder,
                                       m_rightFolder, this);
}

void DiffFoldersFrame::OnItemActivated(wxDataViewEvent& event) { DoOpenDiff(event.GetItem()); }

void DiffFoldersFrame::OnItemContextMenu(wxDataViewEvent& event)
{
    wxDataViewItem item = event.GetItem();
    wxString left = m_dvListCtrl->GetItemText(item, 0);
    wxString right = m_dvListCtrl->GetItemText(item, 1);

    wxMenu menu;
    if(right.IsEmpty()) {
        menu.Append(XRCID("diff-copy-left-to-right"), _("Copy from Left to Right"));
        menu.Bind(wxEVT_MENU, &DiffFoldersFrame::OnCopyToRight, this, XRCID("diff-copy-left-to-right"));

    } else if(left.IsEmpty()) {
        menu.Append(XRCID("diff-copy-right-to-left"), _("Copy from Right to Left"));
        menu.Bind(wxEVT_MENU, &DiffFoldersFrame::OnCopyToLeft, this, XRCID("diff-copy-right-to-left"));
    }
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

    wxFileName fnLeft(m_leftFolder, leftFile);
    wxFileName fnRight(m_rightFolder, rightFile);
    clDiffFrame diffFiles(this, fnLeft, fnRight, false);
    diffFiles.ShowModal();
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
    checksumThreadStop = true;
    if(m_checksumThread) { m_checksumThread->join(); }
    checksumThreadStop = false;
    wxDELETE(m_checksumThread);
}
