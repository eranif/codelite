#ifndef SAMPLE_CODELITE_REMOTE_JSON_HPP
#define SAMPLE_CODELITE_REMOTE_JSON_HPP

const wxString DEFAULT_CODELITE_REMOTE_JSON = R"EOF(
{
  "Language Server Plugin": {
    "servers": [
      {
        "command": "/usr/bin/clangd -limit-results=500 -header-insertion-decorators=1",
        "env": [],
        "name": "clangd",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "rust-analyzer",
        "env": [],
        "name": "rust-analyzer",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "python3 -m pylsp",
        "env": [
          {
            "name": "PYTHONPATH",
            "value": ".:$PYTHONPATH"
          }
        ],
        "name": "python",
        "working_directory": "$(WorkspacePath)"
      }
    ]
  },
  "Source Code Formatter": {
    "tools": [
      {
        "command": "jq . -S $(CurrentFileRelPath)",
        "name": "jq",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "clang-format $(CurrentFileRelPath)",
        "name": "clang-format",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "xmllint --format $(CurrentFileRelPath)",
        "name": "xmllint",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "rustfmt --edition 2021 $(CurrentFileRelPath)",
        "name": "rustfmt",
        "working_directory": "$(WorkspacePath)"
      }
    ]
  }
}
)EOF";

#endif // SAMPLE_CODELITE_REMOTE_JSON_HPP
