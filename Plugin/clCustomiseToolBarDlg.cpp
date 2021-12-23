#include "clCustomiseToolBarDlg.h"

#include "bitmap_loader.h"
#include "clToolBar.h"
#include "clToolBarButtonBase.h"
#include "editor_config.h"
#include "globals.h"
#include "optionsconfig.h"

clCustomiseToolBarDlg::clCustomiseToolBarDlg(wxWindow* parent, clToolBar* tb)
    : clCustomiseToolBarBaseDlg(parent)
    , m_toolbar(tb)
    , m_buttons(m_toolbar->GetButtons())
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    const wxBitmap& bmp = clGetManager()->GetStdIcons()->LoadBitmap("cog", options->GetIconsSize());
    m_dvListCtrlItems->SetRowHeight(bmp.GetScaledHeight() + 8);

    for(size_t i = 0; i < m_buttons.size(); ++i) {
        clToolBarButtonBase* button = m_buttons[i];
        wxVector<wxVariant> cols;
        cols.push_back(wxVariant(!button->IsHidden()));
        if(button->IsSpacer()) {
            cols.push_back(::MakeIconText(_("Spacer"), button->GetBitmap()));
        } else {
            cols.push_back(
                ::MakeIconText(button->IsSeparator() ? _("Separator") : button->GetLabel(), button->GetBitmap()));
        }
        m_dvListCtrlItems->AppendItem(cols, (wxUIntPtr)button);
    }
    ::clFitColumnWidth(m_dvListCtrlItems);
}

clCustomiseToolBarDlg::~clCustomiseToolBarDlg() {}

void clCustomiseToolBarDlg::OnOK(wxCommandEvent& event)
{
    event.Skip();
    for(int i = 0; i < m_dvListCtrlItems->GetItemCount(); ++i) {
        wxVariant val;
        m_dvListCtrlItems->GetValue(val, i, 0);
        clToolBarButtonBase* button =
            reinterpret_cast<clToolBarButtonBase*>(m_dvListCtrlItems->GetItemData(m_dvListCtrlItems->RowToItem(i)));
        button->Show(val.GetBool());
    }
}
