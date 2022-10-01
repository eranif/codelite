#ifndef PLATFORM__HPP
#define PLATFORM__HPP

#include "codelite_exports.h"

#ifdef __WXMSW__
#include "MSYS2.hpp"
typedef MSYS2 PLATFORM;
#else
#include "LINUX.hpp"
typedef LINUX PLATFORM;
#endif
#endif // PLATFORM__HPP
