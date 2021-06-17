#include "clAsciiEscapCodeHandler.hpp"
#include <wx/tokenzr.h>

clAsciiEscapeCodeHandler::clAsciiEscapeCodeHandler()
{
    // Text colours
    m_colours.insert({ 30, wxColour(1, 1, 1) });
    m_colours.insert({ 31, wxColour(222, 56, 43) });
    m_colours.insert({ 32, wxColour(57, 181, 74) });
    m_colours.insert({ 33, wxColour(255, 199, 6) });
    m_colours.insert({ 34, wxColour(0, 111, 184) });
    m_colours.insert({ 35, wxColour(118, 38, 113) });
    m_colours.insert({ 36, wxColour(44, 181, 233) });
    m_colours.insert({ 37, wxColour(204, 204, 204) });
    m_colours.insert({ 90, wxColour(128, 128, 128) });
    m_colours.insert({ 91, wxColour(255, 0, 0) });
    m_colours.insert({ 92, wxColour(0, 255, 0) });
    m_colours.insert({ 93, wxColour(255, 255, 0) });
    m_colours.insert({ 94, wxColour(0, 0, 255) });
    m_colours.insert({ 95, wxColour(255, 0, 255) });
    m_colours.insert({ 96, wxColour(0, 255, 255) });
    m_colours.insert({ 97, wxColour(255, 255, 255) });

    // Background colours
    m_colours.insert({ 40, wxColour(1, 1, 1) });
    m_colours.insert({ 41, wxColour(222, 56, 43) });
    m_colours.insert({ 42, wxColour(57, 181, 74) });
    m_colours.insert({ 43, wxColour(255, 199, 6) });
    m_colours.insert({ 44, wxColour(0, 111, 184) });
    m_colours.insert({ 45, wxColour(118, 38, 113) });
    m_colours.insert({ 46, wxColour(44, 181, 233) });
    m_colours.insert({ 47, wxColour(204, 204, 204) });
    m_colours.insert({ 100, wxColour(128, 128, 128) });
    m_colours.insert({ 101, wxColour(255, 0, 0) });
    m_colours.insert({ 102, wxColour(0, 255, 0) });
    m_colours.insert({ 103, wxColour(255, 255, 0) });
    m_colours.insert({ 104, wxColour(0, 0, 255) });
    m_colours.insert({ 105, wxColour(255, 0, 255) });
    m_colours.insert({ 106, wxColour(0, 255, 255) });
    m_colours.insert({ 107, wxColour(255, 255, 255) });
}

clAsciiEscapeCodeHandler::~clAsciiEscapeCodeHandler() {}

#define NEXT(EOL)                                      \
    if(chunk->is_style_reset() || !chunk->d.empty()) { \
        chunk->set_flag(kCompleted);                   \
        chunk->set_flag(kEndOfLine, EOL);              \
        if(chunk->is_eol()) {                          \
            m_lineNumber++;                            \
        }                                              \
        EnsureCurrent();                               \
        chunk = &m_chunks.back();                      \
        chunk->line_number = m_lineNumber;             \
    }

void clAsciiEscapeCodeHandler::Parse(const wxString& buffer)
{
    EnsureCurrent();
    auto chunk = &m_chunks.back();
    for(const wxChar& ch : buffer) {
        switch(m_state) {
        case eColourHandlerState::kNormal:
            switch(ch) {
            case 0x1B: // ESC
                NEXT(false);
                // remove the text flag
                chunk->set_flag(kTextChunk, false);
                m_state = eColourHandlerState::kInEscape;
                break;
            case '\r':
                // ignore CR
                break;
            case '\n':
                NEXT(true);
                break;
            default:
                chunk->d << ch;
                break;
            }
            break;
        case eColourHandlerState::kInEscape:
            switch(ch) {
            // CSI - Control Sequence Introducer
            case '[':
                m_state = eColourHandlerState::kInCsi;
                break;
            case ']':
                m_state = eColourHandlerState::kInOsc;
                break;
            default:
                // do nothing
                break;
            }
            break;
        case eColourHandlerState::kInOsc:
            // ESC ]
            if(ch == '\a') {
                // bell, leave the current state
                chunk->set_flag(kTitleChunk);
                NEXT(true);
                m_state = eColourHandlerState::kNormal;
            } else {
                chunk->d << ch;
            }
            break;
        case eColourHandlerState::kInCsi:
            // found ESC[
            switch(ch) {
            case 'K':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'J':
            case 'S':
            case 'T':
            case 'f':
            case 'i':
            case 'n':
            case 'u':
            case 's':
                // exit the CSI mode back to normal mode
                NEXT(false);
                m_state = eColourHandlerState::kNormal;
                break;
            case 'm':
                // update the style
                if(chunk->d.empty() || chunk->d == "0") {
                    chunk->set_flag(kResetStyle);
                }
                NEXT(false);
                m_state = eColourHandlerState::kNormal;
                break;
            default:
                chunk->d << ch;
                break;
            }
        }
    }

    // remove the last chunk if it is empty
    if(!m_chunks.empty() && m_chunks.back().is_empty() && m_lines.count(m_chunks.back().line_number)) {
        auto& v = m_lines[m_chunks.back().line_number];
        // remove the reference for this chunk from the map (it will be the last item on the line number vector)
        v.pop_back();

        // remove it from the vector as well
        m_chunks.pop_back();
    }
}

void clAsciiEscapeCodeHandler::Reset()
{
    m_chunks.clear();
    m_lines.clear();
    m_lineNumber = 0;
    m_state = eColourHandlerState::kNormal;
}

void clAsciiEscapeCodeHandler::EnsureCurrent()
{
    if(m_chunks.empty() || (m_chunks.back().flags & kCompleted)) {
        m_chunks.emplace_back();
        m_chunks.back().line_number = m_lineNumber;
        if(m_lines.count(m_lineNumber) == 0) {
            m_lines.insert({ m_lineNumber, {} });
        }
        // associate the newly added item index to the current line number
        m_lines[m_lineNumber].push_back(m_chunks.size() - 1);
    }
}

void clAsciiEscapeCodeHandler::Render(wxDC& dc, const clRenderDefaultStyle& defaultStyle, int line, const wxRect& rect)
{
    // find the line chunks
    if(m_lines.count(line) == 0) {
        return;
    }
    const auto& v = m_lines[line];

    int text_height = dc.GetTextExtent("Tp").GetHeight();

    wxRect tmpRect(0, 0, 20, text_height);
    tmpRect = tmpRect.CenterIn(rect, wxVERTICAL);

    int yy = tmpRect.y;
    int xx = 5;
    // dc.SetClippingRegion(rect);
    for(size_t i : v) {
        const auto& chunk = m_chunks[i];
        if(chunk.is_style_reset()) {
            // reset the style
            defaultStyle.ResetDC(dc);
        } else if(chunk.is_text()) {
            // draw the text
            wxSize text_size = dc.GetTextExtent(chunk.d);
            dc.DrawText(chunk.d, xx, yy);
            xx += text_size.GetWidth();

        } else if(chunk.is_window_title() || chunk.is_empty()) {
            // for now, we do nothing
        } else {
            UpdateStyle(chunk, dc, defaultStyle);
        }

        // if this chunk was EOL, reset the style here
        if(chunk.is_eol()) {
            defaultStyle.ResetDC(dc);
        }
    }
    // dc.DestroyClippingRegion();
}

void clAsciiEscapeCodeHandler::UpdateStyle(const Chunk& chunk, wxDC& dc, const clRenderDefaultStyle& defaultStyle)
{
    // see: https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters
    if(chunk.d == "0") {
        defaultStyle.ResetDC(dc);
    } else {
        wxArrayString attrs = ::wxStringTokenize(chunk.d, ";", wxTOKEN_RET_EMPTY);
        wxFont f = defaultStyle.font;
        for(const wxString& attr : attrs) {
            long number;
            if(!attr.ToCLong(&number)) {
                continue;
            }
            switch(number) {
            case 0:
                // reset attributes
                defaultStyle.ResetDC(dc);
                break;
            case 1:
                f.SetWeight(wxFONTWEIGHT_BOLD);
                break;
            case 2:
                f.SetWeight(wxFONTWEIGHT_LIGHT);
                break;
            case 3:
                f.SetStyle(wxFONTSTYLE_ITALIC);
                break;
            case 4:
                f.SetUnderlined(true);
                break;
            default:
                if((number >= 30 && number <= 37) || (number >= 90 && number <= 97)) {
                    // use colour table to set the text colour
                    wxColour c = GetColour(number);
                    if(c.IsOk()) {
                        dc.SetTextForeground(c);
                    }
                } else if((number >= 40 && number <= 47) || (number >= 100 && number <= 107)) {
                    wxColour c = GetColour(number);
                    if(c.IsOk()) {
                        dc.SetTextBackground(c);
                    }
                }
                break;
            }
        }
        dc.SetFont(f);
    }
}

const wxColour& clAsciiEscapeCodeHandler::GetColour(int num) const
{
    if(m_colours.count(num) == 0) {
        return wxNullColour;
    }
    return m_colours.find(num)->second;
}
