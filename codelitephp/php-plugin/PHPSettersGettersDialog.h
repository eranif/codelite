#ifndef PHPSETTERSGETTERSDIALOG_H
#define PHPSETTERSGETTERSDIALOG_H

#include "php_ui.h"
#include "PHPSetterGetterEntry.h"

class IManager;
class IEditor;
class PHPSettersGettersDialog : public PHPSettersGettersDialogBase
{
    IEditor* m_editor;
    IManager* m_mgr;
    wxString m_scope;

protected:
    void DoPopulate(const PHPEntityBase::List_t& members);
    void Clear();

public:
    PHPSettersGettersDialog(wxWindow* parent, IEditor* editor, IManager* mgr);
    virtual ~PHPSettersGettersDialog();
    PHPSetterGetterEntry::Vec_t GetMembers();
    size_t GetFlags();
    const wxString& GetScope() const { return m_scope; }
};
#endif // PHPSETTERSGETTERSDIALOG_H
