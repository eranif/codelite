#ifndef PHPENTITYVISITOR_H
#define PHPENTITYVISITOR_H

#include "codelite_exports.h"
#include "PHPEntityBase.h"

/**
 * @class PHPEntityVisitor
 * Accept as input a PHPEntitBase and visit all of its children
 */
class WXDLLIMPEXP_CL PHPEntityVisitor
{
public:
    PHPEntityVisitor();
    virtual ~PHPEntityVisitor();
    
    void Visit(PHPEntityBase::Ptr_t parent);
    
    /**
     * @brief called whenver an entity is visited
     * @param entity
     */
    virtual void OnEntity(PHPEntityBase::Ptr_t entity) = 0;
};

#endif // PHPENTITYVISITOR_H
