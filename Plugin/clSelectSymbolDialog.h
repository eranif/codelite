#ifndef CLSELECTSYMBOLDIALOG_H
#define CLSELECTSYMBOLDIALOG_H

#include "clSelectSymbolDialogBase.h"
#include "codelite_exports.h"
#include <wx/sharedptr.h>
#include "cl_command_event.h"
#include <list>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_SYMBOL_SELECTED, clCommandEvent);

struct WXDLLIMPEXP_SDK clSelectSymbolDialogEntry {
    wxString name;
    wxBitmap bmp;
    wxString help;
    wxClientData* clientData;

    clSelectSymbolDialogEntry()
        : bmp(wxNullBitmap)
        , clientData(NULL)
    {
    }

    typedef std::list<clSelectSymbolDialogEntry> List_t;
};

class WXDLLIMPEXP_SDK clSelectSymbolDialog : public clSelectSymbolDialogBase
{
public:
    clSelectSymbolDialog(wxWindow* parent, const clSelectSymbolDialogEntry::List_t& entries);
    virtual ~clSelectSymbolDialog();

protected:
    void AddSymbol(const wxString& name,
                   const wxBitmap& bmp = wxNullBitmap,
                   const wxString& help = "",
                   wxClientData* clientData = NULL);

public:
    /**
     * @brief return the selected item client data
     */
    wxClientData* GetSelection() const;

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
};
#endif // CLSELECTSYMBOLDIALOG_H
