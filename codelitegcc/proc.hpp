#ifndef __PROC_HPP
#define __PROC_HPP

#include <string>

void file_write_content(const std::string& logfile, const std::string& content);
int run_child_process(int argc, char** argv, const std::string& commandline);

#endif // __PROC_HPP
