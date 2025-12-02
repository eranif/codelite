#ifndef LSP_SYMBOLSORTER_H
#define LSP_SYMBOLSORTER_H

#include "LSP/basic_types.h"


namespace LSP
{
class WXDLLIMPEXP_SDK LSPSymbolParser {
public:
    enum SortType {
       SORT_NAME,
       SORT_NAME_CASE_SENSITIVE,
       SORT_KIND_NAME,
       SORT_KIND_NAME_CASE_SENSITIVE,
       SORT_LINE
    };
    static const std::vector<eSymbolKind> SymbolKindOrder;    
    /**
     * @brief Sort a list of DocumentSymbols recursively 
     * @param symbols
     * @param sort
     * @param symbolKindOrder
     */
     
    static void Sort(std::vector<LSP::DocumentSymbol>& symbols, SortType sort, const std::vector<eSymbolKind>& symbolKindOrder = SymbolKindOrder);
    /**
     * @brief Sort a single DocumentSymbol recursively
     * @param symbol
     * @param sort
     * @param symbolKindOrder
     */
    static void Sort(LSP::DocumentSymbol& symbol, SortType sort, const std::vector<eSymbolKind>& symbolKindOrder = SymbolKindOrder); 
   
    /**
     * @brief Sort a list of SymbolInformation without respect to container
     * @param symbols
     * @param sort
     * @param symbolKindOrder
     */
    static void Sort(std::vector<LSP::SymbolInformation>& symbols, SortType sort, const std::vector<eSymbolKind>& symbolKindOrder = SymbolKindOrder);
    
    static std::vector<LSP::DocumentSymbol> Parse(const std::vector<LSP::SymbolInformation>& symbols);
    
    private:
        static inline DocumentSymbol MakeDocumentSymbol(const SymbolInformation& si);
        static long FindParent(std::vector<SymbolInformation>::size_type symbolIndex, const std::vector<SymbolInformation>& symbols);
};

} // namespace LSP
#endif // LSP_SYMBOLSORTER_H