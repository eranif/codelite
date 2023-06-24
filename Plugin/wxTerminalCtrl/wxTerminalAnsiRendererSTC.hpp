#ifndef WXTERMINALANSIRENDERERSTC_HPP
#define WXTERMINALANSIRENDERERSTC_HPP

class wxTerminalAnsiRendererSTC
{
private:
    wxTerminalAnsiRendererSTC(const wxTerminalAnsiRendererSTC&) = delete;
    wxTerminalAnsiRendererSTC& operator=(const wxTerminalAnsiRendererSTC&) = delete;
    wxTerminalAnsiRendererSTC(wxTerminalAnsiRendererSTC&&) = delete;
    wxTerminalAnsiRendererSTC& operator=(wxTerminalAnsiRendererSTC&&) = delete;

public:
    wxTerminalAnsiRendererSTC();
    virtual ~wxTerminalAnsiRendererSTC();

};

#endif // WXTERMINALANSIRENDERERSTC_HPP
