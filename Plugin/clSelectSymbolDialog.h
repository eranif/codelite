#ifndef CLSELECTSYMBOLDIALOG_H
#define CLSELECTSYMBOLDIALOG_H

#include "clSelectSymbolDialogBase.h"
#include "codelite_exports.h"
#include <wx/sharedptr.h>
#include "cl_command_event.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SYMBOL_SELECTED, clCommandEvent);

class WXDLLIMPEXP_SDK clSelectSymbolDialog : public clSelectSymbolDialogBase
{
public:
    clSelectSymbolDialog(wxWindow* parent);
    virtual ~clSelectSymbolDialog();

    void AddSymbol(const wxString& name,
                   const wxBitmap& bmp = wxNullBitmap,
                   const wxString& help = "",
                   wxClientData* clientData = NULL);
    /**
     * @brief return the selected item client data
     */
    wxClientData* GetSelection() const;
    
protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // CLSELECTSYMBOLDIALOG_H
