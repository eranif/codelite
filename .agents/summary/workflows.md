# Workflows

## Key workflows
- Build and configure the IDE with CMake.
- Load editor, runtime, and plugin subsystems on startup.
- Resolve parser and completion services for source navigation.
- Connect debugger and version-control plugins through shared interfaces.
- Use bundled tooling and templates to generate or manage projects.

## Build workflow
```mermaid
sequenceDiagram
  participant Dev as Developer
  participant CMake as CMake
  participant Build as Build System
  participant App as CodeLite
  Dev->>CMake: Configure source tree
  CMake->>Build: Generate targets and compile commands
  Build->>App: Compile/link modules
  App-->>Dev: IDE binaries and plugins
```

## Runtime workflow
```mermaid
sequenceDiagram
  participant User as User
  participant IDE as CodeLite IDE
  participant Plugin as Plugin layer
  participant Parser as Parser services
  participant Ext as External libraries
  User->>IDE: Open workspace/project
  IDE->>Plugin: Load required extensions
  IDE->>Parser: Request symbols/analysis
  Parser->>Ext: Use bundled dependency when needed
  Parser-->>IDE: Analysis result
  IDE-->>User: Updated editor/navigation state
```

## Tooling workflows
- Templates and runtime support are used for generated project scaffolding.
- Platform-specific packaging is handled by separate helper directories.
- Language-specific services are delegated to their respective modules rather than centralized in the editor core.
