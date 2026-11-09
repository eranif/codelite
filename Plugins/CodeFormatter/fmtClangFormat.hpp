#pragma once

#include "GenericFormatter.hpp"

class fmtClangFormat : public GenericFormatter
{
public:
    fmtClangFormat();
    ~fmtClangFormat() override = default;
};
