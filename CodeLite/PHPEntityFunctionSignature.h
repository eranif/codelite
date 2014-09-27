#ifndef PHPENTITYFUNCTIONSIGNATURE_H
#define PHPENTITYFUNCTIONSIGNATURE_H

#include "PHPEntityBase.h" // Base class: PHPEntityBase
#include "codelite_exports.h"

class WXDLLIMPEXP_CL PHPEntityFunctionSignature : public PHPEntityBase
{
public:
    virtual void PrintStdout(int indent) const;
    PHPEntityFunctionSignature();
    virtual ~PHPEntityFunctionSignature();

public:
    virtual wxString ID() const;
};

#endif // PHPENTITYFUNCTIONSIGNATURE_H
