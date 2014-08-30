#ifndef PHP_PARSER_CONSTANTS_H
#define PHP_PARSER_CONSTANTS_H

// YACC and FLEX is based on:
// http://svn.php.net/repository/php/php-src/trunk/Zend/zend_language_parser.y
// http://svn.php.net/repository/php/php-src/trunk/Zend/zend_language_scanner.l

#include <wx/string.h>

#ifdef YYSTYPE
#undef YYSTYPE
#endif
#define YYSTYPE wxString

#include "php_parser.cpp.h"
#include <vector>
#include <wx/arrstr.h>
#include <vector>
#include <map>
#include <list>

extern int php_lineno;

enum PHPModifier {
    PHP_Attr_None        = 0x00000000,
    PHP_Attr_Static      = 0x00000001,
    PHP_Attr_Abstract    = 0x00000002,
    PHP_Attr_Final       = 0x00000004,
    PHP_Access_Protected = 0x00000008,
    PHP_Access_Private   = 0x00000010,
    PHP_Access_Public    = 0x00000020,
    PHP_Attr_Const       = 0x00000040,
    PHP_Attr_Reference   = 0x00000080,
    PHP_Attr_Global      = 0x00000100
};

enum PHPEntryKind {
    PHP_Kind_File    = -2,                 // -2, needed for other purposes this is why we place under the 'PHP_Kind_First'
    PHP_Kind_Unknown = -1,                 // -1
    PHP_Kind_First = 0,                    // 0
    PHP_Kind_Function = PHP_Kind_First,    // 0
    PHP_Kind_Variable,                     // 1
    PHP_Kind_Constant,                     // 2
    PHP_Kind_Member,                       // 3
    PHP_Kind_This,                         // 4
    PHP_Kind_Namespace,                    // 5
    PHP_Kind_Class,                        // 6
    PHP_Kind_Max,                          // 7
};

extern wxString GetAttributes(size_t flags);

#endif // PHP_PARSER_CONSTANTS_H
