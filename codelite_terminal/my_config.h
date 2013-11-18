#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <wx/config.h>
#include <wx/gdicmn.h>
#include <wx/fileconf.h>
#include <wx/colour.h>

class MyConfig : public wxFileConfig
{
public:
    MyConfig();
    virtual ~MyConfig();
    
    wxPoint GetTerminalPosition() const;
    wxSize  GetTerminalSize() const;
    wxColour GetBgColour() const;
    wxColour GetFgColour() const;
    wxFont   GetFont() const;
    
    void SetTerminalPosition(const wxPoint& pt);
    void SetTerminalSize(const wxSize &size);
    void SetBgColour(const wxColour& col);
    void SetFgColour(const wxColour& col);
    void SetFont(const wxFont& font);
    void Save();
};

#endif // MYCONFIG_H
