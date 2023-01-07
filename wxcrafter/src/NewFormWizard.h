#ifndef NEWFORMWIZARD_H
#define NEWFORMWIZARD_H
#include "wxcrafter_gui.h"
#include <imanager.h>

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
    virtual void OnBrowseWxcpFile(wxCommandEvent& event);
    virtual void OnSelectVDUI(wxUpdateUIEvent& event);
    virtual void OnStandloneAppUI(wxUpdateUIEvent& event);
    virtual void OnInheritedClassNameUI(wxUpdateUIEvent& event);
    virtual void OnFilenameUI(wxUpdateUIEvent& event);
    virtual void OnTitleUI(wxUpdateUIEvent& event);
    virtual void OnNewWxcpProject(wxCommandEvent& event);
    virtual void OnInheritedNameFocus(wxFocusEvent& event);
    virtual void OnFilenameFocus(wxFocusEvent& event);
    IManager* m_mgr;

    bool IsAuiToolBar() const;
    bool IsPanel() const;
    bool IsFrame() const;
    bool IsDialog() const;
    bool IsPopupWindow() const;
    bool IsImageList() const;
    bool IsWizard() const;

protected:
    void OnFormTypeSelected(wxCommandEvent& event);
    void OnSelectVirtualFolder(wxCommandEvent& event);
    void OnFinishClicked(wxWizardEvent& event);
    void OnWizardPageChanging(wxWizardEvent& event);
    void OnMakeSkeletonAppUI(wxUpdateUIEvent& event);

public:
    NewFormWizard(wxWindow* parent, IManager* mgr, int type);
    virtual ~NewFormWizard();

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
    wxString GetTitle() const;
    wxString GetWxcpFile() const;
    wxString GetInheritedClassName() const;
};
#endif // NEWFORMWIZARD_H
