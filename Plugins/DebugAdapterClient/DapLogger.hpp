#pragma once

#include "clModuleLogger.hpp"

INITIALISE_MODULE_LOG(DAP_LOG, "DAP", "dap.log")
#define DAP_DEBUG() DAP_LOG().SetCurrentLogLevel(FileLogger::Dbg) << DAP_LOG().Prefix()
#define DAP_SYSTEM() DAP_LOG().SetCurrentLogLevel(FileLogger::System) << DAP_LOG().Prefix()
#define DAP_ERROR() DAP_LOG().SetCurrentLogLevel(FileLogger::Error) << DAP_LOG().Prefix()
#define DAP_WARNING() DAP_LOG().SetCurrentLogLevel(FileLogger::Warning) << DAP_LOG().Prefix()
#define DAP_INFO() DAP_LOG().SetCurrentLogLevel(FileLogger::Info) << DAP_LOG().Prefix()
