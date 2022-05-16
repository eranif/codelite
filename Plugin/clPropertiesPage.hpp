#ifndef CLPROPERTIESPAGE_HPP
#define CLPROPERTIESPAGE_HPP

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
enum class PropertyType {
    INVALID,
    STRING,
    STRING_ARRAY,
    COLOUR,
    CHECKBOX,
};

class WXDLLIMPEXP_SDK clPropertiesPage : public wxPanel
{
    clThemedListCtrl* m_view = nullptr;
    unordered_map<size_t, unordered_map<size_t, PropertyType>> m_cellTypes;

protected:
    void OnActionButton(wxDataViewEvent& e);
    PropertyType GetCellType(size_t row, size_t col) const;
    void SetCellType(size_t row, size_t col, PropertyType type);

    void ShowColourPicker(const wxDataViewItem& item, size_t col);
    void ShowTextEditor(const wxDataViewItem& item, size_t col);
    void ShowStringSelectionMenu(const wxDataViewItem& item, size_t col);

public:
    clPropertiesPage(wxWindow* parent, wxWindowID id = wxID_ANY);
    ~clPropertiesPage();

    void AddProperty(const wxString& label, const wxArrayString& choices, size_t sel = 0);
    void AddProperty(const wxString& label, const vector<wxString>& choices, size_t sel = 0);
    void AddProperty(const wxString& label, bool checked);
    void AddProperty(const wxString& label, const wxString& value);
    void AddProperty(const wxString& label, const char* value) { AddProperty(label, wxString(value)); }
    void AddProperty(const wxString& label, const wxColour& value);
};

#endif // CLPROPERTIESPAGE_HPP
