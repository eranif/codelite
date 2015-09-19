#include "my_config.h"
#include <wx/font.h>
#include <wx/settings.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>

MyConfig::MyConfig()
    : wxFileConfig("",
                   "",
                   (wxStandardPaths::Get().GetUserDataDir() + "/codelite-terminal.ini"),
                   "",
                   wxCONFIG_USE_LOCAL_FILE)
{
    wxFileName fn(wxStandardPaths::Get().GetUserDataDir(), "codelite-terminal.ini");
    fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
}

MyConfig::~MyConfig()
{
    Save();
}

wxPoint MyConfig::GetTerminalPosition() const
{
    wxPoint pt(100, 100);
    Read("frame_position_x", &pt.x);
    Read("frame_position_y", &pt.y);
    return pt;
}

wxSize MyConfig::GetTerminalSize() const
{
    wxSize size(600, 400);
    Read("frame_size_width", &size.x);
    Read("frame_size_height", &size.y);
    return size;
}

void MyConfig::SetTerminalPosition(const wxPoint& pt)
{
    Write("frame_position_x", pt.x);
    Write("frame_position_y", pt.y);
}

void MyConfig::SetTerminalSize(const wxSize& size)
{
    Write("frame_size_width", size.x);
    Write("frame_size_height", size.y);
}

void MyConfig::Save()
{
    Flush();
}

wxColour MyConfig::GetBgColour() const
{
    wxString col;
    if(Read("bg_colour", &col) && !col.IsEmpty()) {
        wxColour colour(col);
        return colour;
    }
    return *wxBLACK;
}

wxColour MyConfig::GetFgColour() const
{
    wxString col;
    if(Read("fg_colour", &col) && !col.IsEmpty()) {
        wxColour colour(col);
        return colour;
    }
    return *wxWHITE;
}

void MyConfig::SetBgColour(const wxColour& col)
{
    Write("bg_colour", col.GetAsString());
}

void MyConfig::SetFgColour(const wxColour& col)
{
    Write("fg_colour", col.GetAsString());
}

wxFont MyConfig::GetFont() const
{
    wxFont defaultFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    defaultFont.SetFamily(wxFONTFAMILY_TELETYPE);

    // read the attributes
    wxString facename;
    int pointSize;

    Read("font_facename", &facename, defaultFont.GetFaceName());
    Read("font_size", &pointSize, defaultFont.GetPointSize());

    wxFont f(wxFontInfo(pointSize).FaceName(facename).Family(wxFONTFAMILY_TELETYPE));
    return f;
}

void MyConfig::SetFont(const wxFont& font)
{
    Write("font_facename", font.GetFaceName());
    Write("font_size", font.GetPointSize());
}
