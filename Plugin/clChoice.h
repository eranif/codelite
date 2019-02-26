#ifndef CLCHOICE_H
#define CLCHOICE_H

#include <wx/choice.h>
#include "clButtonBase.h" // Base class: clButtonBase
#include <wx/arrstr.h>
#include <vector>
#include <string>

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_CHOICE_MENU_SHOWING, wxNotifyEvent);

class WXDLLIMPEXP_SDK clChoice : public clButtonBase
{
    std::vector<wxString> m_choices;
    int m_selection = wxNOT_FOUND;
    bool m_popupShown = false;

protected:
    void OnClick(wxCommandEvent& event);

private:
    void DoShowMenu();
    void Render(wxDC& dc);

public:
    clChoice();
    clChoice(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices,
             long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxChoiceNameStr);
    bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& choices,
                long style = 0, const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxChoiceNameStr);
    virtual ~clChoice();

    /**
     * @brief Finds an item whose label matches the given string.
     */
    int FindString(const wxString& s, bool caseSensitive = false) const;

    /**
     * @brief return the number of strings in the control
     * @return
     */
    size_t GetCount() const { return m_choices.size(); }

    /**
     * @brief return the selected item index
     * @return
     */
    int GetSelection() const;

    /**
     * @brief Returns the label of the item with the given index.
     */
    wxString GetString(size_t index) const;

    /**
     * @brief Sets the selection to the given item n
     */
    void SetSelection(size_t index);

    /**
     * @brief Sets the label for the given item.
     */
    void SetString(size_t index, const wxString& str);

    /**
     * @brief Returns the label of the selected item or an empty string if no item is selected.
     */
    wxString GetStringSelection() const;

    /**
     * @brief Selects the item with the specified string in the control
     */
    void SetStringSelection(const wxString& str);
    
    /**
     * @brief append string
     */
    int Append(const wxString& str);
    
    /**
     * @brief append items
     */
    void Append(const wxArrayString& items);
    
    /**
     * @brief clear the current control strings
     */
    void Clear();
    
    /**
     * @brief replace the current items in the control
     */
    void Set(const wxArrayString& items);
};

#endif // CLCHOICE_H
