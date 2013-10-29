#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <wx/config.h>
#include <wx/gdicmn.h>
#include <wx/fileconf.h>

class MyConfig : public wxFileConfig
{
public:
    MyConfig();
    virtual ~MyConfig();
    
    wxPoint GetTerminalPosition() const;
    wxSize  GetTerminalSize() const;
    
    void SetTerminalPosition(const wxPoint& pt);
    void SetTerminalSize(const wxSize &size);
    
    void Save();
};

#endif // MYCONFIG_H
