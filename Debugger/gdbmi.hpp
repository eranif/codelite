#ifndef GDBMI_HPP
#define GDBMI_HPP

#include "wxStringHash.h"
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <wx/string.h>

namespace gdbmi
{
enum eToken {
    T_LIST_OPEN = 1, // [
    T_LIST_CLOSE,    // ]
    T_TUPLE_OPEN,    // {
    T_TUPLE_CLOSE,   // }
    T_POW,           // ^
    T_STAR,          // *
    T_PLUS,          // +
    T_EQUAL,         // =
    T_TARGET_OUTPUT, // @
    T_STREAM_OUTPUT, // ~
    T_LOG_OUTPUT,    // &
    T_COMMA,         // ,
    T_CSTRING,       // string
    T_WORD,          // Token
    T_DONE,          // done
    T_RUNNING,       // running
    T_CONNECTED,     // connected
    T_ERROR,         // error
    T_EXIT,          // exit
    T_STOPPED,       // stopped
    T_EOF,
};

enum eLineType {
    LT_INVALID = -1,
    LT_RESULT,                // line starting with ^ (with optional txid)
    LT_STATUS_ASYNC_OUTPUT,   // line starting with +
    LT_EXEC_ASYNC_OUTPUT,     // line starting with *
    LT_NOTIFY_ASYNC_OUTPUT,   // line starting with =
    LT_CONSOLE_STREAM_OUTPUT, // line starting with ~
    LT_TARGET_STREAM_OUTPUT,  // line starting with @
    LT_LOG_STREAM_OUTPUT,     // line starting with &
};

struct StringView {
    const wxChar* m_pdata = nullptr;
    size_t m_length = 0;

    wxString to_string() const
    {
        if(!m_pdata) {
            return wxString();
        } else {
            return wxString(m_pdata, m_length);
        }
    }

    StringView() {}
    StringView(const wxString& buffer)
        : StringView(buffer.c_str(), buffer.length())
    {
    }

    StringView(const wxChar* p, size_t len)
    {
        m_pdata = p;
        m_length = len;
    }

    const wxChar* data() const { return m_pdata; }
    size_t length() const { return m_length; }
    char operator[](size_t index) const { return m_pdata[index]; }
    bool empty() const { return m_length == 0; }
};

class Tokenizer
{
    size_t m_pos = 0;
    StringView m_buffer;

protected:
    StringView read_string(eToken* type);
    StringView read_word(eToken* type);

public:
    Tokenizer(StringView buffer);
    StringView next_token(eToken* type);
    /**
     * @brief return the remainder string from m_pos -> end
     */
    StringView remainder();
};

struct Node {
public:
    typedef std::shared_ptr<Node> ptr_t;
    typedef std::vector<ptr_t> vec_t;

private:
    ptr_t do_add_child(const wxString& name)
    {
        children.emplace_back(std::make_shared<Node>());
        auto child = children.back();
        child->name = std::move(name);
        children_map.insert({ child->name, child });
        return child;
    }

public:
    wxString name;
    wxString value; // optional
    vec_t children;
    std::unordered_map<wxString, ptr_t> children_map;

    Node() {}
    Node& find_child(const wxString& name) const;
    Node& operator[](const wxString& name) const { return find_child(name); }
    Node& operator[](size_t index) const
    {
        if(index >= children.size()) {
            thread_local Node emptyNode;
            return emptyNode;
        }
        return *(children[index].get());
    }

    ptr_t add_child()
    {
        wxString s;
        s << children.size();
        return do_add_child(s);
    }

    ptr_t add_child(const wxString& name, const wxString& value = {});
    bool exists(const wxString& name) const { return children_map.count(name) > 0; }
};

struct ParsedResult {
    eLineType line_type = LT_INVALID;
    StringView line_type_context; // depends on the line type, this will hold the context string
    StringView txid;              //  optional
    Node::ptr_t tree = std::make_shared<Node>();
    Node& operator[](const wxString& index) const { return tree->find_child(index); }
    bool exists(const wxString& name) const { return tree->exists(name); }
};

class Parser
{
private:
    void parse_properties(Tokenizer* tokenizer, Node::ptr_t parent);

public:
    void parse(const wxString& buffer, ParsedResult* result);
    void print(Node::ptr_t node, int depth = 0);
};
} // namespace gdbmi

#endif // GDBMI_HPP
