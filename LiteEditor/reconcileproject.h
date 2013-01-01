#ifndef RECONCILEPROJECT_CLASSES_H
#define RECONCILEPROJECT_CLASSES_H

#include "reconcileprojectbase.h"
#include "project.h"
#include <deque>
#include <vector>

typedef std::vector< std::pair<wxString, wxArrayString> > StaleVDFiles;

class ReconcileProjectPanel;

class ReconcileProjectDlg : public ReconcileProjectDlgBaseClass
{
public:
    ReconcileProjectDlg(wxWindow* parent, const wxString& projname);
    virtual ~ReconcileProjectDlg();

    /*!
     * \brief Searches the filesystem and project to find new and stale files, then fills the dialog's fields with them
     * \return success
     */
    bool LoadData();
    void DistributeFiles();
    void IsReconciliationComplete();
    wxArrayString& GetAllNewFiles() { return m_actualFiles; }
    const wxArrayString& GetUnallocatedFiles() { return m_unallocatedFiles; }
    ReconcileProjectPanel* GetRootPanel() const { return m_rootPanel; }
    /*!
     * \brief Removes StaleFiles from project
     * \param files the files to be added
     * \param vdPath the virtual dir to which to add them
     * \return A wxArrayString containing the items successfully removed
     */
    wxArrayString AddMissingFiles(const wxArrayString& files, const wxString& vdPath);
    /*!
     * \brief Removes StaleFiles from project
     * \param StaleFiles the items that no longer exist and that the user has chosen to remove
     * \return A wxArrayString containing the items successfully removed
     */
    wxArrayString RemoveStaleFiles(const wxArrayString& StaleFiles) const;

protected:
    /*!
     * \brief Remove from m_actualFiles all those which are already in the project
     * \param toplevelDir to make-relative to
     */
    void PruneExistingItems(StringSet_t &allfiles);
    void DisplayVirtualDirectories();
    void DistributeFilesByRegex(ReconcileProjectPanel* rootpanel, const wxArrayString& regexes, wxArrayString& files) const;
    void FindStaleFiles();
    wxTreebook* GetTreebook() const { return m_treebook84; }

    ReconcileProjectPanel* m_rootPanel;
    const wxString m_projname;
    wxArrayString m_regexes;
    wxArrayString m_actualFiles;
    wxArrayString m_unallocatedFiles;
    StaleVDFiles* m_staleFiles;         // Holds VD name and its stale filenames
};


class ReconcileProjectPanel : public ReconcileProjectPanelBaseClass
{
public:
    ReconcileProjectPanel(wxWindow* parent, const wxString& displayname, const wxString& vdPath);

    void SetFiles(wxArrayString& files);
    void SetStaleFiles(wxString& files);
    size_t GetStaleFiles(wxArrayString& files) const;
    size_t GetStaleFilesCount() const;
    void StoreChild(ReconcileProjectPanel* childpanel);
    void UnstoreChild(wxWindow* childpanel);
    bool AllocateFileByVD(const wxString& file, const wxString& VD);
    const wxString GetVDDisplayName() { return m_displayname; }
    bool GetIsShowingAll() const { return m_radioShowAll->GetValue(); }
    bool GetHideEmpties() const { return !m_checkBoxShowAllVDs->IsChecked(); }
    bool GetHasFiles() const { return m_hasItems; }

protected:
    /*!
     * \brief If a virtual dir foo has relevant children called e.g. src or includes, put its cpp/h files in the appropriate subdir
     * \param files the files to distribute
     */
    void DistributeFilesToScrInclude(wxArrayString& files);
    /*!
     * \brief If this panel's virtual-dir path is foo/bar/, allocate to it all files with the path foo/bar/
     * \param files the files to distribute
     */
    void DistributeFilesToExactMatches(wxArrayString& files);
    bool GetHasItems(); // Does this panel have children or grandchildren? Caches the result internally too

    bool IsSourceVD(const wxString& name) const;
    bool IsHeaderVD(const wxString& name) const;
    bool IsResourceVD(const wxString& name) const;
    wxCheckListBox* GetActiveChkListBox() const;

    virtual void OnProcessButtonClicked(wxCommandEvent& event);
    virtual void OnRemoveStaleButtonClicked(wxCommandEvent& event);
    virtual void OnProcessButtonUpdateUI(wxUpdateUIEvent& event);
    virtual void OnSelectAll(wxCommandEvent& event);
    virtual void OnSelectAllUpdateUI(wxUpdateUIEvent& event);
    virtual void OnUnselectAll(wxCommandEvent& event);
    virtual void OnUnselectAllUpdateUI(wxUpdateUIEvent& event);
    virtual void OnShowAllInRoot(wxCommandEvent& WXUNUSED(event)) { DoShowAllInRoot(); }
    virtual void DoShowAllInRoot();
    virtual void OnShowUnallocdInRoot(wxCommandEvent& event);
    virtual void OnShowVDsClicked(wxCommandEvent& event);
    
    wxString m_displayname;
    const wxString m_vdPath;    // The panel node's VD path e.g. projectname:foo:bar:src
    std::deque<ReconcileProjectPanel*> m_children; // Child panels in the correct order for processing
    bool m_hasItems; // Whether this panel, or its children, contain files
};


class ReconcileProjectFiletypesDlg : public ReconcileProjectFiletypesDlgBaseClass
{
public:
    ReconcileProjectFiletypesDlg(wxWindow* parent, const wxString& projname);
    virtual ~ReconcileProjectFiletypesDlg();

    void SetData();
    void GetData(wxString& toplevelDir, wxString& types, wxArrayString& excludePaths, wxArrayString& regexes) const;

protected:
    void SetRegex(const wxString& regex);   // Takes a VD|regex string, splits and inserts into listctrl cols
    wxArrayString GetRegexes() const;       // Extracts the regex and VD data from listctrl, joins each to VD|regex string, and puts in arraystring
    virtual void OnIgnoreBrowse(wxCommandEvent& event);
    virtual void OnIgnoreRemove(wxCommandEvent& event);
    virtual void OnAddRegex(wxCommandEvent& event);
    virtual void OnRemoveRegex(wxCommandEvent& event);
    virtual void OnIgnoreRemoveUpdateUI(wxUpdateUIEvent& event);
    virtual void OnRemoveRegexUpdateUI(wxUpdateUIEvent& event);

    const wxString m_projname;
};

class ReconcileByRegexDlg : public ReconcileByRegexDlgBaseClass
{
public:
    ReconcileByRegexDlg(wxWindow* parent, const wxString& projname);
    virtual ~ReconcileByRegexDlg();
    
    wxString GetRegex() { return m_textCtrlVirtualFolder->GetValue() + '|' + m_textCtrlRegex->GetValue(); }

protected:
    virtual void OnTextEnter(wxCommandEvent& event);
    virtual void OnVDBrowse(wxCommandEvent& event);
    virtual void OnRegexOKCancelUpdateUI(wxUpdateUIEvent& event);

    const wxString m_projname;
};

#endif
