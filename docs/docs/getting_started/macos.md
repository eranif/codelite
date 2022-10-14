# macOS

## Install brew

`brew` is the unofficial package manager for macOS.
To install it:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

## Install common packages

- `llvm` toolchain which includes
    - `clangd` - c++ code completion LSP server
    - `clang-format` - c++ source code fromatter
- `jq` - the Json file formatter
- `yq` - the Yaml file formatter

```bash
brew update
brew install git        \
             cmake      \
             libssh     \
             hunspell   \
             jq         \
             yq         \
             llvm
```

After the installation process is done, open terminal app and run this:

```bash
echo 'export PATH=/opt/homebrew/bin:$PATH' >> $HOME/.$(basename $SHELL)rc
source $HOME/.$(basename $SHELL)rc
```
