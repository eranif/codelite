#ifndef FMTRUSTFMT_HPP
#define FMTRUSTFMT_HPP

#include "GenericFormatter.hpp"

class fmtRustfmt : public GenericFormatter
{
public:
    fmtRustfmt();
    ~fmtRustfmt() override = default;
};

#endif // FMTRUSTFMT_HPP
