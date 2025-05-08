#include "PHPEntityVisitor.h"

PHPEntityVisitor::PHPEntityVisitor()
{
}

PHPEntityVisitor::~PHPEntityVisitor()
{
}

void PHPEntityVisitor::Visit(PHPEntityBase::Ptr_t parent)
{
    OnEntity(parent);
    if (!parent->GetChildren().empty()) {
        for (const auto& child : parent->GetChildren()) {
            Visit(child);
        }
    }
}
