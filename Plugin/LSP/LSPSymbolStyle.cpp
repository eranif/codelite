#include "LSPSymbolStyle.h"

namespace LSP
{   
    
const wxString LSPSymbolStyle::ICON_FUNCTION = wxT("\u2A10"); // 10768     2A10     ⨐     CIRCULATION FUNCTION
const wxString LSPSymbolStyle::ICON_CLASS = wxT("\u2394"); // 9108     2394     ⎔     SOFTWARE-FUNCTION SYMBOL
const wxString LSPSymbolStyle::ICON_VARIABLE = wxT("\u2027"); // 8231     2027     ‧     HYPHENATION POINT
const wxString LSPSymbolStyle::ICON_MODULE = wxT("{}");
const wxString LSPSymbolStyle::ICON_ENUMERATOR = wxT("#");

LSPSymbolStyle::LSPSymbolStyle(eSymbolKind kind, LexerConf* lexer) 
{
    switch (kind) {
        case kSK_File:
        case kSK_Module:
        case kSK_Package:
        case kSK_Container:
            icon = ICON_MODULE;
            colour = lexer->GetProperty(wxSTC_P_STRING).GetFgColour();
            break;
        case kSK_Class:
        case kSK_Struct:
        case kSK_Interface:
        case kSK_Object:
        case kSK_Enum:
            icon = ICON_CLASS;
            colour = lexer->GetProperty(wxSTC_P_WORD2).GetFgColour();
            bold = true;
            break;
        case kSK_Method:
        case kSK_Function:
        case kSK_Constructor:            
            icon = ICON_FUNCTION;
            colour = lexer->GetProperty(wxSTC_P_DEFNAME).GetFgColour();
            isFunction = true;
            break;
        case kSK_TypeParameter: // define
        case kSK_EnumMember:        
            icon = ICON_ENUMERATOR;
            colour = lexer->GetProperty(wxSTC_P_OPERATOR).GetFgColour();
            break;
        default:     
            icon = ICON_VARIABLE;
            colour = lexer->GetProperty(wxSTC_P_IDENTIFIER).GetFgColour();
            break;
    }
}

} // namespace LSP