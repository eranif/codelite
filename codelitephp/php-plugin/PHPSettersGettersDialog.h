#ifndef PHPSETTERSGETTERSDIALOG_H
#define PHPSETTERSGETTERSDIALOG_H

#include "php_ui.h"
#include "PHPSetterGetterEntry.h"

class PHPSettersGettersDialog : public PHPSettersGettersDialogBase
{
    IEditor* m_editor;
protected:
    void DoPopulate(const PHPEntityBase::List_t& members);
    
public:
    PHPSettersGettersDialog(wxWindow* parent, IEditor* editor);
    virtual ~PHPSettersGettersDialog();
};
#endif // PHPSETTERSGETTERSDIALOG_H
