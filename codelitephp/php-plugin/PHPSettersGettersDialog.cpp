#include "PHPSettersGettersDialog.h"
#include "windowattrmanager.h"
#include "php_code_completion.h"
#include "php_configuration_data.h"
#include "bitmap_loader.h"
#include "imanager.h"
#include "ieditor.h"
#include "globals.h"

struct PHPSettersGettersDialogClientData {
    PHPEntityBase::Ptr_t m_member;
    PHPSettersGettersDialogClientData(PHPEntityBase::Ptr_t member)
        : m_member(member)
    {
    }
    ~PHPSettersGettersDialogClientData() {}
};

PHPSettersGettersDialog::PHPSettersGettersDialog(wxWindow* parent, IEditor* editor, IManager* mgr)
    : PHPSettersGettersDialogBase(parent)
    , m_editor(editor)
    , m_mgr(mgr)
{
    PHPConfigurationData conf;
    size_t flags = conf.Load().GetSettersGettersFlags();
    m_checkBoxLowercase->SetValue(flags & kSG_StartWithLowercase);
    m_checkBoxPrefixGetter->SetValue(!(flags & kSG_NoPrefix));
    m_checkBoxReurnThis->SetValue(flags & kSG_ReturnThis);

    CenterOnParent();
    SetName("PHPSettersGettersDialog");
    WindowAttrManager::Load(this);
    PHPEntityBase::List_t members;
    PHPCodeCompletion::Instance()->GetMembers(editor, members, m_scope);
    DoPopulate(members);
}

PHPSettersGettersDialog::~PHPSettersGettersDialog()
{

    PHPConfigurationData conf;
    conf.Load().SetSettersGettersFlags(GetFlags()).Save();
    Clear();
}

void PHPSettersGettersDialog::DoPopulate(const PHPEntityBase::List_t& members)
{
    Clear();
    BitmapLoader* bmpLoader = m_mgr->GetStdIcons();
    wxBitmap memberBmp = bmpLoader->LoadBitmap("cc/16/member_public");
    m_dvListCtrlFunctions->DeleteAllItems();
    for (const auto& member : members) {
        wxVector<wxVariant> cols;
        cols.push_back(false);
        cols.push_back(::MakeIconText(member->GetDisplayName(), memberBmp));
        m_dvListCtrlFunctions->AppendItem(cols, (wxUIntPtr) new PHPSettersGettersDialogClientData(member));
    }
}

void PHPSettersGettersDialog::Clear()
{
    for(int i = 0; i < m_dvListCtrlFunctions->GetItemCount(); ++i) {
        PHPSettersGettersDialogClientData* cd = reinterpret_cast<PHPSettersGettersDialogClientData*>(
            m_dvListCtrlFunctions->GetItemData(m_dvListCtrlFunctions->RowToItem(i)));
        wxDELETE(cd);
    }
    m_dvListCtrlFunctions->DeleteAllItems();
}

PHPSetterGetterEntry::Vec_t PHPSettersGettersDialog::GetMembers()
{
    PHPSetterGetterEntry::Vec_t members;
    for(int i = 0; i < m_dvListCtrlFunctions->GetItemCount(); ++i) {
        PHPSettersGettersDialogClientData* cd = reinterpret_cast<PHPSettersGettersDialogClientData*>(
            m_dvListCtrlFunctions->GetItemData(m_dvListCtrlFunctions->RowToItem(i)));
        wxVariant val;
        m_dvListCtrlFunctions->GetValue(val, i, 0);
        if(val.GetBool()) {
            // This item is checked
            PHPSetterGetterEntry entry(cd->m_member);
            members.push_back(entry);
        }
    }
    return members;
}

size_t PHPSettersGettersDialog::GetFlags()
{
    size_t flags = 0;
    if(m_checkBoxLowercase->IsChecked()) flags |= kSG_StartWithLowercase;
    if(!m_checkBoxPrefixGetter->IsChecked()) flags |= kSG_NoPrefix;
    if(m_checkBoxReurnThis->IsChecked()) flags |= kSG_ReturnThis;
    return flags;
}
