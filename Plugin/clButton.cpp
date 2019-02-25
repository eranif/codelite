#include "clButton.h"

clButton::clButton() {}

bool clButton::Create(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size,
                      long style, const wxValidator& validator, const wxString& name)
{
    return clButtonBase::Create(parent, id, label, pos, size, style, validator, name);
}

clButton::clButton(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size,
                   long style, const wxValidator& validator, const wxString& name)
    : clButtonBase(parent, id, label, pos, size, style, validator, name)
{
}

clButton::~clButton()
{
}
