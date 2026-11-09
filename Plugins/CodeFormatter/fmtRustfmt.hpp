#pragma once

#include "GenericFormatter.hpp"

class fmtRustfmt : public GenericFormatter
{
public:
    fmtRustfmt();
    ~fmtRustfmt() override = default;
};
