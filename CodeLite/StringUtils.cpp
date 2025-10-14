#include "StringUtils.h"

#include "macros.h"

#include <vector>
#include <wx/stc/stc.h>
#include <wx/tokenzr.h>

namespace
{
bool is_env_variable(const wxString& str, wxString* env_name)
{
    if (str.empty() || str[0] != '$') {
        return false;
    }
    env_name->reserve(str.length());

    // start from 1 to skip the prefix $
    for (size_t i = 1; i < str.length(); ++i) {
        wxChar ch = str[i];
        if (ch == '(' || ch == ')' || ch == '{' || ch == '}')
            continue;
        env_name->Append(ch);
    }
    return true;
}

/**
 * @brief expand an environment variable. use `env_map` to resolve any variables
 * accepting value in the form of (one example):
 * $PATH;C:\bin;$(LD_LIBRARY_PATH);${PYTHONPATH}
 */
wxString expand_env_variable(const wxString& value, const wxEnvVariableHashMap& env_map)
{
    // split the value into its parts
    wxArrayString parts = wxStringTokenize(value, wxPATH_SEP, wxTOKEN_STRTOK);
    wxString resolved;
    for (const wxString& part : parts) {
        wxString env_name;
        if (is_env_variable(part, &env_name)) {
            // try the environment variables first
            if (env_map.find(env_name) != env_map.end()) {
                resolved << env_map.find(env_name)->second;
            }
        } else {
            // literal
            resolved << part;
        }
        resolved << wxPATH_SEP;
    }
    if (!resolved.empty()) {
        resolved.RemoveLast();
    }
    return resolved;
}

clEnvList_t split_env_string(const wxString& env_str)
{
    clEnvList_t result;
    wxArrayString lines = ::wxStringTokenize(env_str, "\r\n", wxTOKEN_STRTOK);
    for (wxString& line : lines) {
        wxString key = line.BeforeFirst('=');
        wxString value = line.AfterFirst('=');
        if (key.empty()) {
            continue;
        }
        result.push_back({key, value});
    }
    return result;
}

} // namespace
std::string StringUtils::ToStdString(const wxString& str)
{
    const char* data = str.mb_str(wxConvUTF8).data();
    if (!data) {
        data = str.To8BitData();
    }

    std::string res;
    if (!data) {
        return res;
    }
    res = data;
    return res;
}

std::vector<std::string> StringUtils::ToStdStrings(const wxArrayString& strs)
{
    std::vector<std::string> res;
    res.reserve(strs.size());

    for (const auto& s : strs) {
        res.push_back(ToStdString(s));
    }
    return res;
}

std::vector<std::string> StringUtils::ToStdStrings(const std::vector<wxString>& strs)
{
    std::vector<std::string> res;
    res.reserve(strs.size());

    for (const auto& s : strs) {
        res.push_back(ToStdString(s));
    }
    return res;
}

int StringUtils::wxStringToInt(const wxString& str, int defval, int minval, int maxval)
{
    long v;
    if (!str.ToLong(&v)) {
        return defval;
    }

    if (minval != -1 && v < minval) {
        return defval;
    }
    if (maxval != -1 && v > maxval) {
        return defval;
    }

    return v;
}

wxString StringUtils::wxIntToString(int val)
{
    wxString s;
    s << val;
    return s;
}

unsigned int StringUtils::UTF8Length(const wchar_t* uptr, unsigned int tlen)
{
    constexpr unsigned int SURROGATE_LEAD_FIRST = 0xD800;
    // constexpr unsigned int SURROGATE_TRAIL_FIRST = 0xDC00;
    constexpr unsigned int SURROGATE_TRAIL_LAST = 0xDFFF;
    unsigned int len = 0;
    for (unsigned int i = 0; i < tlen && uptr[i];) {
        unsigned int uch = uptr[i];
        if (uch < 0x80) {
            len++;
        } else if (uch < 0x0800) {
            len += 2;
        } else if ((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_TRAIL_LAST)) {
            len += 4;
            i++;
        } else {
            len += 3;
        }
        i++;
    }
    return len;
}

#define BUFF_STATE_NORMAL 0
#define BUFF_STATE_IN_ESC 1
#define BUFF_STATE_IN_OSC 2

// see : https://en.wikipedia.org/wiki/ANSI_escape_code#Escape_sequences
void StringUtils::StripTerminalColouring(const std::string& buffer, std::string& modbuffer)
{
    modbuffer.reserve(buffer.length());
    short state = BUFF_STATE_NORMAL;
    for (const char& ch : buffer) {
        switch (state) {
        case BUFF_STATE_NORMAL:
            if (ch == 0x1B) { // found ESC char
                state = BUFF_STATE_IN_ESC;
            } else {
                modbuffer += ch;
            }
            break;
        case BUFF_STATE_IN_ESC:
            switch (ch) {
            case 'm':
            case 'K':
            case 'G':
            case 'J':
            case 'H':
            case 'X':
            case 'B':
            case 'C':
            case 'D':
            case 'd':
                state = BUFF_STATE_NORMAL;
                break;
            case ']':
                // Operating System Command
                state = BUFF_STATE_IN_OSC;
                break;
            default:
                break;
            }
            break;
        case BUFF_STATE_IN_OSC:
            if (ch == '\a') {
                // bell, leave the current state
                state = BUFF_STATE_NORMAL;
            }
            break;
        }
    }
    modbuffer.shrink_to_fit();
}

namespace
{
wxChar SafeGetChar(wxStringView buf, size_t pos)
{
    if (pos >= buf.length()) {
        return '\0';
    }
    return buf[pos];
}
} // namespace

// see : https://en.wikipedia.org/wiki/ANSI_escape_code#Escape_sequences
//  Operating System Command (OSC)
wxString StringUtils::StripTerminalOSC(const wxString& buffer)
{
    wxStringView sv{buffer.wc_str(), buffer.length()};
    return StripTerminalOSC(sv);
}

// see : https://en.wikipedia.org/wiki/ANSI_escape_code#Escape_sequences
//  Operating System Command (OSC)
wxString StringUtils::StripTerminalOSC(wxStringView buffer)
{
    wxString output;
    output.reserve(buffer.length());

    short state = BUFF_STATE_NORMAL;
    for (size_t i = 0; i < buffer.length(); ++i) {
        wxChar ch = SafeGetChar(buffer, i);
        wxChar next_ch = SafeGetChar(buffer, i + 1);
        switch (state) {
        case BUFF_STATE_NORMAL:
            if (ch == 0x1B && next_ch == ']') {
                state = BUFF_STATE_IN_OSC;
                i++;
            } else {
                output << ch;
            }
            break;
        case BUFF_STATE_IN_OSC:
            // possible terminators for this state
            if (ch == 0x07 /* BELL */) {
                // BELL, leave the current state
                state = BUFF_STATE_NORMAL;
            } else if (ch == 0x1B /* ESC */ && next_ch == 0x5C /* \\ */) {
                state = BUFF_STATE_NORMAL;
                ++i;
            }
            break;
        }
    }
    return output;
}

void StringUtils::StripTerminalColouring(const wxString& buffer, wxString& modbuffer)
{
    std::string source = ToStdString(buffer);
    std::string output;
    StripTerminalColouring(source, output);
    if (!output.empty()) {
        modbuffer = wxString(output.c_str(), wxConvUTF8);
        if (modbuffer.IsEmpty()) {
            modbuffer = wxString::From8BitData(output.c_str());
        }
    } else {
        modbuffer.Clear();
    }
}

void StringUtils::DisableMarkdownStyling(wxString& buffer)
{
    buffer.Replace("\\", "\\\\");
    buffer.Replace("#", "\\#");
    buffer.Replace("-", "\\-");
    buffer.Replace("=", "\\=");
    buffer.Replace("*", "\\*");
    buffer.Replace("~", "\\~");
    buffer.Replace("`", "\\`");
}

wxString StringUtils::DecodeURI(const wxString& uri)
{
    static const wxStringMap_t sEncodeMap = {{"%20", " "},
                                             {"%21", "!"},
                                             {"%23", "#"},
                                             {"%24", "$"},
                                             {"%26", "&"},
                                             {"%27", "'"},
                                             {"%28", "("},
                                             {"%29", ")"},
                                             {"%2A", "*"},
                                             {"%2B", "+"},
                                             {"%2C", ","},
                                             {"%3B", ";"},
                                             {"%3A", ":"},
                                             {"%3D", "="},
                                             {"%3F", "?"},
                                             {"%40", "@"},
                                             {"%5B", "["},
                                             {"%5D", "]"}};
    wxString decodedString;
    wxString escapeSeq;
    int state = 0;
    for (wxChar ch : uri) {
        switch (state) {
        case 0: // Normal
            switch (ch) {
            case '%':
                state = 1;
                escapeSeq << ch;
                break;
            default:
                decodedString << ch;
                break;
            }
            break;
        case 1: // Escaping mode
            escapeSeq << ch;
            if (escapeSeq.size() == 3) {
                // Try to decode it
                const auto iter = sEncodeMap.find(escapeSeq);
                if (iter != sEncodeMap.end()) {
                    decodedString << iter->second;
                } else {
                    decodedString << escapeSeq;
                }
                state = 0;
                escapeSeq.Clear();
            }
            break;
        }
    }
    return decodedString;
}

wxString StringUtils::EncodeURI(const wxString& uri)
{
    static const std::unordered_map<int, wxString> sEncodeMap = {{(int)'!', "%21"},
                                                                 {(int)'#', "%23"},
                                                                 {(int)'$', "%24"},
                                                                 {(int)'&', "%26"},
                                                                 {(int)'\'', "%27"},
                                                                 {(int)'(', "%28"},
                                                                 {(int)')', "%29"},
                                                                 {(int)'*', "%2A"},
                                                                 {(int)'+', "%2B"},
                                                                 {(int)',', "%2C"},
                                                                 {(int)';', "%3B"},
                                                                 {(int)'=', "%3D"},
                                                                 {(int)'?', "%3F"},
                                                                 {(int)'@', "%40"},
                                                                 {(int)'[', "%5B"},
                                                                 {(int)']', "%5D"},
                                                                 {(int)' ', "%20"}};

    wxString encoded;
    for (size_t i = 0; i < uri.length(); ++i) {
        wxChar ch = uri[i];
        const auto iter = sEncodeMap.find((int)ch);
        if (iter != sEncodeMap.end()) {
            encoded << iter->second;
        } else {
            encoded << ch;
        }
    }
    return encoded;
}

bool StringUtils::NextWord(const wxString& str, size_t& offset, wxString& word, bool makeLower)
{
    if (offset == str.size()) {
        return false;
    }
    size_t start = wxString::npos;
    word.Clear();
    for (; offset < str.size(); ++offset) {
        wxChar ch = str[offset];
        bool isWhitespace = ((ch == ' ') || (ch == '\t'));
        if (isWhitespace && (start != wxString::npos)) {
            // we found a trailing whitespace
            break;
        } else if (isWhitespace && (start == wxString::npos)) {
            // skip leading whitespace
            continue;
        } else if (start == wxString::npos) {
            start = offset;
        }
        if (makeLower) {
            ch = wxTolower(ch);
        }
        word << ch;
    }

    if ((start != wxString::npos) && (offset > start)) {
        return true;
    }
    return false;
}

wxString StringUtils::clJoinLinesWithEOL(const wxArrayString& lines, int eol)
{
    wxString glue = "\n";
    switch (eol) {
    case wxSTC_EOL_CRLF:
        glue = "\r\n";
        break;
    case wxSTC_EOL_CR:
        glue = "\r";
        break;
    default:
        glue = "\n";
        break;
    }
    return StringUtils::Join(lines, glue);
}

wxString StringUtils::wxImplode(const wxArrayString& arr, const wxString& glue)
{
    wxString str, tmp;
    for (size_t i = 0; i < arr.GetCount(); i++) {
        str << arr.Item(i) << glue;
    }

    if (str.EndsWith(glue, &tmp)) {
        str = tmp;
    }
    return str;
}

wxArrayString StringUtils::SplitString(const wxString& inString, bool trim)
{
    wxArrayString lines;
    wxString curline;

    bool inContinuation = false;
    for (size_t i = 0; i < inString.length(); ++i) {
        wxChar ch = inString.GetChar(i);
        wxChar ch1 = (i + 1 < inString.length()) ? inString.GetChar(i + 1) : wxUniChar(0);
        wxChar ch2 = (i + 2 < inString.length()) ? inString.GetChar(i + 2) : wxUniChar(0);

        switch (ch) {
        case '\r':
            // do nothing
            curline << ch;
            break;
        case '\n':
            if (inContinuation) {
                curline << ch;

            } else {
                lines.Add(trim ? curline.Trim().Trim(false) : curline);
                curline.clear();
            }
            inContinuation = false;
            break;
        case '\\':
            curline << ch;
            if ((ch1 == '\n') || (ch1 == '\r' && ch2 == '\n')) {
                inContinuation = true;
            }
            break;
        default:
            curline << ch;
            inContinuation = false;
            break;
        }
    }

    // any leftovers?
    if (curline.IsEmpty() == false) {
        lines.Add(trim ? curline.Trim().Trim(false) : curline);
        curline.clear();
    }
    return lines;
}

#define ARGV_STATE_NORMAL 0
#define ARGV_STATE_DQUOTE 1
#define ARGV_STATE_SQUOTE 2
#define ARGV_STATE_ESCAPE 3
#define ARGV_STATE_BACKTICK 4 // `
#define ARGV_STATE_DOLLAR 5   //$
#define ARGV_STATE_PAREN 6    // (

#define PUSH_CURTOKEN()          \
    {                            \
        if (!curstr.empty()) {   \
            A.push_back(curstr); \
            curstr.clear();      \
        }                        \
    }

namespace
{
int get_current_state(const std::vector<int>& states)
{
    if (states.empty()) {
        return ARGV_STATE_NORMAL;
    }
    return states[0];
}

int get_prev_state(const std::vector<int>& states)
{
    if (states.size() < 2) {
        return ARGV_STATE_NORMAL;
    }
    return states[1];
}

void push_state(std::vector<int>& states, int state) { states.insert(states.begin(), state); }
void pop_state(std::vector<int>& states) { states.erase(states.begin()); }
} // namespace

char** StringUtils::BuildArgv(const wxString& str, int& argc)
{
    std::vector<wxString> A;
    int dollar_paren_depth = 0;
    std::vector<int> states = {ARGV_STATE_NORMAL};
    wxString curstr;
    for (wxChar ch : str) {
        switch (get_current_state(states)) {
        case ARGV_STATE_NORMAL: {
            switch (ch) {
            case '$':
                curstr << ch;
                push_state(states, ARGV_STATE_DOLLAR);
                break;
            case ' ':
            case '\t':
            case ';':
                PUSH_CURTOKEN();
                break;
            case '\'':
                push_state(states, ARGV_STATE_SQUOTE);
                curstr << ch;
                break;
            case '"':
                push_state(states, ARGV_STATE_DQUOTE);
                curstr << ch;
                break;
            case '`':
                push_state(states, ARGV_STATE_BACKTICK);
                curstr << ch;
                break;
            case '\\':
                push_state(states, ARGV_STATE_ESCAPE);
                curstr << ch;
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        case ARGV_STATE_DOLLAR: {
            switch (ch) {
            case '(':
                curstr << ch;
                push_state(states, ARGV_STATE_PAREN);
                dollar_paren_depth++; // makes it 1
                break;
            case ';':
            case ' ':
            case '\t':
                PUSH_CURTOKEN();
                pop_state(states);
                break;
            default:
                curstr << ch;
                pop_state(states);
                break;
            }
        } break;
        case ARGV_STATE_PAREN: {
            switch (ch) {
            case '(':
                curstr << ch;
                dollar_paren_depth++; // increase the depth
                break;
            case ')':
                curstr << ch;
                dollar_paren_depth--; // reduce the depth
                // if the depth reached 0, we should leave this state
                if (dollar_paren_depth == 0) {
                    // leave the state
                    pop_state(states);
                }
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        case ARGV_STATE_ESCAPE: {
            if (get_prev_state(states) == ARGV_STATE_NORMAL) {
                curstr << ch;
                pop_state(states);
                break;
            } else if (get_prev_state(states) == ARGV_STATE_DQUOTE) {
                switch (ch) {
                case '"':
                    curstr << "\"";
                    pop_state(states);
                    break;
                default:
                    curstr << "\\" << ch;
                    pop_state(states);
                    break;
                }
            } else if (get_prev_state(states) == ARGV_STATE_BACKTICK) {
                switch (ch) {
                case '`':
                    curstr << "`";
                    pop_state(states);
                    break;
                default:
                    curstr << "\\" << ch;
                    pop_state(states);
                    break;
                }
            } else { // single quote
                switch (ch) {
                case '\'':
                    curstr << "'";
                    pop_state(states);
                    break;
                default:
                    curstr << "\\" << ch;
                    pop_state(states);
                    break;
                }
            }
        } break;
        case ARGV_STATE_DQUOTE: {
            switch (ch) {
            case '\\':
                push_state(states, ARGV_STATE_ESCAPE);
                break;
            case '"':
                curstr << ch;
                pop_state(states);
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        case ARGV_STATE_SQUOTE: {
            switch (ch) {
            case '\\':
                push_state(states, ARGV_STATE_ESCAPE);
                break;
            case '\'':
                curstr << ch;
                pop_state(states);
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        case ARGV_STATE_BACKTICK: {
            switch (ch) {
            case '\\':
                push_state(states, ARGV_STATE_ESCAPE);
                break;
            case '`':
                curstr << ch;
                pop_state(states);
                break;
            default:
                curstr << ch;
                break;
            }
        } break;
        }
    }

    if (!curstr.IsEmpty()) {
        A.push_back(curstr);
    }

    if (A.empty()) {
        return nullptr;
    }

    char** argv = new char*[A.size() + 1];
    argv[A.size()] = NULL;
    for (size_t i = 0; i < A.size(); ++i) {
        argv[i] = strdup(A[i].mb_str(wxConvUTF8).data());
    }
    argc = (int)A.size();
    return argv;
}

void StringUtils::FreeArgv(char** argv, int argc)
{
    for (int i = 0; i < argc; ++i) {
        free(argv[i]);
    }
    delete[] argv;
}

wxArrayString StringUtils::BuildArgv(const wxString& str)
{
    // Don't bother with an empty command line
    wxString trimmed_str = str;
    trimmed_str.Trim().Trim(false);
    if (trimmed_str.empty()) {
        return {};
    }

    int argc = 0;
    char** argv = BuildArgv(trimmed_str, argc);
    wxArrayString arrArgv;
    for (int i = 0; i < argc; ++i) {
        arrArgv.Add(argv[i]);
    }
    FreeArgv(argv, argc);

    for (wxString& s : arrArgv) {
        if ((s.length() > 1) && s.StartsWith("\"") && s.EndsWith("\"")) {
            s.RemoveLast().Remove(0, 1);
        }
    }
    return arrArgv;
}

clEnvList_t StringUtils::BuildEnvFromString(const wxString& envstr) { return split_env_string(envstr); }

clEnvList_t StringUtils::ResolveEnvList(const clEnvList_t& env_list)
{
    wxEnvVariableHashMap current_env;
    ::wxGetEnvMap(&current_env);

    for (auto [env_var_name, env_var_value] : env_list) {
        env_var_value = expand_env_variable(env_var_value, current_env);
        current_env.erase(env_var_name);
        current_env.insert({env_var_name, env_var_value});
    }

    clEnvList_t result;
    result.reserve(current_env.size());

    // convert the hash map into list and return it
    for (const auto& [env_var_name, env_var_value] : current_env) {
        result.push_back({env_var_name, env_var_value});
    }
    return result;
}

clEnvList_t StringUtils::ResolveEnvList(const wxString& envstr)
{
    auto source_list = BuildEnvFromString(envstr);
    return ResolveEnvList(source_list);
}

wxArrayString StringUtils::BuildCommandArrayFromString(const wxString& command)
{
    wxArrayString lines = ::wxStringTokenize(command, "\n", wxTOKEN_STRTOK);

    wxArrayString command_array;
    command_array.reserve(lines.size());

    for (auto& line : lines) {
        line.Trim().Trim(false);
        if (line.StartsWith("#") || line.IsEmpty()) {
            continue;
        }

        line = line.BeforeFirst('#'); // remove trailing comment

        int count = 0;
        auto argv = BuildArgv(line, count);
        for (int i = 0; i < count; ++i) {
            command_array.push_back(argv[i]);
        }
        StringUtils::FreeArgv(argv, count);
    }
    return command_array;
}

wxString StringUtils::BuildCommandStringFromArray(const wxArrayString& command_arr, size_t flags)
{
    wxString command;

    bool span_multiple_lines = !(flags & ONE_LINER);
    bool include_comment_block = (flags & WITH_COMMENT_PREFIX);

    if (span_multiple_lines && include_comment_block) {
        command << "# Command to execute:\n";
        command << "\n";
    }

    const wxString SPACE = span_multiple_lines ? "  " : " ";
    const wxString COMMAND_SEPARATOR = span_multiple_lines ? "\n" : " ";

    for (size_t i = 0; i < command_arr.size(); ++i) {
        if (i > 0) {
            command << SPACE;
        }
        command << command_arr[i] << COMMAND_SEPARATOR;
    }
    return command;
}

wxString& StringUtils::WrapWithQuotes(wxString& str)
{
    if (!str.empty() && str.Contains(" ") && !str.StartsWith("\"") && !str.EndsWith("\"")) {
        str.Prepend("\"").Append("\"");
    }
    return str;
}

wxString StringUtils::WrapWithDoubleQuotes(const wxString& str)
{
    if (str.Contains(" ") && !str.StartsWith("\"") && !str.EndsWith("\"")) {
        return "\"" + str + "\"";
    }
    return str;
}

wxString StringUtils::StripDoubleQuotes(const wxString& str)
{
    wxString s = str;
    if (str.StartsWith("\"")) {
        s.Remove(0, 1);
    }

    if (str.EndsWith("\"")) {
        s.RemoveLast();
    }
    return s;
}

wxArrayString StringUtils::AppendAndMakeUnique(const wxArrayString& arr, const wxString& str, size_t truncate_size)
{
    wxArrayString unique_arr;
    unique_arr.reserve(arr.size());

    for (const auto& s : arr) {
        if (s != str) {
            unique_arr.push_back(s);
        }
    }

    unique_arr.Insert(str, 0);
    if (unique_arr.size() > truncate_size) {
        unique_arr.resize(truncate_size);
    }
    return unique_arr;
}
