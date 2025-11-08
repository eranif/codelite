#include "resources/clXmlResource.hpp"

#include "Scripting/CodeLiteLUA.hpp"

#include <wx/app.h>
#include <wx/xrc/xmlres.h>

clXmlResource& clXmlResource::Get()
{
    static clXmlResource handler;
    return handler;
}

clXmlResource::clXmlResource() { wxTheApp->Bind(wxEVT_MENU_OPEN, &clXmlResource::OnMenuShown, this); }

clXmlResource::~clXmlResource() {}

wxMenuBar* clXmlResource::LoadMenuBar(const wxString& name)
{
    auto menubar = wxXmlResource::Get()->LoadMenuBar(name);
    return menubar;
}

wxMenu* clXmlResource::LoadMenu(const wxString& name)
{
    auto menu = wxXmlResource::Get()->LoadMenu(name);
    CodeLiteLUA::Get().UpdateMenu(name, menu);
    return menu;
}

void clXmlResource::Init() { wxXmlResource::Get()->InitAllHandlers(); }

void clXmlResource::OnMenuShown(wxMenuEvent& event)
{
    event.Skip();
    CodeLiteLUA::Get().UpdateMenu(wxEmptyString, event.GetMenu());
}
