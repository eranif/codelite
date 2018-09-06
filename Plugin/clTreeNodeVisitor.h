#ifndef CLTREENODEVISITOR_H
#define CLTREENODEVISITOR_H

#include "clTreeCtrlModel.h"
#include <functional>
#include <vector>

class WXDLLIMPEXP_SDK clTreeNodeVisitor
{
public:
    clTreeNodeVisitor();
    virtual ~clTreeNodeVisitor();

    /**
     * @brief Visit the tree in order
     * @param item item to start from
     * @param expandedItemsOnly set to false to visit all items, regardless of their state
     * @param VisitFunc a callback to be called per item. The CB signature is (clTreeCtrlNode* child)
     * the callback returns true to continue, false to stop
     */
    void Visit(clTreeCtrlNode* item, bool expandedItemsOnly, std::function<bool(clTreeCtrlNode*, bool)>& func);
};
#endif // CLTREENODEVISITOR_H
