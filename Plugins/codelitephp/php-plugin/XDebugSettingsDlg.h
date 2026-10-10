#ifndef XDEBUGSETTINGSDLG_H
#define XDEBUGSETTINGSDLG_H

#include "XDebugSettings.h"
#include "php_ui.hpp"

/**
 * @brief edit the Xdebug settings of the open workspace (stored in .codelite/xdebug.json)
 */
class XDebugSettingsDlg : public XDebugSettingsDlgBase
{
    XDebugSettings& m_settings;
    bool m_dirty = false;

    void Save();
    void EditFileMapping(const wxDataViewItem& item);

    void OnChanged(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnApplyUI(wxUpdateUIEvent& event);
    void OnAddIncludePath(wxCommandEvent& event);
    void OnFileMappingMenu(wxDataViewEvent& event);
    void OnFileMappingItemActivated(wxDataViewEvent& event);
    void OnNewFileMapping(wxCommandEvent& event);
    void OnEditFileMapping(wxCommandEvent& event);
    void OnDeleteFileMapping(wxCommandEvent& event);

public:
    /**
     * @param settings loaded settings. The dialog saves them on OK / Apply
     */
    XDebugSettingsDlg(wxWindow* parent, XDebugSettings& settings);
    ~XDebugSettingsDlg() override = default;
};

#endif // XDEBUGSETTINGSDLG_H
