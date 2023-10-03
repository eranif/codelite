import sys
import os
import subprocess
import shutil


def run_command_and_return_output(
    command, throw_err=False, working_directory=None
):
    """Execute command and return its output as a string. In case of an error, return an empty string"""
    try:
        return subprocess.check_output(
            args=command,
            cwd=working_directory,
            shell=True,
            stderr=subprocess.STDOUT,
        ).decode("utf-8")
    except Exception as e:
        if throw_err:
            raise e
        else:
            return ""


brew_install_prefix = (
    run_command_and_return_output(
        "brew --prefix --installed openssl pcre2 libssh hunspell",
        throw_err=True,
    )
    .strip()
    .split("\n")
)


def run_install_name_tool(file: str):
    file_base_name = os.path.basename(file)
    patterns = [
        "libwx_",
        "libdapcxx",
        "libcodelite",
        "libwxsqlite",
        "libplugin",
        "libwxshapeframework",
        "libdatabaselayersqlite",
    ]
    patterns = patterns + brew_install_prefix

    grep_E = "|".join(patterns)
    otool_output = run_command_and_return_output(
        f'otool -L {file} |grep -E "{grep_E}" | grep -v -w "{file_base_name}" |grep -v "@executable_path"'
    )
    lines = otool_output.split("\n")
    for line in lines:
        line = line.strip()
        if len(line) == 0:
            continue

        dep_full_path = line.strip().split(" ")[0].strip()
        dep_full_name = os.path.basename(dep_full_path)
        run_command_and_return_output(
            f"install_name_tool -change {dep_full_path} @executable_path/{dep_full_name} {file}"
        )


def install_brew_dyblis(bundle_path):
    brew_libs = [("openssl", "crypto.3")]
    for pkg_name, libname in brew_libs:
        libdir = run_command_and_return_output(
            f"brew --prefix --installed {pkg_name}"
        ).strip()
        shutil.copyfile(
            f"{libdir}/lib/lib{libname}.dylib",
            f"{bundle_path}/lib{libname}.dylib",
        )


if len(sys.argv) != 2:
    print("expected param: build directory")
    sys.exit(1)

bundle_dir = sys.argv[1]
if not os.path.exists(bundle_dir):
    print("expected param: build directory")
    sys.exit(1)

# prepare the list of files to work on
files = run_command_and_return_output(
    f"cat {bundle_dir}/../install_manifest.txt|grep '.dylib'"
)

files_set = set(files.strip().split("\n"))
extra_files = run_command_and_return_output(
    f"find {bundle_dir}/Contents/MacOS/ -maxdepth 1 ! -type l -name '*.dylib' |xargs readlink -f"
)

extra_files_set = set(extra_files.strip().split("\n"))

for file in extra_files_set:
    files_set.add(file)

# add executables
files_set.add(f"{bundle_dir}/Contents/MacOS/codelite")
files_set.add(f"{bundle_dir}/Contents/MacOS/codelite-make")
files_set.add(f"{bundle_dir}/Contents/MacOS/ctagsd")

# install extra libraries
install_brew_dyblis(f"{bundle_dir}/Contents/MacOS/")

# for each file, run install name tool
for file in files_set:
    run_install_name_tool(file)
