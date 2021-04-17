#include "SFTPGrepStyler.h"

SFTPGrepStyler::SFTPGrepStyler(wxStyledTextCtrl* stc)
    : clFindResultsStyler(stc)
{
    m_curstate = kStartOfLine;
}

SFTPGrepStyler::SFTPGrepStyler() {}

SFTPGrepStyler::~SFTPGrepStyler() {}

void SFTPGrepStyler::StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasScope)
{
    int startPos = ctrl->GetEndStyled();
    int endPos = e.GetPosition();
    wxString text = ctrl->GetTextRange(startPos, endPos);
#if wxCHECK_VERSION(3, 1, 1) && !defined(__WXOSX__)
    // The scintilla syntax in wx3.1.1 changed
    ctrl->StartStyling(startPos);
#else
    ctrl->StartStyling(startPos, 0x1f);
#endif
    size_t filenameStyleLen = 0;
    size_t lineNumberStyleLen = 0;
    size_t matchStyleLen = 0;
    size_t headerStyleLen = 0;
    size_t i = 0;
    wxString::const_iterator iter = text.begin();
    for(; iter != text.end(); ++iter) {
        const wxUniChar& ch = *iter;
        size_t chWidth = 1;
        if(!ch.IsAscii()) {
            chWidth = wxString(ch).mb_str(wxConvUTF8).length();
        }

        switch(m_curstate) {
        default:
            break;
        case kStartOfLine:
            if(ch == '=') {
                ++headerStyleLen;
                m_curstate = kHeader;
            } else {
                filenameStyleLen += chWidth;
                m_curstate = kFile;
            }
            break;
        case kHeader:
            headerStyleLen += chWidth;
            if(ch == '\n') {
                m_curstate = kStartOfLine;
                ctrl->SetStyling(headerStyleLen, LEX_FIF_HEADER);
                headerStyleLen = 0;
            }
            break;
        case kFile:
            filenameStyleLen += chWidth;
            if(ch == ':') {
                m_curstate = kLineNumber;
                ctrl->SetStyling(filenameStyleLen, LEX_FIF_FILE);
                filenameStyleLen = 0;
            }
            break;
        case kLineNumber:
            ++lineNumberStyleLen;
            if(ch == ':') {
                m_curstate = kMatch;
                ctrl->SetStyling(lineNumberStyleLen, LEX_FIF_LINE_NUMBER);
                lineNumberStyleLen = 0;
            }
            break;
        case kMatch:
            matchStyleLen += chWidth;
            if(ch == '\n') {
                m_curstate = kStartOfLine;
                ctrl->SetStyling(matchStyleLen, LEX_FIF_MATCH);
                matchStyleLen = 0;
            }
            break;
        }

        i += chWidth;
    }

    // Left overs...
    if(filenameStyleLen) {
        ctrl->SetStyling(filenameStyleLen, LEX_FIF_FILE);
        filenameStyleLen = 0;
    }

    if(matchStyleLen) {
        ctrl->SetStyling(matchStyleLen, LEX_FIF_MATCH);
        matchStyleLen = 0;
    }

    if(lineNumberStyleLen) {
        ctrl->SetStyling(lineNumberStyleLen, LEX_FIF_LINE_NUMBER);
        lineNumberStyleLen = 0;
    }
    if(headerStyleLen) {
        ctrl->SetStyling(headerStyleLen, LEX_FIF_HEADER);
        headerStyleLen = 0;
    }
}

void SFTPGrepStyler::Reset() { m_curstate = kStartOfLine; }
