#include "LSPSymbolParser.h"

namespace LSP
{   
    // eSymbolKind preference when sorting symbols
    const std::vector<eSymbolKind> LSPSymbolParser::SymbolKindOrder = {
        kSK_File,
        kSK_Module,
        kSK_Namespace,
        kSK_Package,
        kSK_Class,
        kSK_Struct,
        kSK_Object,
        kSK_Interface,
        kSK_Enum,
        kSK_EnumMember,
        kSK_Constructor,
        kSK_Method,
        kSK_Function,
        kSK_Operator,
        kSK_Property,
        kSK_Field,
        kSK_Variable,
        kSK_Constant,
        kSK_String,
        kSK_Number,
        kSK_Boolean,
        kSK_Array,
        kSK_Key,
        kSK_Event,
        kSK_TypeParameter,
        kSK_Container,
        kSK_Null
    };
        
    void LSPSymbolParser::Sort(std::vector<LSP::DocumentSymbol>& symbols, SortType sort, const std::vector<eSymbolKind>& symbolKindOrder) 
    {
        std::sort(symbols.begin(), symbols.end(), [symbolKindOrder, sort](const DocumentSymbol& a, const DocumentSymbol& b) 
            {
                switch(sort) {
                    case SORT_NAME: 
                         return a.GetName().CmpNoCase(b.GetName()) < 0; 
                    case SORT_NAME_CASE_SENSITIVE: 
                        return a.GetName().Cmp(b.GetName()) < 0;   
                    case SORT_KIND_NAME: wxFALLTHROUGH;
                    case SORT_KIND_NAME_CASE_SENSITIVE: {
                        if (a.GetKind() == b.GetKind()) {
                            if  (sort == SORT_KIND_NAME)
                                return a.GetName().CmpNoCase(b.GetName()) < 0;   
                            else
                                return a.GetName().Cmp(b.GetName()) < 0;                            
                        }
                        else {
                            const auto& iKindA = std::find(SymbolKindOrder.begin(), SymbolKindOrder.end(), a.GetKind());
                            if (iKindA == SymbolKindOrder.end()) 
                                return false;
                            const auto& iKindB = std::find(SymbolKindOrder.begin(), SymbolKindOrder.end(), b.GetKind());
                            if (iKindB == SymbolKindOrder.end()) 
                                return true;
                            return iKindA < iKindB;
                        }
                    }
                    case SORT_LINE: {
                        int lineA = a.GetRange().GetStart().GetLine();
                        int lineB = b.GetRange().GetStart().GetLine();
                        if (lineA == lineB) {
                            return a.GetRange().GetStart().GetCharacter() < b.GetRange().GetStart().GetCharacter();
                        }
                        else {
                            return lineA < lineB;
                        }
                    }
                }
                return false;
            }
        );
        
        for(auto& symbol : symbols) {
            Sort(symbol.GetChildren(), sort);
        }
    }
    
    void LSPSymbolParser::Sort(std::vector<LSP::SymbolInformation>& symbols, SortType sort, const std::vector<eSymbolKind>& symbolKindOrder) 
    {
        std::sort(symbols.begin(), symbols.end(), [symbolKindOrder, sort](const SymbolInformation& a, const SymbolInformation& b) 
            {
                switch(sort) {
                    case SORT_NAME: 
                         return a.GetName().CmpNoCase(b.GetName()) < 0; 
                    case SORT_NAME_CASE_SENSITIVE: 
                        return a.GetName().Cmp(b.GetName()) < 0;   
                    case SORT_KIND_NAME: wxFALLTHROUGH;
                    case SORT_KIND_NAME_CASE_SENSITIVE: {
                        if (a.GetKind() == b.GetKind()) {
                            if  (sort == SORT_KIND_NAME)
                                return a.GetName().CmpNoCase(b.GetName()) < 0;   
                            else
                                return a.GetName().Cmp(b.GetName()) < 0;                            
                        }
                        else {
                            const auto& iKindA = std::find(SymbolKindOrder.begin(), SymbolKindOrder.end(), a.GetKind());
                            if (iKindA == SymbolKindOrder.end()) 
                                return false;
                            const auto& iKindB = std::find(SymbolKindOrder.begin(), SymbolKindOrder.end(), b.GetKind());
                            if (iKindB == SymbolKindOrder.end()) 
                                return true;
                            return iKindA < iKindB;
                        }
                    }
                    case SORT_LINE: {
                        int lineA = a.GetLocation().GetRange().GetStart().GetLine();
                        int lineB = b.GetLocation().GetRange().GetStart().GetLine();
                        if (lineA == lineB) {
                            return a.GetLocation().GetRange().GetStart().GetCharacter() < b.GetLocation().GetRange().GetStart().GetCharacter();
                        }
                        else {
                            return lineA < lineB;
                        }
                    }
                }
                return false;
            }
        );
    }
    
    void LSPSymbolParser::Sort(LSP::DocumentSymbol& symbol, SortType sort, const std::vector<eSymbolKind>& symbolKindOrder) 
    {
        Sort(symbol.GetChildren(), sort, symbolKindOrder);
    }
    
    LSP::DocumentSymbol LSPSymbolParser::MakeDocumentSymbol(const SymbolInformation& si) {
            DocumentSymbol ds;
            ds.SetName(si.GetName());
            ds.SetRange(si.GetLocation().GetRange());
            ds.SetKind(si.GetKind());
            return ds;
    }
    
    long LSPSymbolParser::FindParent(std::vector<SymbolInformation>::size_type symbolIndex, const std::vector<SymbolInformation>& symbols) {
        const auto& si = symbols[symbolIndex];
        using index_t = std::vector<SymbolInformation>::size_type;
        
        // is first before second?
        // 0: equal; -1: first before second; 1: first after second
        auto comparePosition = [](const LSP::Position& first, const LSP::Position& second) -> int {
            int l1 = first.GetLine();
            int l2 = second.GetLine();
            if (l1 == l2) {
                int c1 = first.GetCharacter();
                int c2 = second.GetCharacter();
                if (c1 == c2)
                    return 0;
                else if (c1 < c2)
                    return -1;
                else 
                    return 1;
            }
            else if (l1 < l2) {
                return -1;
            }
            else {
                return 1;
            }
        };
        
        // does first include second?
        auto isRangeIncluded = [&](const LSP::Range& first, const LSP::Range& second) -> bool {
            int cmpStart = comparePosition(first.GetStart(), second.GetStart());
            int cmpEnd = comparePosition(first.GetEnd(), second.GetEnd());
            if (cmpStart == 0) {
                // start position is equal. only if first ends after second, it can include second
                return cmpEnd >= 0;
            }
            else if (cmpStart == 1) {
                // first starts after second
                return false;
            }
            else {
                // first starts before second - does it end after second?
                return cmpEnd >= 0;
            }
            return false;
        };
        
        const SymbolInformation* parent = nullptr;
        long parentIndex = -1;
        for (index_t i = 0; i < symbols.size(); i++) {
            if (i == symbolIndex)
                continue; // symbol cannot be it's own parent
            
            const auto& candidate = symbols[i];
            const auto& candidateRange = candidate.GetLocation().GetRange();
            if (isRangeIncluded(candidateRange, si.GetLocation().GetRange())) {
                // check if this is the smallest possible parent
                if (!parent || (parent && isRangeIncluded(parent->GetLocation().GetRange(), candidateRange))) {
                    // candidate has smaller range than current parent, so replace the current parent
                    parent = &candidate;
                    parentIndex = i;
                }
            }
        }        
        return parentIndex;
    };
    
    std::vector<LSP::DocumentSymbol> LSPSymbolParser::MakeTree(const std::vector<LSP::SymbolInformation>& symbols)
    {
        using SymbolList = std::vector<LSP::SymbolInformation>;
        using index_t = SymbolList::size_type;
               
        std::map<index_t, long> parents;
        std::vector<index_t> root_nodes;
        std::vector<DocumentSymbol> result;
        
        index_t num_symbols = symbols.size();
		for(SymbolList::size_type i = 0; i < num_symbols; ++i) {
			const SymbolInformation& symbol = symbols[i];        
            // find a parent for this symbol and store it's index
            long parent = FindParent(i, symbols);
            if (parent >= 0) 
                parents[i] = parent;    
            else
                root_nodes.push_back(i);
        }
                
        std::function<void(DocumentSymbol&, index_t)> AddChildren = [&](DocumentSymbol& symbol, index_t index) {
            for (const auto& rel : parents) {
                if (rel.second == index) {
                    DocumentSymbol child = MakeDocumentSymbol(symbols[rel.first]);
                    // find all nodes belonging to this child
                    AddChildren(child, rel.first);
                    symbol.GetChildren().push_back(child);
                }
            }
        };        
        
        // build the tree
        for (const auto& rootIndex : root_nodes) {
            const SymbolInformation& si = symbols[rootIndex];
            DocumentSymbol ds = MakeDocumentSymbol(si);
            AddChildren(ds, rootIndex);
            result.insert(result.end(), ds);
        }
        return result;
    }
}