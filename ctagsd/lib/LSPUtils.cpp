#include "LSPUtils.hpp"
#include "SimpleTokenizer.hpp"
#include <array>

LSPUtils::LSPUtils() {}

LSPUtils::~LSPUtils() {}

void LSPUtils::encode_semantic_tokens(const vector<TokenWrapper>& tokens_vec, vector<int>* encoded_arr)
{
    TokenWrapper dummy;
    dummy.token = SimpleTokenizer::Token(0, 0, 0, 0);

    encoded_arr->reserve(tokens_vec.size() * 5);
    const auto* prev_token_ptr = &dummy.token;
    for(size_t i = 0; i < tokens_vec.size(); ++i) {
        auto* current_token_ptr = &tokens_vec[i].token;
        array<int, 5> info;

        bool changed_line = prev_token_ptr->line() != current_token_ptr->line();
        if(changed_line) {
            info[0] = current_token_ptr->line() - prev_token_ptr->line(); // keep the delta
        } else {
            // we are the same line, set it to 0
            info[0] = 0;
        }

        // represent each column relative the previous token start column
        if(changed_line) {
            info[1] = current_token_ptr->column();
        } else {
            info[1] = current_token_ptr->column() -
                      prev_token_ptr->column(); // keep the delta to the previous token on this line
        }

        // length
        info[2] = current_token_ptr->length();

        // token type
        info[3] = tokens_vec[i].type;

        // modifiers
        info[4] = changed_line ? 99 : 0;
        encoded_arr->insert(encoded_arr->end(), info.begin(), info.end());

        // update the previous token
        prev_token_ptr = current_token_ptr;
    }
}