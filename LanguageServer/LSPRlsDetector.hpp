#ifndef LSPRLSDETECTOR_HPP
#define LSPRLSDETECTOR_HPP

#include "LSPDetector.hpp"

/// Rust LSP
class LSPRlsDetector : public LSPDetector
{
protected:
    bool DoLocate() override;

public:
    LSPRlsDetector();
    ~LSPRlsDetector();
};

#endif // LSPRLSDETECTOR_HPP
