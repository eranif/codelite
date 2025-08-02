#ifndef IMPORTDLG_H
#define IMPORTDLG_H
#include "wxcrafter.h"

class ImportDlg : public ImportDlgBaseClass
{
public:
    struct ImportFileData {
        wxFileName wxcpFilename;
        wxString virtualFolder;
        bool addToProject;
        bool loadWhenDone;

        ImportFileData()
            : addToProject(false)
            , loadWhenDone(false)
        {
        }
    };

public:
    enum IPD_ProjectType { IPD_FB, IPD_Smith, IPD_XRC };

protected:
    IPD_ProjectType m_Type;
    bool m_modified;

protected:
    void OnDestFilePathUpdated(wxCommandEvent& event) override;
    void OnFileImportTextUpdated(wxCommandEvent& event) override;
    void OnAddToProjectUI(wxUpdateUIEvent& event) override;
    void OnAddFileToProjectUI(wxUpdateUIEvent& event) override;
    void OnBrowseForVirtualFolder(wxCommandEvent& event) override;

public:
    ImportDlg(IPD_ProjectType type, wxWindow* parent = NULL, const wxString& sourceFile = "");
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
