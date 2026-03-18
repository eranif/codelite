# AGENTS

## Mission
- CodeLite is a free, open-source, cross-platform IDE for C, C++, Rust, Python, Node.js, and PHP. Your job is to shepherd contributions related to this project: understand how CodeLite is structured, make focused edits, validate them locally when possible, and document the results.
- Always treat the user as a developer collaborator. Ask clarifying questions whenever requirements are ambiguous and summarize your proposed approach before making changes.

## Repository Overview
- `CodeLite/`, `LiteEditor/`, `Runtime/`, `Plugin/`, and sister directories host the core IDE, editors, runtime libraries, and plug-in system.
- `cmake/`, `scripts/`, and the `.build-release*` directories contain build helpers, toolchains, and configuration templates. The repository is driven by CMake (see `CMakeLists.txt`).
- Subprojects such as `LanguageServer/`, `DatabaseExplorer/`, `PHPRefactoring/`, and `SmartCompletion/` are feature-specific extensions built on top of the core.
- `docs/`, `README.md`, and `TODO.md` describe coding standards, build steps, and outstanding work. Consult them when planning changes.

## Build & Tooling Notes
- CodeLite uses CMake. A canonical Windows build sequence is: `cmake -S . -B .build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release`, followed by `cmake --build .build-release --config Release`.
- On other platforms - Linux and macOS - A build sequence is `cmake -S . -B .build-release -DCMAKE_BUILD_TYPE=Release`, , followed by `cmake --build .build-release --config Release`.
- After building, `ctest  --output-on-failure --test-dir .build-release` exercises the bundled test suite wherever supported.
- Keep `compile_commands.json` in sync by regenerating the build directory before running linters or language tooling.
- After generating a code, use `clang-format -i <source-file>` to keep it formatted properly.

## Development Guidance for Agents
1. **Inspect:** Start with `git status` and `git diff` to understand the working tree. Read the relevant files to understand context before modifying.
2. **Edit:** Favor existing style (C++20, wxWidgets, consistent formatting). If unsure, check `.clang-format`, or existing code for guidance.
3. **Build/Test:** Run the minimal reproducible build/test commands for your changes, especially if touching platform-specific code. If a full build is impractical, explain why and list the commands you would run.
4. **Document:** Update or reference `README.md`, `docs/`, or relevant user-facing files when behavior changes.
5. **Communication:** When presenting results, include:
   - Summary of what you changed (and why).
   - Tests you ran (or reasons for not running them).
   - Next steps or unresolved questions.

## Safety & Best Practices
- Prefer minimal changes. Reuse existing abstractions unless a broader refactor is justified.
- Avoid running unknown scripts. Stick to documented commands unless the user explicitly authorizes experimentation.
- Be mindful of cross-platform differences; test or note expected behavior per OS.

## Helpful Resources
- [README.md](README.md)
- [docs/](docs/)
- [TODO.md](TODO.md)
- `CMakeLists.txt` for build flow and component inclusion

Agents reading this file should convert it to their system prompt, honor its guidance, and keep it in sync with the repository's state.
