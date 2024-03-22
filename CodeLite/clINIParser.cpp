#include "clINIParser.hpp"

#include "fileutils.h"

#include <algorithm>

namespace
{
enum class State {
    Key,
    Value,
    Comment,
    SingleQuote,
    DoubleQuote,
    Section,
};
};

void clINISection::AddComment(const wxString& comment) { m_entries.push_back(comment); }

void clINISection::AddKeyValue(const wxString& key, const wxString& value)
{
    clINIKeyValue kv;
    kv.SetName(key);
    kv.SetValue(value);
    m_entries.push_back(kv);
}

void clINISection::Serialise(std::ostream& string) const
{
    if (GetName().empty() && m_entries.empty())
        return;

    if (!GetName().empty()) {
        string << "[" << GetName() << "]\n";
    }

    for (const auto& kv : m_entries) {
        if (kv.CheckType<wxString>()) {
            wxString comment;
            if (kv.GetAs(&comment)) {
                string << "# " << comment << "\n";
            }
        } else if (kv.CheckType<clINIKeyValue>()) {
            clINIKeyValue pair;
            if (kv.GetAs(&pair)) {
                string << pair.GetKey() << " = " << pair.GetValue() << "\n";
            }
        }
    }
    string << "\n";
}

clINIKeyValue clINISection::operator[](const char* key_name) const
{
    wxString name = key_name;
    return operator[](name);
}

clINIKeyValue clINISection::operator[](const wxString& key_name) const
{
    for (const auto& d : m_entries) {
        if (d.CheckType<clINIKeyValue>()) {
            clINIKeyValue kv;
            if (d.GetAs(&kv) && kv.GetKey() == key_name) {
                return kv;
            }
        }
    }
    return {};
}

clINIParser::clINIParser() {}

clINIParser::~clINIParser() {}

bool clINIParser::ParseFile(const wxString& filepath)
{
    wxString content;
    if (!FileUtils::ReadFileContent(filepath, content)) {
        return false;
    }
    return ParseString(content);
}

bool clINIParser::ParseString(const wxString& content)
{
    State state = State::Key;

    m_sections.clear();
    m_sections.emplace_back();

    wxString key;
    wxString value;
    wxString comment;
    wxString section_name;
    for (const wxChar& ch : content) {
        // ignore pesky whitespaces characters
        if (ch == '\r' || ch == '\v') {
            continue;
        }
        if (ch == '\n') {
            // Process what we collected so far
            if (!section_name.empty()) {
                // start a new section
                m_sections.emplace_back();
                m_sections.back().SetName(section_name);
            }

            auto& cur_section = m_sections.back();
            if (!comment.empty()) {
                cur_section.AddComment(comment);
            }

            if (!key.empty()) {
                if (state == State::DoubleQuote || state == State::SingleQuote) {
                    // add the value, untouched
                    cur_section.AddKeyValue(key, value);
                } else {
                    // add the value, trim it before
                    value.Trim().Trim(false);
                    cur_section.AddKeyValue(key, value);
                }
            }

            // reset the state
            state = State::Key;

            // reset the collected values
            key = value = comment = section_name = "";
            continue;
        }
        switch (state) {
        case State::Key:
            switch (ch) {
            case '#':
            case ';':
                state = State::Comment;
                break;
            case '=':
                state = State::Value;
                break;
            case '[':
                state = State::Section;
                break;
            case ' ':
            case '\t':
            case '\r':
                // skip whitespace in key
                break;
            default:
                key << ch;
                break;
            }
            break;
        case State::Value:
            // collect everything as the value
            // take strings + comments into consideration
            switch (ch) {
            case '#':
            case ';':
                state = State::Comment;
                break;
            case '\'':
                state = State::SingleQuote;
                break;
            case ' ':
            case '\t':
                if (!value.empty()) {
                    value << ch;
                }
                break;
            case '"':
                state = State::DoubleQuote;
                break;
            default:
                value << ch;
                break;
            }
            break;
        case State::Comment:
            comment << ch;
            break;
        case State::SingleQuote:
            switch (ch) {
            case '\'':
                // we could only get here while parsing a value
                state = State::Value;
                break;
            default:
                value << ch;
                break;
            }
            break;
        case State::DoubleQuote:
            switch (ch) {
            case '"':
                // we could only get here while parsing a value
                state = State::Value;
                break;
            default:
                value << ch;
                break;
            }
            break;
        case State::Section:
            switch (ch) {
            case ']':
                state = State::Key;
                break;
            default:
                section_name << ch;
                break;
            }
            break;
        }
    }
    return false;
}

void clINIParser::Serialise(std::ostream& string) const
{
    for (const auto& section : m_sections) {
        section.Serialise(string);
    }
}

thread_local clINISection clNullINISection;
const clINISection& clINIParser::operator[](const char* section) const
{
    wxString name = section;
    return operator[](name);
}

const clINISection& clINIParser::operator[](const wxString& section_name) const
{
    auto where = std::find_if(m_sections.begin(), m_sections.end(),
                              [&section_name](const clINISection& d) -> bool { return d.GetName() == section_name; });
    if (where == m_sections.end()) {
        return clNullINISection;
    }
    return *where;
}

bool clINIParser::HasSection(const wxString& section_name) const
{
    auto where = std::find_if(m_sections.begin(), m_sections.end(),
                              [&section_name](const clINISection& d) -> bool { return d.GetName() == section_name; });
    return where != m_sections.end();
}
