#pragma once

#include "GenericFormatter.hpp"

class fmtCMakeFormat : public GenericFormatter
{
public:
    fmtCMakeFormat();
    ~fmtCMakeFormat() override = default;
};
