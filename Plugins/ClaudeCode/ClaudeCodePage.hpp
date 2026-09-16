#ifndef CLAUDECODEPAGE_HPP
#define CLAUDECODEPAGE_HPP
#include "ClaudeCodeUI.hpp"

class wxTerminalViewCtrl;
class ClaudeCodePage : public ClaudeCodePageBase
{
public:
    ClaudeCodePage(wxWindow* parent, wxTerminalViewCtrl* terminal);
    ~ClaudeCodePage() override;

    inline wxTerminalViewCtrl* GetTerminal() { return m_terminal; }

private:
    wxTerminalViewCtrl* m_terminal{nullptr};
};
#endif // CLAUDECODEPAGE_HPP
