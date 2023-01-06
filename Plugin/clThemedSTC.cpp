#include "clThemedSTC.hpp"

#include "ColoursAndFontsManager.h"

clThemedSTC::clThemedSTC(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                         const wxString& name)
    : wxStyledTextCtrl(parent, id, pos, size, style, name)
{
    m_editEventsHandler.Reset(new clEditEventsHandler(this));
    LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
    if(lex) {
        lex->ApplySystemColours(this);
    }
}

clThemedSTC::clThemedSTC(wxWindow* parent, wxWindowID id, const wxString& defaultValue, const wxPoint& pos,
                         const wxSize& size, long style, const wxString& name)
    : wxStyledTextCtrl(parent, id, pos, size, style, name)
{
    m_editEventsHandler.Reset(new clEditEventsHandler(this));
    LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
    if(lex) {
        lex->ApplySystemColours(this);
    }

    if(!defaultValue.empty()) {
        SetText(defaultValue);
    }
}

clThemedSTC::clThemedSTC() { m_editEventsHandler.Reset(nullptr); }

bool clThemedSTC::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
                         const wxString& name)
{
    bool res = wxStyledTextCtrl::Create(parent, id, pos, size, style, name);
    if(res) {
        LexerConf::Ptr_t lex = ColoursAndFontsManager::Get().GetLexer("text");
        if(lex) {
            lex->ApplySystemColours(this);
        }
    }
    return res;
}

clThemedSTC::~clThemedSTC() {}
