#pragma once

#include "clDapSettingsStore.hpp"

#include <vector>

class DapLocator
{
private:
    void find_lldb_dap(std::vector<DapEntry>* entries);
    void find_debugpy(std::vector<DapEntry>* entries);
    void find_gdb(std::vector<DapEntry>* entries);

public:
    DapLocator() = default;
    ~DapLocator() = default;

    size_t Locate(std::vector<DapEntry>* entries);
};
