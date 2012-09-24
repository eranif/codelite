#ifndef __svn_sync_dialog__
#define __svn_sync_dialog__

#include "subversion2_ui.h"
#include "wxcrafter.h"

class Subversion2;
class SvnSyncDialog : public SvnSyncDialogBaseClass
{
    Subversion2 *m_plugin;
private:
    void UpdateUrl(const wxString& rootDir);

protected:
    virtual void OnOkUI(wxUpdateUIEvent& event);
    virtual void OnButtonOK( wxCommandEvent& event );

public:
    /** Constructor */
    SvnSyncDialog( wxWindow* parent, Subversion2 *plugin, const wxString& rootDir, bool excludeBin, const wxString& excludeExtensions );
    virtual ~SvnSyncDialog();

    const wxString& GetRootDir() const {
        return m_rootDir;
    }
    const wxString& GetExcludeExtensions() const {
        return m_excludeExtensions;
    }
    bool GetExcludeBin() const {
        return m_excludeBin;
    }

private:
    wxString m_rootDir;
    wxString m_excludeExtensions;
    bool m_excludeBin;
};

#endif // __svn_sync_dialog__
