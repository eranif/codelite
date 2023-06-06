#ifndef CLCOMBOBOX_HPP
#define CLCOMBOBOX_HPP

#include "wxCustomControls.hpp"

#if wxUSE_NATIVE_COMBOBOX

#include "clButton.h"
#include "codelite_exports.h"
#include "wx_ordered_map.h"

#include <functional>
#include <unordered_map>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/control.h>

class WXDLLIMPEXP_SDK clComboBox : public wxComboBox
{
public:
    clComboBox();
    clComboBox(wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, size_t n = 0,
               const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr);
    clComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
               const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr);
    bool Create(wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, size_t n = 0,
                const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);
    bool Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);
    virtual ~clComboBox();
};
#else
// generic version
#include "clComboBoxGeneric.hpp"
typedef clComboBoxGeneric clComboBox;
#endif
#endif // CLCOMBOBOX_HPP
