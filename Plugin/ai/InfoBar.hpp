#pragma once
#include "codelite_exports.h"

#include <vector>
#include <wx/infobar.h>

class WXDLLIMPEXP_SDK InfoBar : public wxInfoBar
{
public:
    InfoBar(wxWindow* parent, wxWindowID winid = wxID_ANY, long style = 0)
        : wxInfoBar(parent, winid, style)
    {
    }
    ~InfoBar() override = default;

    /**
     * @brief Displays a message in the info bar, queuing it if the bar is currently shown.
     *
     * If the info bar is already visible, the message is added to a pending queue.
     * Otherwise, the message is immediately displayed using the base class implementation.
     *
     * @param msg The message text to display in the info bar.
     * @param flags Display flags controlling the message appearance and behavior (e.g., icon type, buttons).
     *
     * @return void This function does not return a value.
     */
    void ShowMessage(const wxString& msg, int flags) override;

    /**
     * @brief Dismisses the current message and shows the next pending message if available.
     *
     * This method overrides the base class Dismiss() behavior. If there are pending messages
     * in the queue, it removes and displays the next message instead of hiding the info bar.
     * If no pending messages exist, it delegates to the base class to hide the info bar.
     *
     * @return void This method does not return a value.
     */
    void Dismiss() override;

private:
    std::vector<std::pair<wxString, int>> m_pendingMessages;
};
