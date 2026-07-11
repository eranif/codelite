#ifndef DAPLOGGINGHELPER_HPP
#define DAPLOGGINGHELPER_HPP

#include "clModuleLogger.hpp"
#include "dap/dap.hpp"

// extend clModuleLogger to handle dap:: objects
clModuleLogger& operator<<(clModuleLogger& logger, const dap::SourceBreakpoint& obj)
{
    if(!logger.CanLog()) {
        return logger;
    }

    wxString s;
    s << "  SourceBreakpoint {line:" << obj.line << "}";
    logger.Append(s);
    return logger;
}

clModuleLogger& operator<<(clModuleLogger& logger, const std::vector<dap::SourceBreakpoint>& obj)
{
    if(!logger.CanLog()) {
        return logger;
    }

    for(const auto& bp : obj) {
        logger << bp << endl;
    }
    return logger;
}

clModuleLogger& operator<<(clModuleLogger& logger, const dap::FunctionBreakpoint& obj)
{
    if(!logger.CanLog()) {
        return logger;
    }

    wxString s;
    s << "  FunctionBreakpoint {line:" << obj.name << "}";
    logger.Append(s);
    return logger;
}

clModuleLogger& operator<<(clModuleLogger& logger, const std::vector<dap::FunctionBreakpoint>& obj)
{
    if(!logger.CanLog()) {
        return logger;
    }

    for(const auto& bp : obj) {
        logger << bp << endl;
    }
    return logger;
}

#endif // DAPLOGGINGHELPER_HPP
