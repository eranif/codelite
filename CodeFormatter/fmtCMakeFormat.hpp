#ifndef FMTCMAKEFORMAT_HPP
#define FMTCMAKEFORMAT_HPP

#include "GenericFormatter.hpp"

class fmtCMakeFormat : public GenericFormatter
{
public:
    fmtCMakeFormat();
    ~fmtCMakeFormat() override = default;
};

#endif // FMTCMAKEFORMAT_HPP
