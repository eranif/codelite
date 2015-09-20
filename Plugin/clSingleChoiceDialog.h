#ifndef CLSINGLECHOICEDIALOG_H
#define CLSINGLECHOICEDIALOG_H
#include "wxcrafter_plugin.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clSingleChoiceDialog : public clSingleChoiceDialogBase
{
public:
    clSingleChoiceDialog(wxWindow* parent, const wxArrayString& options, int initialSelection = 0);
    virtual ~clSingleChoiceDialog();
    wxString GetSelection() const;
protected:
    virtual void OnItemActivated(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // CLSINGLECHOICEDIALOG_H
