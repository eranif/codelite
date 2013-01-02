#ifndef RECONCILEPROJECT_CLASSES_H
#define RECONCILEPROJECT_CLASSES_H

#include "reconcileprojectbase.h"
#include "project.h"
#include <deque>
#include <vector>
#include "bitmap_loader.h"

class ReconcileProjectDlg : public ReconcileProjectDlgBaseClass
{
protected:
    virtual void OnDone(wxCommandEvent& event);
    virtual void OnAddFile(wxCommandEvent& event);
    virtual void OnAddFileUI(wxUpdateUIEvent& event);
    virtual void OnAutoAssignUI(wxUpdateUIEvent& event);
    virtual void OnAutoSuggest(wxCommandEvent& event);
    virtual void OnUndoSelectedFiles(wxCommandEvent& event);
    virtual void OnUndoSelectedFilesUI(wxUpdateUIEvent& event);

    wxString      m_projname;
    wxArrayString m_regexes;
    StringSet_t   m_newfiles;
    StringSet_t   m_stalefiles;
    StringSet_t   m_allfiles;
    BitmapLoader::BitmapMap_t m_bitmaps;
    wxString      m_toplevelDir;
    
    typedef std::multimap<wxString, wxString> StringMultimap_t;
    
protected:
    void DoFindFiles();
    wxBitmap GetBitmap(const wxString &filename) const;

public:
    ReconcileProjectDlg(wxWindow* parent, const wxString& projname);
    virtual ~ReconcileProjectDlg();

    /*!
     * \brief Searches the filesystem and project to find new and stale files, then fills the dialog's fields with them
     * \return success
     */
    bool LoadData();
    void DistributeFiles();

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

    wxString GetRegex() {
        return m_textCtrlVirtualFolder->GetValue() + '|' + m_textCtrlRegex->GetValue();
    }

protected:
    virtual void OnTextEnter(wxCommandEvent& event);
    virtual void OnVDBrowse(wxCommandEvent& event);
    virtual void OnRegexOKCancelUpdateUI(wxUpdateUIEvent& event);

    const wxString m_projname;
};

#endif
