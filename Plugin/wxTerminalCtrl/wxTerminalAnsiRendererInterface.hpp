#ifndef WXTERMINALANSIRENDERERINTERFACE_HPP
#define WXTERMINALANSIRENDERERINTERFACE_HPP

class wxTerminalAnsiRendererInterface
{
private:
    wxTerminalAnsiRendererInterface(const wxTerminalAnsiRendererInterface&) = delete;
    wxTerminalAnsiRendererInterface& operator=(const wxTerminalAnsiRendererInterface&) = delete;
    wxTerminalAnsiRendererInterface(wxTerminalAnsiRendererInterface&&) = delete;
    wxTerminalAnsiRendererInterface& operator=(wxTerminalAnsiRendererInterface&&) = delete;

public:
    wxTerminalAnsiRendererInterface();
    virtual ~wxTerminalAnsiRendererInterface();

};

#endif // WXTERMINALANSIRENDERERINTERFACE_HPP
