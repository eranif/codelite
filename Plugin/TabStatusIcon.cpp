#include "TabStatusIcon.hpp"

#include "bitmap_loader.h"
#include "globals.h"

#include <utility>

namespace
{
// The names of the icons (svgs/<theme>/<name>.svg). The animations have kFrameCount frames: <name>-0 ... <name>-15
constexpr const char* kSpinnerName = "agent-spinner";
constexpr const char* kBlockedName = "agent-attention-blocked";
constexpr const char* kDoneName = "agent-attention-done";

wxBitmap LoadIcon(const wxString& name) { return clGetManager()->GetStdIcons()->LoadBitmap(name); }

std::vector<wxBitmap> LoadFrames(const char* name)
{
    std::vector<wxBitmap> frames;
    frames.reserve(TabStatusIcon::kFrameCount);
    for (int i = 0; i < TabStatusIcon::kFrameCount; ++i) {
        frames.push_back(LoadIcon(wxString::Format("%s-%d", name, i)));
    }
    return frames;
}
} // namespace

TabStatusIcon::TabStatusIcon(clTabIconBook book, wxWindow* page)
    : m_book(std::move(book))
    , m_page(page)
{
    if (m_page != nullptr) {
        m_page->Bind(wxEVT_DESTROY, &TabStatusIcon::OnPageDestroyed, this);
    }
}

TabStatusIcon::~TabStatusIcon()
{
    // Restore the icon of the tab, while the page is still there (at most one of the icons is shown)
    m_attention.reset();
    m_spinner.reset();
    if (m_page != nullptr) {
        m_page->Unbind(wxEVT_DESTROY, &TabStatusIcon::OnPageDestroyed, this);
    }
}

void TabStatusIcon::SetState(wxTerminalProgramState state)
{
    if (m_page == nullptr) {
        return;
    }

    // Only a change is handled (a working program reports its progress again and again)
    if (std::exchange(m_state, state) == state) {
        return;
    }

    switch (state) {
    case wxTerminalProgramState::Working:
        // An icon that asks for the user is out of date now. Remove it first, so the spinner finds the original icon
        m_attention.reset();
        ShowSpinner();
        break;

    case wxTerminalProgramState::Blocked:
    case wxTerminalProgramState::Done:
        // Remove the spinner first, so the new icon finds the original icon
        m_spinner.reset();
        ShowAttention(state);
        break;

    case wxTerminalProgramState::Idle:
    case wxTerminalProgramState::Error:
    case wxTerminalProgramState::Clear:
        // Nothing is going on. An icon that asks for the user stays until the user comes back to the page.
        m_spinner.reset();
        break;
    }
}

void TabStatusIcon::ShowSpinner()
{
    if (m_spinner) {
        return; // already spinning
    }
    m_spinner = std::make_unique<TabAttentionIcon>(
        m_book, m_page, LoadFrames(kSpinnerName), false, kFrameIntervalMs, m_baseIcon);
}

void TabStatusIcon::ShowAttention(wxTerminalProgramState state)
{
    // Replacing the icon first restores the original tab icon
    m_attention.reset();

    // The user is looking at this page, or the notifications are off
    if (!m_attentionEnabled || !m_book.IsOk() || m_book.isCurrentPage(m_page)) {
        return;
    }

    // A pulse: the program waits for the user. A check mark: the program is done.
    const std::vector<wxBitmap> frames = state == wxTerminalProgramState::Blocked
                                             ? LoadFrames(kBlockedName)
                                             : std::vector<wxBitmap>{LoadIcon(kDoneName)};
    m_attention = std::make_unique<TabAttentionIcon>(m_book, m_page, frames, true, kFrameIntervalMs, m_baseIcon);
}

void TabStatusIcon::OnPageDestroyed(wxWindowDestroyEvent& event)
{
    event.Skip();
    // wxEVT_DESTROY is a command event: it is sent to the parents of a destroyed window as well. Ignore the events of
    // the children of the page
    if (event.GetEventObject() != m_page) {
        return;
    }
    // The page (and its tab) is being destroyed: do not touch it, or the book, any more
    m_spinner.reset();
    m_attention.reset();
    m_page = nullptr;
}
