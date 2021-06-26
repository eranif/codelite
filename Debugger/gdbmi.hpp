#ifndef GDBMI_HPP
#define GDBMI_HPP

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace gdbmi
{
enum eToken {
    T_LIST_OPEN = 1, // [
    T_LIST_CLOSE,    // ]
    T_TUPLE_OPEN,    // {
    T_TUPLE_CLOSE,   // }
    T_EQUAL,         // =
    T_POW,           // ^
    T_STAR,          // *
    T_PLUS,          // +
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

struct StringView {
    const char* m_pdata = nullptr;
    size_t m_length = 0;

    std::string to_string() const
    {
        if(!m_pdata) {
            return std::string();
        } else {
            return std::string(m_pdata, m_length);
        }
    }

    StringView() {}
    StringView(const std::string& buffer)
        : StringView(buffer.c_str(), buffer.length())
    {
    }

    StringView(const char* p, size_t len)
    {
        m_pdata = p;
        m_length = len;
    }

    const char* data() const { return m_pdata; }
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
};

struct Node {
public:
    typedef std::shared_ptr<Node> ptr_t;
    typedef std::vector<ptr_t> vec_t;

private:
    ptr_t do_add_child(const std::string& name)
    {
        children.emplace_back(std::make_shared<Node>());
        auto child = children.back();
        child->name = std::move(name);
        children_map.insert({ child->name, child });
        return child;
    }

public:
    std::string name;
    std::string value; // optional
    vec_t children;
    std::unordered_map<std::string, ptr_t> children_map;

    Node& find_child(const std::string& name)
    {
        if(children_map.count(name) == 0) {
            thread_local Node emptyNode;
            return emptyNode;
        }
        return *children_map[name].get();
    }

    Node& operator[](const std::string& name) { return find_child(name); }

    ptr_t add_child()
    {
        std::stringstream ss;
        ss << children.size();
        return do_add_child(ss.str());
    }

    ptr_t add_child(std::string name, std::string value = {});
    bool exists(const std::string& name) const { return children_map.count(name) > 0; }
};

struct ParsedResult {
    StringView txid;
    StringView result_class;
    Node::ptr_t tree = std::make_shared<Node>();
    Node& operator[](const std::string& index) const { return tree->find_child(index); }
    bool exists(const std::string& name) const { return tree->exists(name); }
};

class Parser
{
private:
    void parse_properties(Tokenizer* tokenizer, Node::ptr_t parent);

public:
    void parse(const std::string& buffer, ParsedResult* result);
    void print(Node::ptr_t node, int depth = 0);
};
} // namespace gdbmi

#endif // GDBMI_HPP
