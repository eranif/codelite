#ifndef WordsTokens_H
#define WordsTokens_H

enum {
    kWORD_T_WORD = 500,
    kWORD_T_STRING,
    kWORD_T_NUMBER,
};

struct WordToken {
    wxString word;
    int type;
};

//---------------------------
// API
//---------------------------
void* wordsLexerNew(const wxString& content, size_t options = 0);
void wordsLexerDestroy(void** scanner);
bool wordsLexerNext(void* scanner, WordToken& token);

#endif // WordsTokens_H
