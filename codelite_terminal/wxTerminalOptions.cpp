#include "wxTerminalOptions.h"

#include "fileutils.h"

#include <cl_standard_paths.h>
#include <drawingutils.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

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
    m_font.SetPointSize(14);
    m_textColour = wxColour("rgb(248, 248, 242)");
    m_bgColour = wxColour("rgb(41, 43, 55)");
}

wxTerminalOptions::~wxTerminalOptions() {}

void wxTerminalOptions::FromJSON(const JSONItem& json)
{
    m_font = json.namedObject("font").toFont(m_font);
    m_bgColour = json.namedObject("bgColour").toColour(m_bgColour);
    m_textColour = json.namedObject("textColour").toColour(m_textColour);
    m_history = json.namedObject("history").toArrayString();
}

JSONItem wxTerminalOptions::ToJSON() const
{
    JSONItem json = JSONItem::createObject();
    json.addProperty("font", m_font);
    json.addProperty("bgColour", m_bgColour);
    json.addProperty("textColour", m_textColour);
    json.addProperty("history", m_history);
    return json;
}

wxTerminalOptions& wxTerminalOptions::Get()
{
    static wxTerminalOptions options;
    return options;
}

wxTerminalOptions& wxTerminalOptions::Load()
{
    wxFileName fn(wxStandardPaths::Get().GetUserDataDir(), "wxterminalctrl.conf");
    JSON root(fn);
    if(root.isOk()) {
        FromJSON(root.toElement());
    }
    return *this;
}

wxTerminalOptions& wxTerminalOptions::Save()
{
    wxFileName fn(wxStandardPaths::Get().GetUserDataDir(), "wxterminalctrl.conf");
    JSON json(ToJSON());
    json.save(fn);
    return *this;
}

void wxTerminalOptions::SetHistory(const wxArrayString& history)
{
    const std::size_t threshold = 500;
    if(history.size() > threshold + 1) {
        m_history = wxArrayString();
        m_history.Alloc(threshold);
        for(std::size_t i = 0; i < threshold; ++i) {
            m_history.Add(history[i]);
        }
    } else {
        m_history = history;
    }
}
