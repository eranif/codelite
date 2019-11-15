#include "ColoursAndFontsManager.h"
#include "clThemedSTC.hpp"

clThemedSTC::clThemedSTC(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                         const wxString& name)
    : wxStyledTextCtrl(parent, id, pos, size, style, name)
{
    LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
    if(lex) { lex->Apply(this); }
}

clThemedSTC::clThemedSTC() {}

bool clThemedSTC::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                         const wxString& name)
{
    bool res = wxStyledTextCtrl::Create(parent, id, pos, size, style, name);
    if(res) {
        LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
        if(lex) { lex->Apply(this); }
    }
    return res;
}

clThemedSTC::~clThemedSTC() {}
