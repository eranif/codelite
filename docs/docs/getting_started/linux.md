# Linux

Linux offers multiple ways to install software:

- Using the distro package manager ( `yum`, `apt`, `pacman` etc )
- Using `brew`
- Using `snap`

As a general rule:

If you are happy with the version that your distro offers you - then use it.
However, older Linux versions do not upgrade their packages fast enough or at
al. In such cases, I recommed you to use `brew`

## Install the essentials

(the below code uses `apt`):

```bash
sudo apt install build-essential            \
                 git cmake                  \
                 libssh-dev libsqlite3-dev  \
                 libpcre2-dev
```

## Optional: install `Rust`

- Install `Rust` on Linux is easy as:

```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

- Install `rust-analyzer` for code completion:

```bash
rustup update
rustup +nightly component add rust-src rust-analyzer-preview
```

## Optional: install `Python`

- On `Debian` / `Ubuntu` use this command:

```bash
sudo apt-get install python3
```

- Install `pip`

```bash
wget https://bootstrap.pypa.io/get-pip.py -O /tmp/get-pip.py
python3 /tmp/get-pip.py
```

- Install `black` - python code formatter:

```bash
python3 -m pip install black
```

- Install `pylsp` - python code completion engine:

```bash
python3 -m pip install python-lsp-server
```

- Install `debugpy` - python debugger:

```bash
python3 -m pip install debugpy
```

## Optional: install `brew` for Linux

Although associated with macOS, brew also supports Linux. It is a good option when your distro is old and does not provide
the latest software.

To install `brew` on Linux:

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

By default, `brew` will install software under:

```bash
/home/linuxbrew/.linuxbrew/bin
```

So make sure you add this to your `$(basename $SHELL)rc` file:

```bash
echo 'export PATH=/home/linuxbrew/.linuxbrew/bin:$PATH' >> $HOME/.$(basename $SHELL)rc
source $HOME/.$(basename $SHELL)rc
```

Once installed you can install software similar to [macOS][1]

[1]: /getting_started/macos
