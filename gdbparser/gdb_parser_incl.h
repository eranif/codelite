#ifndef _GDB_PARSER_H_
#define _GDB_PARSER_H_

#include <vector>
#include <map>
#include <string>

extern std::string gdb_result_string;
using GdbStringMap_t = std::map<std::string, std::string>;
using GdbChildren_t = std::vector<GdbStringMap_t>;

struct GdbChildrenInfo {
    GdbChildren_t children;
    bool          has_more;

    GdbChildrenInfo() {
        clear();
    }

    void push_back( const GdbStringMap_t& m ) {
        children.push_back( m );
    }

    void clear() {
        children.clear();
        has_more = false;
    }

    void print() const {
        printf("has_more : %d\n", has_more ? 1 : 0);
        for (const auto& child : children) {
            printf("--------------\n");
            for (const auto& p : child) {
                printf("%s : %s\n", p.first.c_str(), p.second.c_str());
            }
        }
    }
    
};

extern bool setGdbLexerInput(const std::string &in, bool ascii, bool wantWhitespace = false);
extern void gdbParseListChildren( const std::string &in, GdbChildrenInfo &children);
extern void gdbParseRegisterNames( const std::string &in, std::vector<std::string>& names );
extern void gdb_parse_result(const std::string &in);
extern void gdb_result_push_buffer(const std::string &new_input);
extern void gdb_result_pop_buffer();
extern int  gdb_result_lex();
extern void gdb_result_lex_clean();

#endif // _GDB_PARSER_H_
