//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : gdblexer.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
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
#endif //GDBLEXER_H


