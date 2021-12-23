#include "clTreeNodeVisitor.h"

clTreeNodeVisitor::clTreeNodeVisitor() {}

clTreeNodeVisitor::~clTreeNodeVisitor() {}

void clTreeNodeVisitor::Visit(clRowEntry* item, bool expandedItemsOnly, std::function<bool(clRowEntry*, bool)>& func)
{
    clRowEntry* current = item;
    while(current) {
        bool isVisible = current->IsVisible();
        if(expandedItemsOnly && isVisible) {
            if(!func(current, isVisible)) {
                return;
            }
        } else if(!expandedItemsOnly) {
            if(!func(current, isVisible)) {
                return;
            }
        }
        current = current->GetNext();
    }
}
