#ifndef __quick_outline_dlg__
#define __quick_outline_dlg__

#include "php_ui.h"
class IEditor;
class IManager;
class PHPEntry;

class PHPQuickOutlineDlg : public QuickOutlineDlgBase
{
protected:
    IEditor  *m_editor;
    IManager *m_mgr;

protected:
    // Handlers for QuickOutlineDlgBase events.
    void OnKeyDown( wxKeyEvent& event );
    void OnTextEntered( wxCommandEvent& event );
    void OnItemActivated(wxTreeEvent& event);
    void OnEnter(wxCommandEvent& event);

protected:
    void DoItemSelected(const wxTreeItemId &item);
    void DoSelectMatch(const wxString& filename, int line, const wxString &what);

public:
    PHPQuickOutlineDlg( wxWindow* parent, IEditor *editor, IManager *manager );
    virtual ~PHPQuickOutlineDlg();
};

#endif // __quick_outline_dlg__
