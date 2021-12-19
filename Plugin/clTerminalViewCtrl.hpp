#ifndef CLTERMINALVIEWCTRL_HPP
#define CLTERMINALVIEWCTRL_HPP

#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clDataViewListCtrl.h"
#include "cl_command_event.h"

#include <codelite_exports.h>
#include <wx/font.h>

class WXDLLIMPEXP_SDK clTerminalViewCtrl : public clDataViewListCtrl
{
    clControlWithItemsRowRenderer* m_renderer = nullptr;
    clAnsiEscapeCodeColourBuilder m_builder;
    bool m_overwriteLastLine = false;
    bool m_scroll_to_bottom = true;
    wxFont m_rendererFont;

protected:
    void OnSysColourChanged(clCommandEvent& e);
    void ApplyStyle();

public:
    clTerminalViewCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize, long style = wxDV_MULTIPLE | wxDV_NO_HEADER);
    ~clTerminalViewCtrl();
    /**
     * @brief Add line of text, with optionally user data
     */
    void AddLine(const wxString& text, bool text_ends_with_cr, wxUIntPtr data = 0);

    clAnsiEscapeCodeColourBuilder& GetBuilder(bool clear_it = false);

    void SetScrollToBottom(bool b) { this->m_scroll_to_bottom = b; }

    /**
     * @brief set an alternative font for drawing
     */
    void SetDefaultFont(const wxFont& font) override;
};

#endif // CLTERMINALVIEWCTRL_HPP
