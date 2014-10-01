#ifndef PHPDOCVISITOR_H
#define PHPDOCVISITOR_H

#include "codelite_exports.h"
#include "PHPEntityVisitor.h" // Base class: PHPEntityVisitor
#include "PhpLexerAPI.h"
#include <vector>
#include "PHPSourceFile.h"

class WXDLLIMPEXP_CL PHPDocVisitor : public PHPEntityVisitor
{
    std::map<int, phpLexerToken> m_comments;
    PHPSourceFile& m_sourceFile;
    
public:
    PHPDocVisitor(PHPSourceFile& sourceFile, const std::vector<phpLexerToken>& comments);
    virtual ~PHPDocVisitor();

public:
    virtual void OnEntity(PHPEntityBase::Ptr_t entity);
};

#endif // PHPDOCVISITOR_H
