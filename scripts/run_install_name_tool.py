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


# brew_install_prefix = (
#    run_command_and_return_output(
#        "brew --prefix --installed openssl pcre2 libssh hunspell",
#        throw_err=True,
#    )
#    .strip()
#    .split("\n")
# )


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
    #    patterns = patterns + brew_install_prefix

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


if len(sys.argv) != 2:
    print("expected param: build directory")
    sys.exit(1)


bundle_dir = sys.argv[1]
if not os.path.exists(bundle_dir):
    print("expected param: build directory")
    sys.exit(1)

# Copy wxWidgets libraries that the binary actually depends on
codelite_binary = f"{bundle_dir}/Contents/MacOS/codelite"
if os.path.exists(codelite_binary):
    print(f"Analyzing dependencies of {codelite_binary}")
    otool_output = run_command_and_return_output(f"otool -L {codelite_binary}")
    lines = otool_output.split("\n")
    for line in lines:
        line = line.strip()
        if "libwx_" in line and "@executable_path" not in line:
            dep_path = line.split(" ")[0].strip()
            lib_name = os.path.basename(dep_path)

            # Try to find the library in common locations
            actual_path = None
            if os.path.exists(dep_path):
                actual_path = dep_path
            else:
                # For @rpath, try to find in wx-config lib dir and common locations
                search_dirs = ["/usr/local/lib", "/usr/lib"]
                # Also try to get from wx-config
                wx_lib_output = run_command_and_return_output(
                    "wx-config --prefix 2>/dev/null"
                )
                if wx_lib_output:
                    search_dirs.insert(0, f"{wx_lib_output.strip()}/lib")

                for search_dir in search_dirs:
                    candidate = os.path.join(search_dir, lib_name)
                    if os.path.exists(candidate):
                        actual_path = candidate
                        break

            if actual_path:
                dest_file = f"{bundle_dir}/Contents/MacOS/{lib_name}"
                if not os.path.exists(dest_file):
                    print(f"Copying wxWidgets library: {lib_name}")
                    shutil.copy2(actual_path, dest_file)
                else:
                    print(f"wxWidgets library already exists: {lib_name}")
            else:
                print(
                    f"Warning: Could not find wxWidgets library {lib_name} (referenced as {dep_path})"
                )
else:
    print(f"Warning: codelite binary not found at {codelite_binary}")

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

# for each file, run install name tool
for file in files_set:
    run_install_name_tool(file)
