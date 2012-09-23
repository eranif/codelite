#include "outline_settings.h"
#include <wx/stdpaths.h>
#include "json_node.h"

OutlineSettings::OutlineSettings()
{
}

OutlineSettings::~OutlineSettings()
{
}

void OutlineSettings::Load()
{
/*    wxFileName fn(wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config"), wxT("outline.conf"));
    JSONRoot json(fn);
    if( json.isOk() ) {
        
    }*/
}

void OutlineSettings::Save()
{
/*    wxFileName fn(wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + wxT("config"), wxT("outline.conf"));
    JSONRoot json(cJSON_Object);
   
    json.save(fn);*/
}
