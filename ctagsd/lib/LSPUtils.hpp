#ifndef LSPUTILS_HPP
#define LSPUTILS_HPP

#include "LSP/CompletionItem.h"
#include "LSP/basic_types.h"
#include "SimpleTokenizer.hpp"
#include "entry.h"

#include <unordered_set>
#include <vector>

enum eTokenType {
    TYPE_VARIABLE,
    TYPE_CLASS,
    TYPE_FUNCTION,
};

struct TokenWrapper {
    SimpleTokenizer::Token token;
    eTokenType type = TYPE_VARIABLE;
};

class LSPUtils
{
private:
    static void to_symbol_information(const TagEntry* tag, LSP::SymbolInformation& symbol_information,
                                      std::unordered_set<wxString>* parents_seen = nullptr);

public:
    LSPUtils();
    ~LSPUtils();

    static void encode_semantic_tokens(const std::vector<TokenWrapper>& tokens_vec, std::vector<int>* encoded_arr);
    static LSP::eSymbolKind get_symbol_kind(const TagEntry* tag);
    static LSP::CompletionItem::eCompletionItemKind get_completion_kind(const TagEntry* tag);
    static std::vector<LSP::SymbolInformation> to_symbol_information_array(const std::vector<TagEntryPtr>& tags,
                                                                           bool for_tree_view);
    static std::vector<LSP::SymbolInformation> to_symbol_information_array(const std::vector<TagEntry>& tags,
                                                                           bool for_tree_view);
};

#endif // LSPUTILS_HPP
