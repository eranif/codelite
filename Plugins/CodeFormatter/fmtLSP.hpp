#ifndef FMTLSP_HPP
#define FMTLSP_HPP

#include "GenericFormatter.hpp"

/// Format with the language server of the file. It has no command: the CodeFormatter plugin sends
/// `textDocument/formatting` to the server and applies the reply to the editor
class fmtLSP : public GenericFormatter
{
public:
    fmtLSP();
    ~fmtLSP() override = default;
};

#endif // FMTLSP_HPP
