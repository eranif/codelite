#ifndef CLENHANCEDTOOLBAR_HPP
#define CLENHANCEDTOOLBAR_HPP

#include "clToolBar.h"
#include "clToolBarButtonBase.h"

#include <codelite_exports.h>
#include <unordered_map>

using namespace std;
class WXDLLIMPEXP_SDK clEnhancedToolBar : public clToolBar
{
public:
    struct Button {
        wxWindowID id;
        wxString label;
        size_t bmp_id;
    };

protected:
    struct ButtonState {
        Button button1;
        Button button2;
        size_t selection = 0;
        wxEvtHandler* sink = nullptr;
        ButtonState() {}
        ButtonState(const Button& bt1, const Button& bt2, size_t sel, wxEvtHandler* handler)
            : button1(bt1)
            , button2(bt2)
            , selection(sel)
            , sink(handler)
        {
        }
        Button& GetSelection() { return selection == 0 ? button1 : button2; }
        void ChangeSelection() { selection == 0 ? selection = 1 : selection = 0; };
        wxEvtHandler* GetSink(clEnhancedToolBar* tb) { return sink == nullptr ? tb->GetEventHandler() : sink; }
    };

protected:
    unordered_map<wxWindowID, ButtonState> m_buttons;
    void OnButtonClicked(wxCommandEvent& event);

public:
    clEnhancedToolBar(wxWindow* parent, wxWindowID winid = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                      const wxString& name = "clEnhancedToolBar");
    virtual ~clEnhancedToolBar();

    /**
     * @brief add a button that changes its state per click. The initial state is 'button1'
     */
    void Add2StatesTool(wxWindowID id, wxEvtHandler* sink, const Button& button1, const Button& button2,
                        wxItemKind kind = wxITEM_NORMAL);

    /**
     * @brief Set the button to use settings defined by actionID
     */
    void SetButtonAction(wxWindowID buttonId, wxWindowID actionID);
};

#endif // CLENHANCEDTOOLBAR_HPP
