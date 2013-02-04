@ECHO OFF

:: get the revision and set it into variable
call git describe --tags  > tmp.text
set /p GIT_VERSION= < tmp.text 
del tmp.text 

echo #ifndef CL_GIT_REVISION > autoversion.cpp
echo #define CL_GIT_REVISION >> autoversion.cpp
echo. >> autoversion.cpp
echo const wxChar* clGitRevision = wxT(^"%GIT_VERSION%^"); >> autoversion.cpp
echo #endif >> autoversion.cpp

