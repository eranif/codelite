#ifndef LSP_SYMBOLSORTER_H
#define LSP_SYMBOLSORTER_H

#include "LSP/basic_types.h"


namespace LSP
{
class WXDLLIMPEXP_SDK LSPSymbolSorter {
public:
    enum SortType {
       SORT_NAME,
       SORT_NAME_CASE_SENSITIVE,
       SORT_KIND_NAME,
       SORT_KIND_NAME_CASE_SENSITIVE,
       SORT_LINE
    };
   
    static void Sort(std::vector<LSP::DocumentSymbol>& symbols, SortType sort);
    static void Sort(LSP::DocumentSymbol& symbol, SortType sort);
};

} // namespace LSP
#endif // LSP_SYMBOLSORTER_H