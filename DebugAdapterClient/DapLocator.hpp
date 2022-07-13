#ifndef DAPLOCATOR_HPP
#define DAPLOCATOR_HPP

#include "clDapSettingsStore.hpp"

#include <vector>

class DapLocator
{
private:
    void find_lldb_vscode(std::vector<DapEntry>* entries);
    void find_debugpy(std::vector<DapEntry>* entries);

public:
    DapLocator();
    ~DapLocator();

    size_t Locate(std::vector<DapEntry>* entries);
};

#endif // DAPLOCATOR_HPP
