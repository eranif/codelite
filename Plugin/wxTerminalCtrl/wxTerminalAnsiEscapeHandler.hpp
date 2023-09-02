#ifndef WXTERMINALANSIESCAPEHANDLER_HPP
#define WXTERMINALANSIESCAPEHANDLER_HPP

#include "clResult.hpp"
#include "codelite_exports.h"
#include "wxTerminalAnsiRendererInterface.hpp"

#include <wx/colour.h>
#include <wx/string.h>
#include <wx/textctrl.h>

enum class wxHandleError {
    kNeedMoreData = 1,
    kProtocolError = 2,
    kNotFound = 3,
};

typedef clResult<wxStringView, wxHandleError> wxHandlResultStringView;

class WXDLLIMPEXP_SDK wxTerminalAnsiEscapeHandler
{
private:
    wxHandlResultStringView handle_csi(wxStringView sv, wxTerminalAnsiRendererInterface* renderer);
    wxHandlResultStringView handle_osc(wxStringView sv, wxTerminalAnsiRendererInterface* renderer);
    // handle colour and style
    void handle_sgr(wxStringView sv, wxTerminalAnsiRendererInterface* renderer);
    wxHandlResultStringView loop_until_st(wxStringView sv);

public:
    wxTerminalAnsiEscapeHandler();
    virtual ~wxTerminalAnsiEscapeHandler();

    /**
     * @brief process buffer, return the number of chars consumed
     */
    size_t ProcessBuffer(wxStringView input, wxTerminalAnsiRendererInterface* renderer);

    /**
     * @brief return the colour by its ANSI code
     */
    static const wxColour& GetColour(int colour_number);
};

#endif // WXTERMINALANSIESCAPEHANDLER_HPP
