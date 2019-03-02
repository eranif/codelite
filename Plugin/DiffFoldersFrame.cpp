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

DiffFoldersFrame::DiffFoldersFrame(wxWindow* parent)
    : DiffFoldersBaseFrame(parent)
{
}

DiffFoldersFrame::~DiffFoldersFrame() {}

void DiffFoldersFrame::OnClose(wxCommandEvent& event) { Close(); }

void DiffFoldersFrame::OnNewCmparison(wxCommandEvent& event)
{
    DiffSelectFoldersDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        CallAfter(&DiffFoldersFrame::BuildTrees, dlg.GetDirPickerLeft()->GetPath(), dlg.GetDirPickerRight()->GetPath());
    }
}
#define LEFT_COL 0
#define RIGHT_COL 1

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

void DiffFoldersFrame::BuildTrees(const wxString& left, const wxString& right)
{
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

    // Add all the left entries
    for(size_t i = 0; i < leftFiles.size(); ++i) {
        const wxString& filename = leftFiles.Item(i);
        wxString fullname = wxFileName(filename).GetFullName();
        viewList.AddFile(fullname);
        viewList.GetEntry(fullname).SetExistsInLeft(true);
    }

    // add the right entries
    for(size_t i = 0; i < rightFiles.size(); ++i) {
        const wxString& filename = rightFiles.Item(i);
        wxString fullname = wxFileName(filename).GetFullName();
        if(viewList.HasFile(fullname)) {
            viewList.GetEntry(fullname).SetExistsInRight(true);
        } else {
            viewList.AddFile(fullname);
            viewList.GetEntry(fullname).SetExistsInRight(true);
        }
    }

    // Sort the merged list
    DiffViewEntry::Vect_t V = viewList.ToSortedVector();

    for(size_t i = 0; i < V.size(); ++i) {
        cols.clear();
        const DiffViewEntry& entry = V[i];
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
}

void DiffFoldersFrame::OnItemActivated(wxDataViewEvent& event)
{
    wxString leftFile = m_dvListCtrl->GetItemText(event.GetItem(), 0);
    wxString rightFile = m_dvListCtrl->GetItemText(event.GetItem(), 1);
    if(leftFile.IsEmpty() || rightFile.IsEmpty()) { return; }

    wxFileName fnLeft(m_leftFolder, leftFile);
    wxFileName fnRight(m_rightFolder, rightFile);
    clDiffFrame* diffFiles = new clDiffFrame(this, fnLeft, fnRight, false);
    diffFiles->Show();
}

void DiffFoldersFrame::OnItemContextMenu(wxDataViewEvent& event) {}
