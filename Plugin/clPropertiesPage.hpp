#ifndef CLPROPERTIESPAGE_HPP
#define CLPROPERTIESPAGE_HPP

#include "clThemedListCtrl.h"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <wx/any.h>
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/window.h>

enum class LineKind {
    UNKNOWN,
    COLOUR,
    CHOICE,
    TEXT_EDIT,
    CHECKBOX,
    FILE_PICKER,
    DIR_PICKER,
    INTEGER,
    BUTTON,
    LANGUAGE_PICKER,
};

struct WXDLLIMPEXP_SDK LineData {
    LineKind line_kind = LineKind::UNKNOWN;
    wxAny value;
    std::function<void(const wxString&, const wxAny&)> callback = nullptr;
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_PROPERTIES_PAGE_MODIFIED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_PROPERTIES_PAGE_SAVED, clCommandEvent);

class WXDLLIMPEXP_SDK clPropertiesPage : public wxPanel
{
    clThemedListCtrl* m_view = nullptr;
    std::vector<size_t> m_header_rows;
    bool m_theme_event_connected = false;
    std::unordered_map<size_t, LineData> m_lines;
    bool m_isModified = false;

public:
    typedef std::function<void(const wxString&, const wxAny&)> Callback_t;

protected:
    void OnActionButton(wxDataViewEvent& e);
    void OnThemeChanged(clCommandEvent& event);
    void SetHeaderColours(const wxDataViewItem& item);
    void OnInitDone(wxCommandEvent& event);
    void OnChoice(wxDataViewEvent& event);
    void OnValueChanged(wxDataViewEvent& event);

    // Helpers
    void ShowColourPicker(size_t line, const wxColour& colour);
    void ShowLanguagePicker(size_t line, const wxString& langs);
    void ShowTextEditor(size_t line, const wxString& text);
    void ShowStringSelectionMenu(size_t line, const wxArrayString& options);
    void ShowFilePicker(size_t line, const wxString& path);
    void ShowDirPicker(size_t line, const wxString& path);
    void ShowNumberPicker(size_t line, long number);
    void DoButtonClicked(size_t line);
    void SetModified();

    bool GetLineData(size_t line, const LineData** data) const;

    template <typename T>
    void UpdateLineData(size_t line, LineKind kind, const T& data, clPropertiesPage::Callback_t update_cb)
    {
        if(m_lines.count(line) == 0) {
            m_lines.insert({ line, {} });
        }

        // update the values
        LineData& d = m_lines[line];
        d.value = data;
        d.line_kind = kind;
        if(update_cb != nullptr) {
            d.callback = std::move(update_cb);
        }
    }

    template <typename T> void UpdateLastLineData(LineKind kind, const T& data, clPropertiesPage::Callback_t update_cb)
    {
        if(m_view->IsEmpty()) {
            return;
        }

        size_t line = m_view->GetItemCount() - 1;
        UpdateLineData(line, kind, data, update_cb);
    }

    void NotifyChange(size_t line);

public:
    clPropertiesPage(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clPropertiesPage();

    void AddProperty(const wxString& label, const wxArrayString& choices, size_t sel,
                     clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, const std::vector<wxString>& choices, size_t sel,
                     clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, const std::vector<wxString>& choices, const wxString& selection,
                     clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, const wxArrayString& choices, const wxString& selection,
                     clPropertiesPage::Callback_t update_cb);

    void AddProperty(const wxString& label, bool checked, clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, const wxString& value, clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, const char* value, clPropertiesPage::Callback_t update_cb)
    {
        AddProperty(label, wxString(value), std::move(update_cb));
    }
    void AddProperty(const wxString& label, const wxColour& value, clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, long value, clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, int value, clPropertiesPage::Callback_t update_cb)
    {
        AddProperty(label, (long)value, std::move(update_cb));
    }

    void AddPropertyFilePicker(const wxString& label, const wxString& path, clPropertiesPage::Callback_t update_cb);
    void AddPropertyDirPicker(const wxString& label, const wxString& path, clPropertiesPage::Callback_t update_cb);
    void AddPropertyLanguagePicker(const wxString& label, const wxArrayString& langs,
                                   clPropertiesPage::Callback_t update_cb);
    void AddPropertyButton(const wxString& label, const wxString& button_label, clPropertiesPage::Callback_t update_cb);
    void AddHeader(const wxString& label);

    bool IsModified() const { return m_isModified; }
    void ClearModified();

    /**
     * @brief call this method to tell the control what we are done adding items
     */
    void Finalize();
    /**
     * @brief tell the control that we are about to add multiple items, so avoid un-needed drawings
     * once done, call `Finalize`
     */
    void Begin();

    /**
     * @brief clear the view and discard all caches
     */
    virtual void Clear();
};

#endif // CLPROPERTIESPAGE_HPP
