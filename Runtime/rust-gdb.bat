@echo off
REM  Find out where the pretty printer Python module is

SET GDB_PYTHON_MODULE_DIRECTORY=%SCRIPTS_PATH%

REM  Run GDB with the additional arguments that load the pretty printers
REM  Set the environment variable `RUST_GDB` to overwrite the call to a
REM  different/specific command (defaults to `gdb`).
if not defined RUST_GDB ( SET RUST_GDB=gdb )

if not defined PYTHONPATH ( 
    SET PYTHONPATH=%GDB_PYTHON_MODULE_DIRECTORY%
) else (
    SET PYTHONPATH=%PYTHONPATH%;%GDB_PYTHON_MODULE_DIRECTORY%
)

%RUST_GDB% --directory="%GDB_PYTHON_MODULE_DIRECTORY%" -iex "add-auto-load-safe-path %GDB_PYTHON_MODULE_DIRECTORY%" %*