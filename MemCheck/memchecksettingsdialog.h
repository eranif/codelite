/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 */

#ifndef _MEMCHECKSETTINGSDIALOG_H_
#define _MEMCHECKSETTINGSDIALOG_H_

#include "memcheckui.h"

class MemCheckSettingsDialog: public MemCheckSettingsDialogBase
{

public:
    MemCheckSettingsDialog(wxWindow *parent,
                           MemCheckSettings *settings);
    virtual ~MemCheckSettingsDialog();

protected:
    virtual void OnFilePickerValgrindOutputFileUI(wxUpdateUIEvent& event);
    
    /**
     * @brief Shows popup menu with two functions: add new supp and remove supp
     * @param event
     */
    virtual void OnSuppListRightDown(wxMouseEvent& event);

    MemCheckSettings *m_settings;

    void OnAddSupp(wxCommandEvent & event);
    void OnDelSupp(wxCommandEvent & event);

    virtual void OnValgrindOutputFileChanged(wxFileDirPickerEvent & event);
    virtual void ValgrindResetOptions(wxCommandEvent & event);
    virtual void OnOK(wxCommandEvent & event);
};
#endif // _MEMCHECKSETTINGSDIALOG_H
