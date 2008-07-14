#ifndef GDBLEXER_H
#define GDBLEXER_H

#include <string>

#define LE_GDB_WHITE 257 
#define LE_GDB_WORD 258
#define LE_GDB_DONE 259
#define LE_GDB_NAME 260
#define LE_GDB_VALUE 261
#define LE_GDB_LOCALS 263
#define LE_GDB_STRING_LITERAL 264
#define LE_GDB_CHAR_LITERAL 265

extern int le_gdb_lex();
extern bool le_gdb_set_input(const std::string &in);
extern std::string le_gdb_string_word;
extern char *le_gdb_text;
extern void le_gdb_lex_clean();
extern void le_gdb_push_buffer(const std::string &new_input);
extern void le_gdb_pop_buffer();

#endif //GDBLEXER_H


