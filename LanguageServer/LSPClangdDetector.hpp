#ifndef LSPCLANGDDETECTOR_HPP
#define LSPCLANGDDETECTOR_HPP

#include "LSPDetector.hpp"

class LSPClangdDetector : public LSPDetector
{
public:
    LSPClangdDetector();
    virtual ~LSPClangdDetector();
    
    virtual bool Locate();
};

#endif // LSPCLANGDDETECTOR_HPP
