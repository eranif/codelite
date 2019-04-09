#include "wxTerminalOptions.h"
#include "fileutils.h"
#include <drawingutils.h>
#include <cl_standard_paths.h>
#include <wx/filename.h>

void wxTerminalOptions::SetCommandFromFile(const wxString& command)
{
    wxString content;
    if(FileUtils::ReadFileContent(command, content)) {
        content.Trim().Trim(false);
        SetCommand(content);
    }
}

wxTerminalOptions::wxTerminalOptions()
{
    m_font = DrawingUtils::GetDefaultFixedFont();
    m_textColour = wxColour("rgb(248, 248, 242)");
    m_bgColour = wxColour("rgb(41, 43, 55)");
}

wxTerminalOptions::~wxTerminalOptions() {}

void wxTerminalOptions::FromJSON(const JSONItem& json)
{
    m_font = json.namedObject("font").toFont(m_font);
    m_bgColour = json.namedObject("bgColour").toColour(m_bgColour);
    m_textColour = json.namedObject("textColour").toColour(m_textColour);
}

JSONItem wxTerminalOptions::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("font", m_font);
    json.addProperty("bgColour", m_bgColour);
    json.addProperty("textColour", m_textColour);
    return json;
}

wxTerminalOptions& wxTerminalOptions::Get()
{
    static wxTerminalOptions options;
    return options;
}

wxTerminalOptions& wxTerminalOptions::Load()
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "wxterminalctrl.conf");
    JSON root(fn);
    if(root.isOk()) { FromJSON(root.toElement()); }
    return *this;
}

wxTerminalOptions& wxTerminalOptions::Save()
{
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "wxterminalctrl.conf");
    JSON json(ToJSON());
    json.save(fn);
    return *this;
}
