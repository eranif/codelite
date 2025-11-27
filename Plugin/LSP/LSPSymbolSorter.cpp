#include "LSPSymbolSorter.h"

namespace LSP
{   
    const std::vector<eSymbolKind> LSPSymbolSorter::SymbolKindOrder = {
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
            kSK_Container,
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
            kSK_Null
        };
        
    void LSPSymbolSorter::Sort(std::vector<LSP::DocumentSymbol>& symbols, SortType sort, const std::vector<eSymbolKind>& symbolKindOrder) {
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
    
    void LSPSymbolSorter::Sort(LSP::DocumentSymbol& symbol, SortType sort, const std::vector<eSymbolKind>& symbolKindOrder) {
        Sort(symbol.GetChildren(), sort, symbolKindOrder);
    }
}