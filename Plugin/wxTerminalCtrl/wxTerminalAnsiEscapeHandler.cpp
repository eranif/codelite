#include "wxTerminalAnsiEscapeHandler.hpp"

#include "clModuleLogger.hpp"
#include "clResult.hpp"
#include "file_logger.h"

INITIALISE_MODULE_LOG(LOG, "AnsiEscapeHandler", "ansi_escape_parser.log");

namespace
{
enum AnsiControlCode {
    BELL = 0x07, // Makes an audible noise.
    BS = 0x08,   //  Backspace  Moves the cursor left (but may "backwards wrap" if cursor is at start of line).
    HT = 0x09,   // Tab
    LF = 0x0A,   // Line Feed
    FF = 0x0C,   // Form Feed
    CR = 0x0D,   // Carriage Return
    ESC = 0x1B,  // Escape
};

enum AnsiSequenceType {
    NEED_MORE_DATA = -3,
    NOT_ESCAPE = -2,
    UNKNOWN_SEQUENCE_TYPE = -1,
    SS2 = 'N', // Single Shift Two
    SS3 = 'O', // Single Shift Three
    DCS = 'P', // Device Control String
    CSI = '[', // Control Sequence Introducer
    ST = '\\', // String Terminator
    OSC = ']', // Operating System Command
    SOS = 'X', // Start of String
    PM = '^',  // Privacy Message
    APC = '_', // Application Program Command
};

/// Common sequences + private sequences
enum AnsiControlSequence {
    UnknownControlSequence = -1,
    CursorUp = 'A', // Cursor Up, CSI n A. Moves the cursor n (default 1) cells in the given direction. If the cursor is
                    // already at the edge of the screen, this has no effect.
    CursorDown = 'B',    // Cursor down, CSI n B. Moves the cursor n (default 1) cells in the given direction. If the
                         // cursor is already at the edge of the screen, this has no effect.
    CursorForward = 'C', // Cursor Forward, CSI n C. Moves the cursor n (default 1) cells in the given direction. If the
                         // cursor is already at the edge of the screen, this has no effect.
    CursorBackward = 'D', // Cursor Backward, CSI n D. Moves the cursor n (default 1) cells in the given direction. If
                          // the cursor is already at the edge of the screen, this has no effect.
    CursorNextLine = 'E', // Cursor Next Line, CSI n E. Moves cursor to beginning of the line n (default 1) lines down
    CursorPreviousLine =
        'F', // Cursor Previous Line, CSI n F. Moves cursor to beginning of the line n (default 1) lines up
    CursorHorizontalAbsolute = 'G', // Cursor Horizontal Absolute, CSI n G. Moves the cursor to column n (default 1)
    CursorPosition = 'H', // Cursor Position, CSI n ; m H. Moves the cursor to row n, column m. The values are 1-based,
                          // and default to 1 (top left corner) if omitted. A sequence such as CSI ;5H is a synonym for
                          // CSI 1;5H as well as CSI 17;H is the same as CSI 17H and CSI 17;1H
    EraseInDisplay = 'J', // Erase in Display,  CSI n J. Clears part of the screen. If n is 0 (or missing), clear from
                          // cursor to end of screen. If n is 1, clear from cursor to beginning of the screen. If n is
                          // 2, clear entire screen (and moves cursor to upper left on DOS ANSI.SYS). If n is 3, clear
                          // entire screen and delete all lines saved in the scrollback buffer (this feature was added
                          // for xterm and is supported by other terminal applications).
    EraseInLine = 'K', // Erase in Line. CSI n K. Erases part of the line. If n is 0 (or missing), clear from cursor to
                       // the end of the line. If n is 1, clear from cursor to beginning of the line. If n is 2, clear
                       // entire line. Cursor position does not change.
    ScrollUp =
        'S', // Scroll Up. CSI n S. Scroll whole page up by n (default 1) lines. New lines are added at the bottom
    ScrollDown =
        'T', // Scroll Down. CSI n T. Scroll whole page down by n (default 1) lines. New lines are added at the top
    HorizontalVerticalPosition =
        'f', // Horizontal Vertical Position. CSI n ; m f. Same as CursorPosition, but counts as a format effector
             // function (like CR or LF) rather than an editor function (like CursorDown or CursorNextLine). This can
             // lead to different handling in certain terminal modes
    SelectGraphicRendition =
        'm',      // Select Graphic Rendition. CSI n m. Sets colors and style of the characters following this code
    AuxOn = 'i',  // CSI 5i. Enable aux serial port usually for local serial printer
    AuxOff = 'i', // CSI 4i. Disable aux serial port usually for local serial printer
    DeviceStatusReport = 'n', // CSI 6n. Device Status Report. Reports the cursor position (CPR) by transmitting
                              // ESC[n;mR, where n is the row and m is the column.

    // private sequences, we support them so we can ignore them
    SaveCurrentCursorPosition = 's',
    RestoreSavedCursorPosition = 'u',
    ActionEnable = 'h',  // Enable action, the action is determinted by the content between CSI <action> <h>
    ActionDisable = 'l', // Disable action, the action is determinted by the content between CSI <action> <l>
};

struct ColsRows {
    long n = 1;
    long m = 1;
};

struct AnsiControlSequenceValue {
    AnsiControlSequence seq = AnsiControlSequence::UnknownControlSequence;
    wxAny value;

    ColsRows value_cols_rows() const
    {
        ColsRows v;
        if(value.GetAs(&v)) {
            return v;
        }
        return {};
    }

    long value_long() const
    {
        long v;
        if(value.GetAs(&v)) {
            return v;
        }
        return 0;
    }

    long value_int() const
    {
        int v;
        if(value.GetAs(&v)) {
            return v;
        }
        return 0;
    }

    wxString value_string() const
    {
        wxString v;
        if(value.GetAs(&v)) {
            return v;
        }
        return wxEmptyString;
    }
};

/// read from the buffer until we find code
inline size_t find_control_code(wxStringView buffer, int code)
{
    size_t skip = 0;
    for(; skip < buffer.length(); ++skip) {
        if(buffer[skip] == code) {
            return skip;
        }
    }
    return wxStringView::npos;
}

/// safely get char at pos. return 0 if out-of-index
inline wxChar safe_get_char(wxStringView buffer, size_t pos)
{
    if(pos < buffer.length()) {
        return buffer[pos];
    }
    return 0;
}

/// read from the buffer until we find code
inline size_t find_st(wxStringView buffer, size_t* stlen)
{
    size_t pos = 0;
    for(; pos < buffer.length(); ++pos) {
        if(buffer[pos] == AnsiControlCode::ESC && safe_get_char(buffer, pos + 1) == AnsiSequenceType::ST) {
            *stlen = 2;
            return pos;
        } else if(buffer[pos] == AnsiControlCode::BELL) {
            *stlen = 1;
            return pos;
        }
    }
    return wxStringView::npos;
}

/// read from the buffer until we find the first code1 or code2 (whichever comes first)
inline size_t find_first_control_code(wxStringView buffer, int code1, int code2)
{
    size_t skip = 0;
    for(; skip < buffer.length(); ++skip) {
        if(buffer[skip] == code1 || buffer[skip] == code2) {
            break;
        }
    }
    return skip;
}

/// Starting from the start of the buffer, check the ANSI sequence type
inline AnsiSequenceType ansi_sequence_type(wxStringView buffer)
{
    wxChar ch0 = buffer[0];
    if(ch0 != AnsiControlCode::ESC) {
        return AnsiSequenceType::NOT_ESCAPE;
    }

    if(buffer.length() < 2) {
        return AnsiSequenceType::NEED_MORE_DATA;
    }

    wxChar ch1 = buffer[1];
    switch(ch1) {
    case AnsiSequenceType::SS2:
    case AnsiSequenceType::SS3:
    case AnsiSequenceType::DCS:
    case AnsiSequenceType::CSI:
    case AnsiSequenceType::ST:
    case AnsiSequenceType::OSC:
    case AnsiSequenceType::SOS:
    case AnsiSequenceType::PM:
    case AnsiSequenceType::APC:
        return (AnsiSequenceType)ch1;
    default:
        return AnsiSequenceType::UNKNOWN_SEQUENCE_TYPE;
    }
}

/// Parse wxStringView -> long
inline long wxStringViewAtol(wxStringView sv, long default_value)
{
    if(sv.empty()) {
        return default_value;
    }

    wxString str(sv.data(), sv.length());
    long v = default_value;
    if(str.ToCLong(&v)) {
        return v;
    }
    return default_value;
}

/// Parse "n;m" and return a `ColsRows` struct
inline ColsRows parse_cols_rows(wxStringView s, long default_value)
{
    ColsRows res{ default_value, default_value };
    auto where = s.find(';');
    if(where == wxStringView::npos) {
        res.n = wxStringViewAtol(s, default_value);
    } else {
        res.n = wxStringViewAtol(s.substr(0, where), default_value);
        res.m = wxStringViewAtol(s.substr(where + 1), default_value);
    }
    return res;
}

/// This function should be called after a successful call to
/// ansi_sequence_type() == CSI
/// An example for accepted sequence: 0x1B 0x9B 1 A
///                                     \   /  /| |
///                                      CSI  n | A => CursorUp (Cursor Up)
///                                             |
///                                          buffer
/// Loop until we find the terminator
inline wxHandlResultStringView ansi_control_sequence(wxStringView buffer, AnsiControlSequenceValue* value)
{
    for(size_t i = 0; i < buffer.length(); ++i) {
        switch(buffer[i]) {
        case AnsiControlSequence::SaveCurrentCursorPosition:
        case AnsiControlSequence::RestoreSavedCursorPosition: {
            // just skip it
            return wxHandlResultStringView::make_success(buffer.substr(i + 1));
        } break;
        case AnsiControlSequence::CursorUp:                 // default 1
        case AnsiControlSequence::CursorDown:               // default 1
        case AnsiControlSequence::CursorForward:            // default 1
        case AnsiControlSequence::CursorBackward:           // default 1
        case AnsiControlSequence::CursorNextLine:           // default 1
        case AnsiControlSequence::CursorPreviousLine:       // default 1
        case AnsiControlSequence::CursorHorizontalAbsolute: // default 1
        case AnsiControlSequence::ScrollUp:                 // default 1
        case AnsiControlSequence::ScrollDown:               // default 1
        //   AnsiControlSequence::AuxOff
        case AnsiControlSequence::AuxOn:              // NA
        case AnsiControlSequence::DeviceStatusReport: // NA
        {
            // CSI <number> <Terminator>
            auto sv = buffer.substr(0, i);
            long n = wxStringViewAtol(sv, 1 /* default value */);
            value->value = n;
            value->seq = (AnsiControlSequence)buffer[i];
            // return the remainder string
            return wxHandlResultStringView::make_success(buffer.substr(i + 1));
        } break;
        case AnsiControlSequence::EraseInDisplay: // default 0
        case AnsiControlSequence::EraseInLine:    // default 0
        {
            // CSI <number> <Terminator>
            auto sv = buffer.substr(0, i);
            long n = wxStringViewAtol(sv, 0 /* default value */);
            value->value = n;
            value->seq = (AnsiControlSequence)buffer[i];
            // return the remainder string
            return wxHandlResultStringView::make_success(buffer.substr(i + 1));
        } break;
        case AnsiControlSequence::CursorPosition:
        case AnsiControlSequence::HorizontalVerticalPosition: {
            // CSI<number>;<number><Terminator>
            auto nm = parse_cols_rows(wxStringView(buffer.data(), i), 1);
            value->value = nm;
            value->seq = (AnsiControlSequence)buffer[i];
            return wxHandlResultStringView::make_success(buffer.substr(i + 1));
        } break;
        case AnsiControlSequence::ActionEnable:
        case AnsiControlSequence::ActionDisable:
        case AnsiControlSequence::SelectGraphicRendition: {
            // Select Graphic Rendition. CSI n m. Sets colors and style of the characters
            // following this code
            value->value = wxString(buffer.data(), i);
            value->seq = (AnsiControlSequence)buffer[i];
            return wxHandlResultStringView::make_success(buffer.substr(i + 1));
        } break;
        }
    }
    return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
}

} // namespace

wxTerminalAnsiEscapeHandler::wxTerminalAnsiEscapeHandler() {}

wxTerminalAnsiEscapeHandler::~wxTerminalAnsiEscapeHandler() {}

size_t wxTerminalAnsiEscapeHandler::ProcessBuffer(wxStringView input, wxTerminalAnsiRendererInterface* renderer)
{
    LOG_IF_DEBUG
    {
        LOG_DEBUG(LOG) << "Processing buffer:" << endl;
        LOG_DEBUG(LOG) << wxString(input.data(), input.length()) << endl;
    }

    wxStringView sv = input;
    size_t consumed = 0;
    while(!sv.empty()) {
        switch(ansi_sequence_type(sv)) {
        case AnsiSequenceType::NEED_MORE_DATA:
            // found the ESC but not enough in the buffer to parse the rest
            // keep the ESC and request more data
            return input.length() - sv.length();

        case AnsiSequenceType::UNKNOWN_SEQUENCE_TYPE:
            // bug in the protocol, remove the 2 chars that we parsed
            sv.remove_prefix(2);
            break;

        case AnsiSequenceType::NOT_ESCAPE: {
            // no ansi escape found
            size_t len = find_control_code(sv, AnsiControlCode::ESC);
            if(len != wxStringView::npos) {
                wxStringView str = sv.substr(0, len);
                renderer->AddString(str);
                sv.remove_prefix(len);
            } else {
                // add the entire sv
                renderer->AddString(sv);
                sv = {};
            }
        } break;
        case AnsiSequenceType::CSI: {
            wxStringView buf = sv.substr(2);
            auto res = handle_csi(buf, renderer);
            if(!res) {
                switch(res.error()) {
                case wxHandleError::kProtocolError:
                    // recover by skipping these 2 chars
                    sv.remove_prefix(2);
                    break;
                case wxHandleError::kNeedMoreData:
                    return input.length() - sv.length();
                default:
                    break;
                }
            } else {
                // success, update the string view
                sv = res.success();
            }
        } break;
        case AnsiSequenceType::SS2:
        case AnsiSequenceType::SS3:
            // skip these sequences
            sv.remove_prefix(2);
            break;
        case AnsiSequenceType::DCS:
        case AnsiSequenceType::SOS:
        case AnsiSequenceType::PM:
        case AnsiSequenceType::APC: {
            // read until we find the ST, if we can't find it
            // return to the caller without consuming the bytes
            wxStringView buf{ sv };
            auto res = loop_until_st(buf);
            if(res) {
                sv = res.success();
            } else {
                return input.length() - sv.length();
            }
        } break;
        case AnsiSequenceType::OSC: {
            wxStringView buf = sv.substr(2);
            auto res = handle_osc(buf, renderer);
            if(!res) {
                switch(res.error()) {
                case wxHandleError::kProtocolError:
                    // recover
                    sv.remove_prefix(2);
                    break;
                case wxHandleError::kNeedMoreData:
                    return input.length() - sv.length();
                default:
                    break;
                }
            } else {
                // success, update the string view
                sv = res.success();
            }
        } break;
        case AnsiSequenceType::ST:
            // we shouldn't find this here, it should be handled as part of the loop_until_st() method called
            // from the other cases
            sv.remove_prefix(2);
            break;
        }
    }
    return input.length();
}

wxHandlResultStringView wxTerminalAnsiEscapeHandler::loop_until_st(wxStringView sv)
{
    size_t len = 0;
    size_t pos = find_st(sv, &len);
    if(pos == wxStringView::npos) {
        return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
    }
    return wxHandlResultStringView::make_success(sv.substr(pos + len));
}

wxHandlResultStringView wxTerminalAnsiEscapeHandler::handle_osc(wxStringView sv,
                                                                wxTerminalAnsiRendererInterface* renderer)
{
    if(sv.empty()) {
        return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
    }

    // ESC ]0;this is the window title <BEL>
    // ESC ]0;this is the window title <ST>
    // ESC ]8;;link <ST>
    wxChar ch = sv[0];
    sv.remove_prefix(1); // remove the `0` | `8`
    switch(ch) {
    case '0': {
        size_t pos = 0;
        // expecting ';'
        pos = find_control_code(sv, ';');
        if(pos == wxStringView::npos) {
            // incomplete buffer
            return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
        }

        sv.remove_prefix(pos + 1); // remove, including the ;
        size_t st_len;
        pos = find_st(sv, &st_len);
        if(pos == wxStringView::npos) {
            // incomplete buffer
            return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
        }

        wxStringView window_title = sv.substr(0, pos);
        renderer->SetWindowTitle(window_title);
        sv.remove_prefix(pos + st_len); // removing everything, including the terminator
        return wxHandlResultStringView::make_success(sv);
    } break;
    case '8': {
        size_t pos = find_control_code(sv, '\a' /* BEL */);
        if(pos == wxStringView::npos) {
            // incomplete buffer
            return wxHandlResultStringView::make_error(wxHandleError::kNeedMoreData);
        }
        sv.remove_prefix(pos + 1); // remove, including the BEL
        return wxHandlResultStringView::make_success(sv);
    } break;
    default:
        return wxHandlResultStringView::make_error(wxHandleError::kProtocolError);
    }
}

wxHandlResultStringView wxTerminalAnsiEscapeHandler::handle_csi(wxStringView sv,
                                                                wxTerminalAnsiRendererInterface* renderer)
{
    AnsiControlSequenceValue value;
    auto res = ansi_control_sequence(sv, &value);
    if(!res) {
        return res;
    }

    switch(value.seq) {
    case AnsiControlSequence::UnknownControlSequence:
    case AnsiControlSequence::ScrollUp:
    case AnsiControlSequence::ScrollDown:
    case AnsiControlSequence::AuxOn: // + AuxOff
    case AnsiControlSequence::DeviceStatusReport:
    default:
        break;
    case AnsiControlSequence::CursorBackward:
        // Move caret backward
        renderer->MoveCaret(value.value_long(), wxLEFT);
        break;
    case AnsiControlSequence::CursorPreviousLine:
    case AnsiControlSequence::CursorUp:
        // Move caret backward
        renderer->MoveCaret(value.value_long(), wxUP);
        break;
    case AnsiControlSequence::CursorNextLine:
    case AnsiControlSequence::CursorDown:
        // Move caret backward
        renderer->MoveCaret(value.value_long(), wxDOWN);
        break;
    case AnsiControlSequence::CursorForward:
        // Move caret backward
        renderer->MoveCaret(value.value_long(), wxRIGHT);
        break;
    case AnsiControlSequence::CursorHorizontalAbsolute:
        // Move caret backward
        renderer->SetCaretX(value.value_long());
        break;
    case AnsiControlSequence::EraseInLine:
        // Move caret backward
        switch(value.value_long()) {
        case 0:
            // for caret -> end of line
            renderer->ClearLine(wxRIGHT);
            break;
        case 1:
            // from 0 -> caret
            renderer->ClearLine(wxLEFT);
            break;
        case 2:
        default:
            // entire line
            renderer->ClearLine(wxLEFT | wxRIGHT);
            break;
        }
        break;
    case AnsiControlSequence::EraseInDisplay:
        // Move caret backward
        switch(value.value_long()) {
        case 0:
            // for caret -> end of screen
            renderer->ClearDisplay(wxDOWN);
            break;
        case 1:
            // from 0 -> caret
            renderer->ClearDisplay(wxUP);
            break;
        case 2:
        default:
            // entire display
            renderer->ClearDisplay(wxUP | wxDOWN);
            break;
        }
        break;
    case AnsiControlSequence::HorizontalVerticalPosition:
    case AnsiControlSequence::CursorPosition: {
        // Move caret backward
        auto pos = value.value_cols_rows();
        renderer->SetCaretX(pos.n);
        renderer->SetCaretY(pos.m);
    } break;
    case AnsiControlSequence::SelectGraphicRendition: {
        wxString s = value.value_string();
        wxStringView sv{ s };
        handle_sgr(sv, renderer);
    } break;
    }

    // return the SV byeond the CSI area
    return res;
}

void wxTerminalAnsiEscapeHandler::handle_sgr(wxStringView sv, wxTerminalAnsiRendererInterface* renderer)
{
    LOG_IF_DEBUG { LOG_DEBUG(LOG) << "SGR:" << wxString(sv.data(), sv.length()) << endl; }

    wxUnusedVar(sv);
    wxUnusedVar(renderer);
}
