#ifndef CLPROPERTIESPAGE_HPP
#define CLPROPERTIESPAGE_HPP

#include "cl_command_event.h"
#include "codelite_exports.h"

#include <unordered_map>
#include <vector>
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/window.h>

class clThemedListCtrl;
using namespace std;
class WXDLLIMPEXP_SDK clPropertiesPage : public wxPanel
{
    clThemedListCtrl* m_view = nullptr;
    vector<size_t> m_header_rows;
    bool m_events_connected = false;

protected:
    void OnActionButton(wxDataViewEvent& e);
    void ShowColourPicker(const wxDataViewItem& item, size_t col);
    void ShowTextEditor(const wxDataViewItem& item, size_t col);
    void ShowStringSelectionMenu(const wxDataViewItem& item, size_t col);
    void OnThemeChanged(clCommandEvent& event);
    void SetHeaderColours(const wxDataViewItem& item);
    void OnInitDone(wxCommandEvent& event);

public:
    clPropertiesPage(wxWindow* parent, wxWindowID id = wxID_ANY);
    ~clPropertiesPage();

    void AddProperty(const wxString& label, const wxArrayString& choices, size_t sel = 0);
    void AddProperty(const wxString& label, const vector<wxString>& choices, size_t sel = 0);
    void AddProperty(const wxString& label, bool checked);
    void AddProperty(const wxString& label, const wxString& value);
    void AddProperty(const wxString& label, const char* value) { AddProperty(label, wxString(value)); }
    void AddProperty(const wxString& label, const wxColour& value);
    void AddHeader(const wxString& label);
};

#endif // CLPROPERTIESPAGE_HPP
