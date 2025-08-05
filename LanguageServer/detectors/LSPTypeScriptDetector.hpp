#ifndef LSPTYPESCRIPTDETECTOR_HPP
#define LSPTYPESCRIPTDETECTOR_HPP

#include "LSPDetector.hpp" // Base class: LSPDetector

class LSPTypeScriptDetector : public LSPDetector
{
public:
    LSPTypeScriptDetector();
    ~LSPTypeScriptDetector() override = default;

    bool DoLocate() override;
};

#endif // LSPTYPESCRIPTDETECTOR_HPP
