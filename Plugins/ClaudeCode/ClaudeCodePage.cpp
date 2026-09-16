#include "ClaudeCodePage.hpp"

#include "terminal_view.h"

#include <wx/sizer.h>

ClaudeCodePage::ClaudeCodePage(wxWindow* parent, wxTerminalViewCtrl* terminal)
    : ClaudeCodePageBase(parent)
    , m_terminal{terminal}
{
    GetSizer()->Add(m_terminal, wxSizerFlags(1).Expand());
    GetSizer()->Layout();
}

ClaudeCodePage::~ClaudeCodePage() {}
