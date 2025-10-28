#include "resources/clXmlResource.hpp"

#include "Scripting/CodeLiteLUA.hpp"

#include <wx/xrc/xmlres.h>

clXmlResource& clXmlResource::Get()
{
    static clXmlResource handler;
    return handler;
}

wxMenuBar* clXmlResource::LoadMenuBar(const wxString& name)
{
    auto menubar = wxXmlResource::Get()->LoadMenuBar(name);
    // note: pointer must be freed by the caller.
    return menubar;
}

wxMenu* clXmlResource::LoadMenu(const wxString& name)
{
    auto menu = wxXmlResource::Get()->LoadMenu(name);
    // note: pointer must be freed by the caller.
    CodeLiteLUA::Get().UpdateMenu(name, menu);
    return menu;
}

void clXmlResource::Init() { wxXmlResource::Get()->InitAllHandlers(); }
