#include "clChoice.h"

#include "wxCustomControls.hpp"

#if wxUSE_NATIVE_CHOICE
clChoice::clChoice() {}

clChoice::~clChoice() {}

bool clChoice::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                      const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
{
    return wxChoice::Create(parent, id, pos, size, choices, style, validator, name);
}

clChoice::clChoice(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                   const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
    : wxChoice(parent, id, pos, size, choices, style, validator, name)
{
}

void clChoice::SetText(const wxString& text) { SetStringSelection(text); }

#else

#include <unordered_map>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

wxDEFINE_EVENT(wxEVT_CHOICE_MENU_SHOWING, wxNotifyEvent);

clChoice::clChoice() {}

clChoice::~clChoice() { Unbind(wxEVT_BUTTON, &clChoice::OnClick, this); }

bool clChoice::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                      const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
{
    wxUnusedVar(style);
    m_choices.insert(m_choices.end(), choices.begin(), choices.end());
    wxString initialValue;
    if(!choices.IsEmpty()) {
        m_selection = 0;
        initialValue = m_choices[0];
    }

    if(!clButtonBase::Create(parent, id, initialValue, pos, size, 0, validator, name)) {
        return false;
    }
    SetHasDropDownMenu(true);
    Bind(wxEVT_BUTTON, &clChoice::OnClick, this);
    return true;
}

clChoice::clChoice(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                   const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
{
    wxUnusedVar(style);
    Create(parent, id, pos, size, choices, 0, validator, name);
}

int clChoice::FindString(const wxString& s, bool caseSensitive) const
{
    for(size_t i = 0; i < m_choices.size(); ++i) {
        if(caseSensitive) {
            if(m_choices[i] == s) {
                return i;
            }
        } else {
            if(s.CmpNoCase(m_choices[i]) == 0) {
                return i;
            }
        }
    }
    return wxNOT_FOUND;
}

int clChoice::GetSelection() const { return m_selection; }

wxString clChoice::GetString(size_t index) const
{
    if(index >= m_choices.size()) {
        return "";
    }
    return m_choices[index];
}

void clChoice::SetSelection(size_t index)
{
    if(index >= m_choices.size()) {
        return;
    }
    m_selection = index;
    SetText(m_choices[m_selection]);
}

void clChoice::SetString(size_t index, const wxString& str)
{
    if(index >= m_choices.size()) {
        return;
    }
    m_choices[index] = str;
    // if we are updating the selected item, refresh the view
    if(index == (size_t)m_selection) {
        SetText(m_choices[m_selection]);
    }
}

wxString clChoice::GetStringSelection() const { return GetString((size_t)m_selection); }

void clChoice::SetStringSelection(const wxString& str)
{
    int where = FindString(str, true);
    if(where != wxNOT_FOUND) {
        SetSelection((size_t)where);
    }
}

void clChoice::OnClick(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoShowMenu();
}

void clChoice::DoShowMenu()
{
    // Check if the user allows us to disable the action
    wxNotifyEvent eventShowing(wxEVT_CHOICE_MENU_SHOWING);
    eventShowing.SetEventObject(this);
    GetEventHandler()->ProcessEvent(eventShowing);
    if(!eventShowing.IsAllowed()) {
        return;
    }

    wxMenu menu;
    int selectedIndex = wxNOT_FOUND;
    std::unordered_map<int, int> idToIndex;
    for(size_t i = 0; i < m_choices.size(); ++i) {
        int menuId = wxXmlResource::GetXRCID(m_choices[i]);
        const wxString& label = m_choices[i];
        wxMenuItem* item = menu.Append(menuId, label, label, wxITEM_CHECK);
        item->Check((int)i == m_selection);
        idToIndex.insert({ menuId, i });
        menu.Bind(
            wxEVT_MENU,
            [&](wxCommandEvent& e) {
                if(idToIndex.count(e.GetId())) {
                    selectedIndex = idToIndex[e.GetId()];
                }
            },
            menuId);
    }

    // Show the menu
    ShowMenu(menu);

    // Update the button label
    if(selectedIndex != wxNOT_FOUND) {
        SetSelection(selectedIndex);
        // Fire event
        wxCommandEvent evt(wxEVT_CHOICE);
        evt.SetEventObject(this);
        evt.SetInt(GetSelection());
        GetEventHandler()->AddPendingEvent(evt);
    }
}

void clChoice::Render(wxDC& dc)
{
#if !wxUSE_NATIVE_CHOICE
#if !wxUSE_NATIVE_BUTTON
    if(m_popupShown) {
        SetPressed();
    }
    clButtonBase::Render(dc);
#endif
#endif
}

int clChoice::Append(const wxString& str)
{
    m_choices.push_back(str);
    return (m_choices.size() - 1);
}

void clChoice::Append(const wxArrayString& items)
{
    m_choices.reserve(m_choices.size() + items.size());
    m_choices.insert(m_choices.end(), items.begin(), items.end());
}

void clChoice::Clear()
{
    m_choices.clear();
    SetText("");
    Refresh();
}

void clChoice::Set(const wxArrayString& items)
{
    m_choices.clear();
    Append(items);
    SetText("");
    Refresh();
}
#endif
