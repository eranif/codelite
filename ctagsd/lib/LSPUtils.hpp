#ifndef LSPUTILS_HPP
#define LSPUTILS_HPP

#include "SimpleTokenizer.hpp"
#include <vector>

using namespace std;

enum eTokenType {
    TYPE_VARIABLE,
    TYPE_CLASS,
};

struct TokenWrapper {
    SimpleTokenizer::Token token;
    eTokenType type = TYPE_VARIABLE;
};

class LSPUtils
{
public:
    LSPUtils();
    ~LSPUtils();

    static void encode_semantic_tokens(const vector<TokenWrapper>& tokens_vec, vector<int>* encoded_arr);
};

#endif // LSPUTILS_HPP
