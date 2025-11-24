#ifndef LSP_SYMBOLSTYLE_H
#define LSP_SYMBOLSTYLE_H

#include "LSP/basic_types.h"
#include "codelite_exports.h"
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/colour.h>
#include "clTerminalViewCtrl.hpp"
#include "../lexer_configuration.h"

namespace LSP
{
struct WXDLLIMPEXP_SDK LSPSymbolStyle {
    wxString icon = "";
    wxColour colour = *wxBLACK;
    bool bold = false;
    bool isFunction = false;    
    
    static const wxString ICON_FUNCTION;
    static const wxString ICON_CLASS;
    static const wxString ICON_VARIABLE;
    static const wxString ICON_MODULE;
    static const wxString ICON_ENUMERATOR;
    
    LSPSymbolStyle() = default;
    LSPSymbolStyle(eSymbolKind kind, LexerConf* lexer);
};

} // namespace LSP
#endif // LSP_SYMBOLSTYLE_H