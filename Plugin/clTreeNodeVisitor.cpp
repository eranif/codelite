#include "clTreeNodeVisitor.h"

clTreeNodeVisitor::clTreeNodeVisitor() {}

clTreeNodeVisitor::~clTreeNodeVisitor() {}

void clTreeNodeVisitor::Visit(
    clTreeCtrlNode* item, bool expandedItemsOnly, std::function<bool(clTreeCtrlNode*, bool)>& func)
{
    clTreeCtrlNode* current = item;
    while(current) {
        bool isVisible = current->IsVisible();
        if(expandedItemsOnly && isVisible) {
            if(!func(current, isVisible)) { return; }
        } else if(!expandedItemsOnly) {
            if(!func(current, isVisible)) { return; }
        }
        current = current->GetNext();
    }
}
