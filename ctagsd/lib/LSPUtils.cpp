#include "LSPUtils.hpp"

#include "SimpleTokenizer.hpp"
#include "macros.h"

#include <array>
#include <wx/filesys.h>

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

LSP::eSymbolKind LSPUtils::get_symbol_kind(const TagEntry* tag)
{
    LSP::eSymbolKind kind = LSP::eSymbolKind::kSK_Variable;
    if(tag->IsMethod()) {
        kind = LSP::eSymbolKind::kSK_Method;
    } else if(tag->IsConstructor()) {
        kind = LSP::eSymbolKind::kSK_Class;
    } else if(tag->IsClass()) {
        kind = LSP::eSymbolKind::kSK_Class;
    } else if(tag->IsStruct()) {
        kind = LSP::eSymbolKind::kSK_Struct;
    } else if(tag->IsLocalVariable()) {
        kind = LSP::eSymbolKind::kSK_Variable;
    } else if(tag->IsTypedef()) {
        kind = LSP::eSymbolKind::kSK_TypeParameter;
    } else if(tag->IsMacro()) {
        kind = LSP::eSymbolKind::kSK_TypeParameter;
    } else if(tag->GetKind() == "namespace") {
        kind = LSP::eSymbolKind::kSK_Module;
    } else if(tag->GetKind() == "union") {
        kind = LSP::eSymbolKind::kSK_Struct;
    } else if(tag->GetKind() == "enum") {
        kind = LSP::eSymbolKind::kSK_Enum;
    } else if(tag->GetKind() == "enumerator") {
        kind = LSP::eSymbolKind::kSK_EnumMember;
    }
    return kind;
}

LSP::CompletionItem::eCompletionItemKind LSPUtils::get_completion_kind(const TagEntry* tag)
{
    LSP::CompletionItem::eCompletionItemKind kind = LSP::CompletionItem::kKindVariable;
    if(tag->IsMethod()) {
        kind = LSP::CompletionItem::kKindFunction;
    } else if(tag->IsConstructor()) {
        kind = LSP::CompletionItem::kKindClass;
    } else if(tag->IsClass()) {
        kind = LSP::CompletionItem::kKindClass;
    } else if(tag->IsStruct()) {
        kind = LSP::CompletionItem::kKindStruct;
    } else if(tag->IsLocalVariable()) {
        kind = LSP::CompletionItem::kKindVariable;
    } else if(tag->IsTypedef()) {
        kind = LSP::CompletionItem::kKindTypeParameter;
    } else if(tag->IsMacro()) {
        kind = LSP::CompletionItem::kKindTypeParameter;
    } else if(tag->GetKind() == "namespace") {
        kind = LSP::CompletionItem::kKindModule;
    } else if(tag->GetKind() == "union") {
        kind = LSP::CompletionItem::kKindStruct;
    } else if(tag->GetKind() == "enum") {
        kind = LSP::CompletionItem::kKindEnum;
    } else if(tag->GetKind() == "enumerator") {
        kind = LSP::CompletionItem::kKindEnumMember;
    } else if(tag->GetKind() == "file") {
        kind = LSP::CompletionItem::kKindTypeParameter;
    }
    return kind;
}

vector<LSP::SymbolInformation> LSPUtils::to_symbol_information_array(const vector<TagEntryPtr>& tags,
                                                                     bool for_tree_view)
{
    vector<LSP::SymbolInformation> result;
    result.reserve(tags.size());

    wxStringSet_t parent_seen;
    for(auto tag : tags) {
        LSP::SymbolInformation symbol_information;
        to_symbol_information(tag.Get(), symbol_information, for_tree_view ? &parent_seen : nullptr);
        result.push_back(symbol_information);
    }
    return result;
}

vector<LSP::SymbolInformation> LSPUtils::to_symbol_information_array(const vector<TagEntry>& tags, bool for_tree_view)
{
    vector<LSP::SymbolInformation> result;
    result.reserve(tags.size());

    wxStringSet_t parent_seen;
    for(const auto& tag : tags) {
        LSP::SymbolInformation symbol_information;
        to_symbol_information(&tag, symbol_information, for_tree_view ? &parent_seen : nullptr);
        result.push_back(symbol_information);
    }
    return result;
}

void LSPUtils::to_symbol_information(const TagEntry* tag, LSP::SymbolInformation& symbol_information,
                                     unordered_set<wxString>* parents_seen)
{
    if(parents_seen && tag->IsContainer()) {
        parents_seen->insert(tag->GetName());
    }

    LSP::Location loc;
    LSP::Range range;
    range.SetStart({ tag->GetLine() - 1, 0 });
    range.SetEnd({ tag->GetLine() - 1, 0 });
    loc.SetRange(range);
    loc.SetPath(FileUtils::FilePathToURI(tag->GetFile()));

    symbol_information.SetKind(get_symbol_kind(tag));

    wxString display_name = tag->GetDisplayName();
    wxString display_name_full = tag->GetFullDisplayName();

    if(tag->IsMethod() && tag->GetName().StartsWith("__anon")) {
        // Lambda
        display_name = display_name_full = "[lambda]" + tag->GetSignature();
    } else if(tag->IsNamespace() && tag->GetName().StartsWith("__anon")) {
        display_name = display_name_full = "(anonymous namespace)";
    }

    if(parents_seen) {
        if(parents_seen->count(tag->GetParent())) {
            symbol_information.SetContainerName(tag->GetParent());
            symbol_information.SetName(display_name);
        } else {
            // don't bother adding parent that we did not
            // see until this point
            symbol_information.SetName(display_name_full);
        }

    } else {
        symbol_information.SetContainerName(tag->GetParent());
        symbol_information.SetName(display_name);
    }
    symbol_information.SetLocation(loc);
}
