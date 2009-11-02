#ifndef _GDB_PARSER_H_
#define _GDB_PARSER_H_

#include <vector>
#include <map>
#include <string>

extern std::string gdb_result_string;

extern bool setGdbLexerInput(const std::string &in, bool ascii, bool wantWhitespace = false);
extern void gdbParseListChildren( const std::string &in, std::vector<std::map<std::string, std::string> > &children);
extern void gdb_parse_result(const std::string &in);
extern void gdb_result_push_buffer(const std::string &new_input);
extern void gdb_result_pop_buffer();
extern int  gdb_result_lex();
extern void gdb_result_lex_clean();

#endif // _GDB_PARSER_H_
