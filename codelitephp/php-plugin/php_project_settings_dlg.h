#ifndef __php_project_settings_dlg__
#define __php_project_settings_dlg__

#include "php_ui.h"

class PHPProjectSettingsDlg : public PHPProjectSettingsBase
{
private:
    bool     m_dirty;
    wxString m_projectName;

protected:
    virtual void OnFileMappingItemActivated(wxDataViewEvent& event);
    virtual void OnFileMappingMenu(wxDataViewEvent& event);
    virtual void OnUseSystemBrowser(wxCommandEvent& event);
    virtual void OnPHPIniSelected(wxFileDirPickerEvent& event);
    virtual void OnUpdateApplyUI(wxCommandEvent& event);
    virtual void OnAddCCPath(wxCommandEvent& event);
    virtual void OnAddIncludePath(wxCommandEvent& event);
    virtual void OnArgumentsEntered(wxCommandEvent& event);
    virtual void OnIndexFileSelected(wxFileDirPickerEvent& event);
    virtual void OnPHPExecChanged(wxFileDirPickerEvent& event);
    virtual void OnPageChanged(wxChoicebookEvent& event);
    virtual void OnPauseWhenExeTerminates(wxCommandEvent& event);
    virtual void OnProjectURLChanged(wxCommandEvent& event);
    virtual void OnWorkingDirectoryChanged(wxFileDirPickerEvent& event);
    
    void OnNewFileMapping(wxCommandEvent &e);
    void OnDeleteFileMapping(wxCommandEvent &e);
    void OnEditFileMapping(wxCommandEvent &e);
    
    void Save();
    void EditItem(const wxDataViewItem &item);
    
protected:
    // Handlers for PHPProjectSettingsBase events.
    void OnOK( wxCommandEvent& event );
    void OnApply( wxCommandEvent& event );
    void OnApplyUI( wxUpdateUIEvent& event );
    
    
public:
    /** Constructor */
    PHPProjectSettingsDlg( wxWindow* parent, const wxString &projectName );
    virtual ~PHPProjectSettingsDlg();

    bool IsDirty() const {
        return m_dirty;
    }
    void SetDirty(bool dirty) {
        this->m_dirty = dirty;
    }
    const wxString& GetProjectName() const {
        return m_projectName;
    }
};

#endif // __php_project_settings_dlg__
