#ifndef CLTERMINALVIEWCTRL_HPP
#define CLTERMINALVIEWCTRL_HPP

#include "clAsciiEscapeColourBuilder.hpp"
#include "clDataViewListCtrl.h"
#include "cl_command_event.h"
#include <codelite_exports.h>

class WXDLLIMPEXP_SDK clTerminalViewCtrl : public clDataViewListCtrl
{
    clControlWithItemsRowRenderer* m_renderer = nullptr;
    clAsciiEscapeColourBuilder m_builder;

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
    void AddLine(const wxString& text, wxUIntPtr data = 0);

    clAsciiEscapeColourBuilder& GetBuilder();
};

#endif // CLTERMINALVIEWCTRL_HPP
