@ECHO OFF

:: get the revision and set it into variable
call git rev-parse --short HEAD  > tmp.text
set /p GIT_VERSION= < tmp.text 
del tmp.text 

echo #ifndef CL_GIT_REVISION > autoversion.cpp
echo #define CL_GIT_REVISION >> autoversion.cpp
echo. >> autoversion.cpp
echo const char* clGitRevision = ^"%GIT_VERSION%^"; >> autoversion.cpp
echo #endif >> autoversion.cpp

