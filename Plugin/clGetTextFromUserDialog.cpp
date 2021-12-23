#include "clGetTextFromUserDialog.h"

#include "globals.h"
#include "windowattrmanager.h"

clGetTextFromUserDialog::clGetTextFromUserDialog(wxWindow* parent, const wxString& title, const wxString& message,
                                                 const wxString& initialValue, int charsToSelect)
    : clGetTextFromUserBaseDialog(parent)
{
    SetTitle(title);
    m_staticTextCaption->SetLabel(message);
    m_textCtrl->ChangeValue(initialValue);
    if(charsToSelect != wxNOT_FOUND && charsToSelect < (int)m_textCtrl->GetValue().Length()) {
        m_textCtrl->SetSelection(0, charsToSelect);
    } else {
        m_textCtrl->SelectAll();
    }
    // WindowAttrManager::Load(this);
    CenterOnParent();
    SetName("clGetTextFromUserDialog");
    GetSizer()->Fit(this);
}

clGetTextFromUserDialog::~clGetTextFromUserDialog() {}
