#ifndef IMPORTDLG_H
#define IMPORTDLG_H
#include "wxcrafter.hpp"

class ImportDlg : public ImportDlgBaseClass
{
public:
    struct ImportFileData {
        wxFileName wxcpFilename;
        wxString virtualFolder;
        bool addToProject = false;
        bool loadWhenDone = false;
    };

public:
    enum IPD_ProjectType { IPD_FB, IPD_Smith, IPD_XRC };

protected:
    IPD_ProjectType m_Type;
    bool m_modified = false;
    bool m_showAddToProject = false;

protected:
    void OnDestFilePathUpdated(wxCommandEvent& event) override;
    void OnFileImportTextUpdated(wxCommandEvent& event) override;
    void OnAddFileToProjectUI(wxUpdateUIEvent& event) override;
    void OnBrowseForVirtualFolder(wxCommandEvent& event) override;

public:
    ImportDlg(IPD_ProjectType type, wxWindow* parent, const wxString& sourceFile, bool showAddToProject);
    ~ImportDlg() override = default;

    wxString GetFilepath() const { return m_filepathText->GetValue(); }
    const wxString GetOutputFilepath() const { return m_textName->GetValue(); }

    ImportFileData GetData() const;

protected:
    void OnBrowseForOutputFolder(wxCommandEvent& event) override;
    virtual void OnOKButtonUpdateUI(wxUpdateUIEvent& event);
    void OnBrowse(wxCommandEvent& event) override;
};
#endif // IMPORTDLG_H
