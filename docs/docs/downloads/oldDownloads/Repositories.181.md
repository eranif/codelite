# Downloads

## Windows
---
We provide pre‑built 64‑bit binaries for Windows. Visit the [CodeLite downloads page][1] and select the version you need.

There are two variants available:

- **Stable release** – more stable, released 1‑2 times per year.
- **Weekly builds** – released more frequently. Choose this if you want to try the latest features. Despite the name, these builds are very stable.

## macOS
---
Since version `16.1.0`, CodeLite can be installed via **Homebrew**.

1. Install Homebrew (if you haven’t already) from [brew.sh][9].
2. Open **Terminal.app** and run:

```bash
# Run this only once
brew tap eranif/codelite

# Refresh the formulae cache
brew update

# Install or reinstall CodeLite
brew reinstall --cask codelite-official

# Remove macOS security warning for a damaged app
xattr -cr /Applications/codelite.app/
```

CodeLite will be installed at `/Applications/CodeLite.app`.

## Linux
---
### Ubuntu / Debian
---
#### Setup

This page lists the binary packages for CodeLite 18.1. The RAD plugin **wxCrafter** (now free open‑source software) is included in all packages.

The packages already contain the required wxWidgets libraries (`wx3.2.2` or `wx3.2.4`), so you do not need to install wxWidgets separately unless you want to build applications against them.

Ubuntu 16.04 (xenial) and newer provide official CodeLite packages. To avoid a name clash, our packages have the suffix **unofficial** (except for Debian 12 bookworm, see below).

#### Adding the repository key

*For Debian 12 (bookworm), Debian 13 (trixie), Mint 22.2 (Zara) and earlier:*

```bash
wget -qO- http://repos.codelite.org/CodeLite.asc | sudo tee /etc/apt/trusted.gpg.d/CodeLite.asc
```

*For newer Ubuntu releases (24.04 noble, 24.10 oracular, 25.04 plucky):*

```bash
sudo mkdir -m 0755 -p /etc/apt/keyrings/
wget -qO- http://repos.codelite.org/CodeLite.asc | sudo tee -a /etc/apt/keyrings/CodeLite.asc > /dev/null

echo "Types: deb
URIs: http://repos.codelite.org/ubuntu-18.1
Suites: noble
Components: universe
Signed-By: /etc/apt/keyrings/CodeLite.asc" |
    sudo tee /etc/apt/sources.list.d/CodeLite.sources > /dev/null
```

#### Adding the repository source

Add the appropriate line from the table below:

*If you use an APT front‑end (e.g., Synaptic), add the repository using the listed entry.
*If you edit sources manually, append the line to `/etc/apt/sources.list` as root.

**Example for Ubuntu noble (24.04):**

```bash
sudo apt-add-repository 'deb https://repos.codelite.org/ubuntu/ noble universe'
```

| Distro / Release | Repository line |
|------------------|-----------------|
| Debian bookworm  | `deb https://repos.codelite.org/debian/ bookworm devel` |
| Debian trixie    | `deb https://repos.codelite.org/debian/ trixie devel` |
| Ubuntu noble, oracular, plucky, questing | `deb https://repos.codelite.org/ubuntu/ noble universe` |

After adding the source, update the package list and install CodeLite:

```bash
sudo apt update
sudo apt install codelite
```

> **Tip:** The Ubuntu packages also work on other Ubuntu flavors (e.g., Xubuntu) and derivatives such as Mint.

#### Using APT directly

List available versions:

```bash
apt-cache madison codelite
```

Install a specific version (e.g., the official `17.0` package on Debian bookworm):

```bash
sudo apt install codelite=17.0*
```

#### Using Synaptic

1. Open the package’s **Properties** and go to the **Versions** tab.
2. Choose **Package → Force Version**, then **Apply**.
3. (Optional) **Package → Lock Version** to prevent upgrades.

#### Official vs. unofficial packages

Historically, the official Debian/Ubuntu repositories only offered the very old CodeLite 6.1. Since Ubuntu 16.04 (xenial), they provide CodeLite 9.1 or newer, and Debian stretch offers 10.0. This created a name clash with our packages, so we added the suffix **unofficial** to our package names (e.g., `codelite_17.0-1unofficial.bullseye_amd64.deb`).

*Note:* Debian 12 bookworm cannot handle the `unofficial` suffix, so we removed it for that release. Debian 13 trixie works with the suffix again.

### CodeLite RPMs
---
RPMs are available for Fedora 41 and openSUSE Leap 15.6:

| Distro | Version |
|--------|---------|
| Fedora | [`41`][12] |
| openSUSE | [`15.6`][15] |

Import the CodeLite public key as root:

```bash
rpm --import https://repos.codelite.org/CodeLite.asc
```

Install an RPM directly (example for Fedora 41):

```bash
rpm -Uvh https://repos.codelite.org/rpms-18.1/fedora/41/codelite-18.1.0-1.fc41.x86_64.rpm
```

Source RPMs are also available for [Fedora 41][2] and [openSUSE][4].

### Arch Linux
---
CodeLite is in the Arch User Repository (AUR). Install it with:

```bash
yaourt -S codelite
```

### Previous CodeLite releases
---
Links to older Linux packages are provided on [this page](./oldDownloads/Repositories16.md).

### wxCrafter binary packages
---
wxCrafter is already included in the CodeLite packages above, so a separate download is unnecessary. If you need wxCrafter for CodeLite 12.0 or older, see [this page](./oldDownloads/Repositories12.md).

[1]: https://downloads.codelite.org
[2]: https://repos.codelite.org/rpms-18.1/fedora/codelite-18.1.0-1.fc41.src.rpm
[4]: https://repos.codelite.org/rpms-18.1/suse/codelite-18.1.0-1.suse.src.rpm
[5]: https://aur.archlinux.org/packages/codelite/
[9]: https://brew.sh
[12]: https://repos.codelite.org/rpms-18.1/fedora/41/codelite-18.1.0-1.fc41.x86_64.rpm
[15]: https://repos.codelite.org/rpms-18.1/suse/15.6/codelite-18.1.0-1.suse15.6.x86_64.rpm
[20]: https://askubuntu.com/questions/1286545/what-commands-exactly-should-replace-the-deprecated-apt-key/1307181#1307181
