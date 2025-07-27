#ifndef NEWFORMWIZARD_H
#define NEWFORMWIZARD_H

#include "imanager.h"
#include "wxcrafter_gui.h"

struct NewFormDetails {
    wxString virtualFolder;
    wxString filename;
    wxString className;
    wxString inheritedClassName;
    int formType;
    wxString formTitle;
    wxString wxcpFile;
};

class NewFormWizard : public NewFormWizardBaseClass
{
protected:
    void OnBrowseWxcpFile(wxCommandEvent& event) override;
    void OnSelectVDUI(wxUpdateUIEvent& event) override;
    void OnStandloneAppUI(wxUpdateUIEvent& event) override;
    void OnInheritedClassNameUI(wxUpdateUIEvent& event) override;
    void OnFilenameUI(wxUpdateUIEvent& event) override;
    void OnTitleUI(wxUpdateUIEvent& event) override;
    void OnNewWxcpProject(wxCommandEvent& event) override;
    void OnInheritedNameFocus(wxFocusEvent& event) override;
    void OnFilenameFocus(wxFocusEvent& event) override;
    IManager* m_mgr;

    bool IsAuiToolBar() const;
    bool IsPanel() const;
    bool IsFrame() const;
    bool IsDialog() const;
    bool IsPopupWindow() const;
    bool IsImageList() const;
    bool IsWizard() const;

protected:
    void OnFormTypeSelected(wxCommandEvent& event) override;
    void OnSelectVirtualFolder(wxCommandEvent& event) override;
    void OnFinishClicked(wxWizardEvent& event) override;
    void OnWizardPageChanging(wxWizardEvent& event) override;
    void OnMakeSkeletonAppUI(wxUpdateUIEvent& event);

public:
    NewFormWizard(wxWindow* parent, IManager* mgr, int type);
    ~NewFormWizard() override = default;

    NewFormDetails GetFormDetails() const
    {
        NewFormDetails fd;
        fd.className = GetClassName();
        fd.virtualFolder = GetVirtualFolder();
        fd.filename = GetGeneratedFileBaseName();
        fd.formType = GetFormType();
        fd.formTitle = GetTitle();
        fd.wxcpFile = GetWxcpFile();
        fd.inheritedClassName = GetInheritedClassName();
        return fd;
    }

    int GetFormType() const;
    wxString GetVirtualFolder() const;
    wxString GetGeneratedFileBaseName() const;
    wxString GetClassName() const;
    wxString GetTitle() const override;
    wxString GetWxcpFile() const;
    wxString GetInheritedClassName() const;
};
#endif // NEWFORMWIZARD_H
