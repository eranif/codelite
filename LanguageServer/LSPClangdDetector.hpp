#ifndef LSPCLANGDDETECTOR_HPP
#define LSPCLANGDDETECTOR_HPP

#include "LSPDetector.hpp"

class LSPClangdDetector : public LSPDetector
{
protected:
    virtual bool DoLocate();

public:
    LSPClangdDetector();
    virtual ~LSPClangdDetector();
};

#endif // LSPCLANGDDETECTOR_HPP
