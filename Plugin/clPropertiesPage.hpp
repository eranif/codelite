#ifndef CLPROPERTIESPAGE_HPP
#define CLPROPERTIESPAGE_HPP

#include "clThemedListCtrl.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <unordered_map>
#include <vector>
#include <wx/any.h>
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/window.h>

using namespace std;
enum class LineKind {
    UNKNOWN,
    COLOUR,
    CHOICE,
    TEXT_EDIT,
    CHECKBOX,
    FILE_PICKER,
    DIR_PICKER,
};

struct WXDLLIMPEXP_SDK LineData {
    LineKind line_kind = LineKind::UNKNOWN;
    wxAny value;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_PROPERTIES_PAGE_MODIFIED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_PROPERTIES_PAGE_SAVED, clCommandEvent);

class WXDLLIMPEXP_SDK clPropertiesPage : public wxPanel
{
    clThemedListCtrl* m_view = nullptr;
    vector<size_t> m_header_rows;
    bool m_theme_event_connected = false;
    unordered_map<size_t, LineData> m_lines;
    bool m_isModified = false;

protected:
    void OnActionButton(wxDataViewEvent& e);
    void OnThemeChanged(clCommandEvent& event);
    void SetHeaderColours(const wxDataViewItem& item);
    void OnInitDone(wxCommandEvent& event);
    void OnChoice(wxDataViewEvent& event);

    // Helpers
    void ShowColourPicker(size_t line, const wxColour& colour);
    void ShowTextEditor(size_t line, const wxString& text);
    void ShowStringSelectionMenu(size_t line, const wxArrayString& options);
    void ShowFilePicker(size_t line, const wxString& path);
    void ShowDirPicker(size_t line, const wxString& path);
    void SetModified();

    bool GetLineData(size_t line, const LineData** data) const;
    template <typename T> void SetLineData(size_t line, LineKind kind, const T& data)
    {
        if(m_lines.count(line))
            m_lines.erase(line);
        LineData d;
        d.value = data;
        d.line_kind = kind;
        m_lines.insert({ line, d });
    }

    template <typename T> void SetLastLineData(LineKind kind, const T& data)
    {
        if(m_view->IsEmpty()) {
            return;
        }

        size_t line = m_view->GetItemCount() - 1;
        SetLineData(line, kind, data);
    }

public:
    clPropertiesPage(wxWindow* parent, wxWindowID id = wxID_ANY);
    ~clPropertiesPage();

    void AddProperty(const wxString& label, const wxArrayString& choices, size_t sel = 0);
    void AddProperty(const wxString& label, const vector<wxString>& choices, size_t sel = 0);
    void AddProperty(const wxString& label, bool checked);
    void AddProperty(const wxString& label, const wxString& value);
    void AddProperty(const wxString& label, const char* value) { AddProperty(label, wxString(value)); }
    void AddProperty(const wxString& label, const wxColour& value);
    void AddPropertyFilePicker(const wxString& label, const wxString& path);
    void AddPropertyDirPicker(const wxString& label, const wxString& path);
    void AddHeader(const wxString& label);

    bool IsModified() const { return m_isModified; }
    void ClearModified();
};

#endif // CLPROPERTIESPAGE_HPP
