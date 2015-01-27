#include "PHPSettersGettersDialog.h"
#include "windowattrmanager.h"
#include "php_code_completion.h"
#include "php_configuration_data.h"

PHPSettersGettersDialog::PHPSettersGettersDialog(wxWindow* parent, IEditor* editor)
    : PHPSettersGettersDialogBase(parent)
    , m_editor(editor)
{
    PHPConfigurationData conf;
    size_t flags = conf.Load().GetSettersGettersFlags();
    m_checkBoxLowercase->SetValue(flags & kSG_StartWithLowercase);
    m_checkBoxPrefixGetter->SetValue(flags & kSG_Prefix);
    m_checkBoxReurnThis->SetValue(flags & kSG_ReturnThis);
    
    WindowAttrManager::Load(this, "PHPSettersGettersDialog");
    PHPEntityBase::List_t members = PHPCodeCompletion::Instance()->GetMembers(editor);
}

PHPSettersGettersDialog::~PHPSettersGettersDialog() { WindowAttrManager::Save(this, "PHPSettersGettersDialog"); }

void PHPSettersGettersDialog::DoPopulate(const PHPEntityBase::List_t& members)
{
    m_dvListCtrlFunctions->DeleteAllItems();
    PHPEntityBase::List_t::const_iterator iter = members.begin();
    for(; iter != members.end(); ++iter) {
        
    }
}
