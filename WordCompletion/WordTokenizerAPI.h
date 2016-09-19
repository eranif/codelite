#ifndef WORD_TOKENIZER_API_H
#define WORD_TOKENIZER_API_H

#include <wx/string.h>
// Needed as per GH-1364
#ifndef __WIN32
#include <unistd.h>
#endif

extern int isatty(int);

typedef void* WordScanner_t;
struct WordLexerToken
{
    char* text;
    int type;
    WordLexerToken()
        : text(NULL)
        , type(0)
    {
    }
};

// Types
enum eWordTokens {
    kWordDelim = 600,
    kWordNumber,
};

void WordLexerDestroy(WordScanner_t* scanner);
WordScanner_t WordLexerNew(const wxString& buffer);
bool WordLexerNext(WordScanner_t scanner, WordLexerToken& token);

#endif
