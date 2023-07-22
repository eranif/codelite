import subprocess
import os
import sys
import pathlib


class InstallNameTool:
    def __init__(self):
        self.depth = 0
        self.visited = set()
        self.deps = []
        self.files = set()

    def _run_command_and_return_output(self, command, working_directory=None):
        """Execute command and return its output as a string. In case of an error, return an empty string"""
        try:
            return subprocess.check_output(
                args=command,
                cwd=working_directory,
                shell=True,
                stderr=subprocess.STDOUT,
            ).decode("utf-8")
        except Exception as e:
            return ""

    def build_list_of_files(self):
        plugins_dir = f"{sys.argv[1]}/Contents/SharedSupport/plugins"
        macos_dir = f"{sys.argv[1]}/Contents/MacOS"
        macos_dir_content = self._run_command_and_return_output(
            f"find {macos_dir} -maxdepth 1 -not -type d -not -type l |grep -v '.sh'"
        )
        plugins_dir_content = self._run_command_and_return_output(
            f"find {plugins_dir} -maxdepth 1 -not -type d -not -type l -name '*.dylib'"
        )
        all_files = macos_dir_content + plugins_dir_content
        files_list = all_files.split("\n")
        for file in files_list:
            file = file.strip()
            if len(file) == 0:
                continue
            self.files.add(file)

    def run_install_name_tool(self, file: str):
        file_base_name = os.path.basename(file)
        grep_E = "|".join(
            [
                "libwx_",
                "libdapcxx",
                "libcodelite",
                "libwxsqlite",
                "libplugin",
                "libwxshapeframework",
                "libdatabaselayersqlite",
                "libpcre",
                "libssh",
                "libhunspell",
            ]
        )
        otool_output = self._run_command_and_return_output(
            f'otool -L {file} |grep -E "f{grep_E}" | grep -v -w "{file_base_name}"'
        )
        lines = otool_output.split("\n")
        for line in lines:
            line = line.strip()
            if len(line) == 0:
                continue

            dep_full_path = line.strip().split(" ")[0].strip()
            dep_full_name = os.path.basename(dep_full_path)
            self._run_command_and_return_output(
                f"install_name_tool -change {dep_full_path} @executable_path/{dep_full_name} {file}"
            )

    def fix_files(self):
        for file in self.files:
            self.run_install_name_tool(file)

    def list_deps(self):
        pass


def main():
    tool = InstallNameTool()
    tool.build_list_of_files()
    tool.fix_files()


if __name__ == "__main__":
    main()
