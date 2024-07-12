#ifndef DIFFFOLDERSFRAME_H
#define DIFFFOLDERSFRAME_H

#include "DiffUI.h"
#include "bitmap_loader.h"
#include "clFilesCollector.h"
#include "codelite_exports.h"
#include "globals.h"
#include "imanager.h"

#include <thread>

struct WXDLLIMPEXP_SDK DiffViewEntry {
protected:
    bool m_existsInLeft = false;
    bool m_existsInRight = false;
    clFilesScanner::EntryData m_left;
    clFilesScanner::EntryData m_right;

private:
    int GetImageId(const clFilesScanner::EntryData& d) const
    {
        if(d.flags & clFilesScanner::kIsFolder) {
            return clGetManager()->GetStdIcons()->GetMimeImageId(FileExtManager::TypeFolder);
        } else {
            return clGetManager()->GetStdIcons()->GetMimeImageId(d.fullpath);
        }
    }

public:
    bool IsExistsInLeft() const { return m_existsInLeft; }
    bool IsExistsInRight() const { return m_existsInRight; }
    bool IsExistsInBoth() const { return m_existsInRight && m_existsInLeft; }
    bool IsOK() const { return !m_left.fullpath.empty() || !m_right.fullpath.empty(); }
    int GetImageId(bool left) const { return GetImageId(left ? m_left : m_right); }

    wxString GetFullName() const
    {
        if(m_existsInLeft) {
            return wxFileName(GetLeft().fullpath).GetFullName();
        } else {
            return wxFileName(GetRight().fullpath).GetFullName();
        }
    }

    void SetLeft(const clFilesScanner::EntryData& left)
    {
        this->m_left = left;
        m_existsInLeft = true;
    }
    void SetRight(const clFilesScanner::EntryData& right)
    {
        this->m_right = right;
        m_existsInRight = true;
    }
    const clFilesScanner::EntryData& GetLeft() const { return m_left; }
    const clFilesScanner::EntryData& GetRight() const { return m_right; }
    typedef std::vector<DiffViewEntry> Vect_t;
    typedef std::unordered_map<wxString, DiffViewEntry> Hash_t;
};

struct WXDLLIMPEXP_SDK DiffView {
protected:
    DiffViewEntry::Hash_t m_table;

public:
    void CreateEntry(const clFilesScanner::EntryData& d, bool left)
    {
        DiffViewEntry entry;
        if(left) {
            entry.SetLeft(d);
        } else {
            entry.SetRight(d);
        }
        m_table.insert({ entry.GetFullName(), entry });
    }

    DiffViewEntry& GetEntry(const wxString& filename)
    {
        static DiffViewEntry nullEntry;
        if(HasFile(filename)) {
            return m_table[filename];
        }
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
            return a.GetFullName().CmpNoCase(b.GetFullName()) < 0;
        });
        return V;
    }
};

class WXDLLIMPEXP_SDK DiffFoldersFrame : public DiffFoldersBaseDlg
{
    wxString m_leftFolder;
    wxString m_rightFolder;
    size_t m_depth = 0;
    bool m_showSimilarItems = false;
    std::thread* m_checksumThread = nullptr;
    DiffViewEntry::Vect_t m_entries;

public:
    DiffFoldersFrame(wxWindow* parent);
    virtual ~DiffFoldersFrame();
    void OnChecksum(int callId, const wxArrayString& checksumArray);

protected:
    void BuildTrees(const wxString& left, const wxString& right);
    void DoOpenDiff(const wxDataViewItem& item);
    void StopChecksumThread();
    bool CanUp() const;

protected:
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnNewCmparison(wxCommandEvent& event);
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnItemContextMenu(wxDataViewEvent& event);

    void OnMenuDiff(wxCommandEvent& event);
    void OnCopyToRight(wxCommandEvent& event);
    void OnCopyToLeft(wxCommandEvent& event);
    void OnShowSimilarFiles(wxCommandEvent& event);
    void OnShowSimilarFilesUI(wxUpdateUIEvent& event);
    void OnRefresh(wxCommandEvent& event);
    void OnRefreshUI(wxUpdateUIEvent& event);
    void OnUpFolder(wxCommandEvent& event);
    void OnUpFolderUI(wxUpdateUIEvent& event);
};
#endif // DIFFFOLDERSFRAME_H
