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
    if(!parent->GetChildren().empty()) {
        PHPEntityBase::List_t::const_iterator iter = parent->GetChildren().begin();
        for(; iter != parent->GetChildren().end(); ++iter) {
            Visit(*iter);
        }
    }
}
