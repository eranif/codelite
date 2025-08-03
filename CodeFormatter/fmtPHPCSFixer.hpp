#ifndef PHPCSFIXER_HPP
#define PHPCSFIXER_HPP

#include "GenericFormatter.hpp"

class fmtPHPCSFixer : public GenericFormatter
{
public:
    fmtPHPCSFixer();
    ~fmtPHPCSFixer() override = default;
};

#endif // PHPCSFIXER_HPP
