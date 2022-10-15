#ifndef PLATFORM__HPP
#define PLATFORM__HPP

#include "codelite_exports.h"

#ifdef __WXMSW__
#include "MSYS2.hpp"
#define ThePlatform MSYS2::Get()
#else
#include "LINUX.hpp"
#define ThePlatform LINUX::Get()
#endif
#endif // PLATFORM__HPP
