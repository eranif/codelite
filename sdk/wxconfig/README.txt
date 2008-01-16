//--------------------------------------------------------------------------------------------
// Name:        wx-config-win
// Purpose:     A wx-config implementation for MinGW/DMars/VC
// Author:      Takeshi Miya <takeshimiya at gmail dot com>
// Created:     2006-03-23
// Copyright:   (c) Takeshi Miya
// Licence:     wxWidgets licence
// $URL: http://wx-config-win.googlecode.com/svn/trunk/README.txt $
//--------------------------------------------------------------------------------------------

wx-config-win is a tiny implementation of wx-config for Windows compilers.

wx-config is a helper tool used when compiling applications that makes use of the wxWidgets library. 
It helps you insert the correct compiler options on the command line so an application can use 
g++ -o test.exe test.cpp `wx-config --libs --cflags` for instance, rather than hard-coding 
values on where to find wxWidgets and it's configurations (monolithic, debug, unicode, wxuniversal, etc). 

Note that backticks is a property of the shell or the build system used.
Fortunately the Code::Blocks IDE supports backticks natively on windows. 
For other Windows IDEs and build systems there are workarounds, commonly using "response files".


Usage: wx-config [options]
Options:
  --prefix[=DIR]              Path of the wxWidgets installation (ie. C:\wxWidgets2.6.3)
  --wxcfg[=DIR]               Relative path of the build.cfg file (ie. gcc_dll\mswud)
  --cflags                    Outputs all pre-processor and compiler flags.
  --cxxflags                  Same as --cflags but for C++.
  --rcflags                   Outputs all resource compiler flags. [UNTESTED]
  --libs                      Outputs all linker flags.
  
  --debug[=yes|no]            Uses a debug configuration if found.
  --unicode[=yes|no]          Uses an unicode configuration if found.
  --static[=yes|no]           Uses a static configuration if found.
  --universal[=yes|no]        Uses an universal configuration if found.
  --compiler[=gcc,dmc,vc]     Selects the compiler.
  --release                   Outputs the wxWidgets release number.
  --cc                        Outputs the name of the C compiler.
  --cxx                       Outputs the name of the C++ compiler.
  --ld                        Outputs the linker command.
  -v                          Outputs the revision of wx-config.

  Note that using --prefix is not needed if you have defined the
  environmental variable WXWIN.

  Also note that using --wxcfg is not needed if you have defined the
  environmental variable WXCFG.

//--------------------------------------------------------------------------------------------

The currently supported compilers are:
* GNU MinGW [gcc]
* MS Visual C++ [vc]
* Digital Mars [dmc]

Advantages of using wx-config-win:
* Same commands for build in linux, mac, and windows.
* Doesn't need cygwin (there is a wx-config for cygwin already).
* The projects of the IDE will be exactly the same in all systems for most cases.
* Separates the user library configurations entirely from the project.
* The makefiles exported by the IDE (if any) will be a lot smaller and easier to handle.
* Can be used from the console (redirecting the output) or from an IDE that supports backticks (like Code::Blocks).
* Source code is 1 single cpp so you can include and compile it as a dependency easily in your project if you want.
* No dependencies, only ISO/C++ standard code.
* Feel free to hack it :)

Disvantages of using wx-config-win: 
* Only works on something (IDE, build system, console, etc) that supports backticks or redirecting the output 
  (though there are workarounds for IDEs that doesn't, like Visual Studio IDE).
* The whole "*-config" scripts are dying on linux (ie. sdl-config, wx-config, gtk-config, ...).
  There is a strong standard on *nix now called pkg-config, developed by FreeDesktop, and is being used 
  in a lot of packages, like gtk, glib, sqlite, etc (and there is a pkg-config.exe for windows too).
  So this solution is a temporal relief (I hope) so that when in future versions of the library uses the pkg-config 
  standard, there wouldn't be any need for this.
  Here's the FR: http://sourceforge.net/tracker/index.php?func=detail&aid=1434365&group_id=9863&atid=359863

//--------------------------------------------------------------------------------------------
