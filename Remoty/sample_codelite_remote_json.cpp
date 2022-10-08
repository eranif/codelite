#ifndef SAMPLE_CODELITE_REMOTE_JSON_HPP
#define SAMPLE_CODELITE_REMOTE_JSON_HPP

const wxString DEFAULT_CODELITE_REMOTE_JSON = R"EOF(
{
  "Language Server Plugin": {
    "servers": [
      {
        "command": "/usr/bin/clangd -limit-results=500 -header-insertion-decorators=1",
        "env": [],
        "languages": [
          "c",
          "cpp"
        ],
        "name": "clangd",
        "priority": 90,
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "rust-analyzer",
        "env": [],
        "languages": [
          "rust"
        ],
        "name": "rust-analyzer",
        "priority": 90,
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
        "languages": [
          "python"
        ],
        "name": "python",
        "priority": 90,
        "working_directory": "$(WorkspacePath)"
      }
    ]
  },
  "Source Code Formatter": {
    "tools": [
      {
        "command": "jq . -S $(CurrentFileRelPath)",
        "inplace_edit": false,
        "languages": [
          "Json"
        ],
        "name": "jq",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "clang-format $(CurrentFileRelPath)",
        "inplace_edit": false,
        "languages": [
          "C/C++",
          "Java",
          "Javascript/Typescript"
        ],
        "name": "clang-format",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "xmllint --format $(CurrentFileRelPath)",
        "inplace_edit": false,
        "languages": [
          "Xml"
        ],
        "name": "xmllint",
        "working_directory": "$(WorkspacePath)"
      },
      {
        "command": "rustfmt --edition 2021 $(CurrentFileRelPath)",
        "inplace_edit": true,
        "languages": [
          "Rust"
        ],
        "name": "rustfmt",
        "working_directory": "$(WorkspacePath)"
      }
    ]
  }
}
)EOF";

#endif // SAMPLE_CODELITE_REMOTE_JSON_HPP
