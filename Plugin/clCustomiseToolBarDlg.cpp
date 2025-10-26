#include "clCustomiseToolBarDlg.h"

#include "bitmap_loader.h"
#include "clToolBar.h"
#include "clToolBarButtonBase.h"
#include "editor_config.h"
#include "globals.h"
#include "optionsconfig.h"

namespace
{
/**
 * @brief fit the dataview columns width to match their content
 */
void clFitColumnWidth(wxDataViewCtrl& ctrl)
{
#ifndef __WXOSX__
    for (size_t i = 0; i < ctrl.GetColumnCount(); ++i) {
        ctrl.GetColumn(i)->SetWidth(wxCOL_WIDTH_AUTOSIZE);
    }
#endif
}
} // namespace

clCustomiseToolBarDlg::clCustomiseToolBarDlg(wxWindow* parent, clToolBarGeneric* tb)
    : clCustomiseToolBarBaseDlg(parent)
    , m_toolbar(tb)
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    const wxBitmap& bmp = clGetManager()->GetStdIcons()->LoadBitmap("cog", options->GetIconsSize());
    m_dvListCtrlItems->SetRowHeight(bmp.GetScaledHeight() + 8);

    for (const clToolBarButtonBase* button : m_toolbar->GetButtons()) {
        wxVector<wxVariant> cols;
        cols.push_back(wxVariant(!button->IsHidden()));
        if (button->IsSpacer()) {
            cols.push_back(::MakeIconText(_("Spacer"), button->GetBitmap()));
        } else {
            cols.push_back(
                ::MakeIconText(button->IsSeparator() ? _("Separator") : button->GetLabel(), button->GetBitmap()));
        }
        m_dvListCtrlItems->AppendItem(cols, (wxUIntPtr)button);
    }
    ::clFitColumnWidth(*m_dvListCtrlItems);
}

clCustomiseToolBarDlg::~clCustomiseToolBarDlg() {}

void clCustomiseToolBarDlg::OnOK(wxCommandEvent& event)
{
    event.Skip();
    for (int i = 0; i < m_dvListCtrlItems->GetItemCount(); ++i) {
        wxVariant val;
        m_dvListCtrlItems->GetValue(val, i, 0);
        clToolBarButtonBase* button =
            reinterpret_cast<clToolBarButtonBase*>(m_dvListCtrlItems->GetItemData(m_dvListCtrlItems->RowToItem(i)));
        button->Show(val.GetBool());
    }
}
