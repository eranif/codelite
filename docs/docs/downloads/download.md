# Downloads

## Windows
---

We provide pre-built binaries for Windows for both 32 and 64 bit.
visit [CodeLite downloads page][1] and pick the version you want.

Note that there are two variants to choose from:

- Stable release: more stable releases, released 1-2 per year.
- Weekly builds: more frequent releases.
   If you wish to experiment with the latest features, pick this variant.
  Note that despite their name, these releases are very stable.

## macOS
---

Since version `16.1.0`, CodeLite installation method is by using [`brew`][9]

- Install `brew` from [here][9]
- Open `Terminal.app` and type:

```bash
# the below is only needed once
brew tap eranif/codelite

# update the cache
brew update

# install CodeLite
brew reinstall --cask codelite-official

# in order to avoid macOS errors about "damaged app can't be opened"...
xattr -cr /Applications/codelite.app/
```

- CodeLite should now be installed under `/Applications/CodeLite.app`

## Linux
---

### Ubuntu / Debian
---

#### Setup

This page gives links to the available binary packages for CodeLite 16.0. Its RAD plugin wxCrafter, which is now Free Open-Source Software,
is included in all the CodeLite packages.

Note that the CodeLite packages incorporate the required wxWidgets libraries (`wx3.1.6`), so there is no need to install wxWidgets too.
Of course you can if you wish, and you'll need to do so to build apps against wxWidgets.
CodeLite doesn't (yet) require wx3.1 though, and you can still build your own against wx3.0.x if you prefer.

Ubuntu `16.04` (`xenial`) and later have official CodeLite packages. Therefore, to avoid a name-clash, our CodeLite packages have `unofficial` appended to the name.

To download CodeLite for Debian based systems (Debian / Mint / Ubuntu etc):

- Add the CodeLite public key to avoid warnings or worse from apt/aptitude

```bash
sudo apt-key adv --fetch-keys http://repos.codelite.org/CodeLite.asc
```

- Now let apt know that the repositories exist by adding the proper line from Table 1 or 2:
    - If you use an `apt` front-end e.g. synaptic, tell it to Add a repository, using as data the appropriate entry from the tables below
    - If you do things by hand, add the appropriate URLs from the tables to `/etc/apt/sources.list` (you need to become superuser) :

For example, if you use Ubuntu jammy (22.04), either open `/etc/apt/sources.list` in an editor (as superuser) and append the proper line
from Table 1, or else use `apt-add-repository` in a terminal:

```bash
 sudo apt-add-repository 'deb https://repos.codelite.org/ubuntu/ jammy universe'
```

*Table 1: CodeLite repositories (GTK+3):*

Distro/release 	| Lines to append
----------------|---------------------------
debian buster 	| `deb https://repos.codelite.org/debian/ buster devel`
debian bullseye	| `deb https://repos.codelite.org/debian/ bullseye devel`
ubuntu focal 	| `deb https://repos.codelite.org/ubuntu/ focal universe`
ubuntu impish |	`deb https://repos.codelite.org/ubuntu/ impish universe`
ubuntu jammy |	`deb https://repos.codelite.org/ubuntu/ jammy universe`

- You then need to update your repositories. In synaptic, click the `Reload` button. If you're doing things by hand, do:

```bash
 sudo apt update
 sudo apt install codelite
```

!!! Tip
    The Ubuntu packages should also work on other *buntus, and on derivatives e.g. Mint.

#### Apt

You can see which versions are available by doing:

```bash
 apt-cache madison codelite
```

Then you can select which version to install. For example, to prefer the official 14.0 package in debian buster, use the line:

```bash
 sudo apt install codelite=14.0*
```

#### Synaptic

You can see which versions are available in the 'Versions' tab of Properties. Then use Package > Force Version followed by Apply.
You may wish also to use Package > Lock Version.

#### Official versus unofficial CodeLite packages
---

For a long time the official debian and ubuntu repositories supplied only the ancient CodeLite 6.1. However since ubuntu `16.04` (xenial) it has been updated to
CodeLite 9.1 or later, and debian stretch has 10.0. This is very welcome, but creates a name-clash with any packages we supply.
Therefore, starting with CodeLite 9.2, our debian packages have `unofficial` added to the version e.g. `codelite_16.0-1unofficial.bullseye_amd64.deb`

This allows you to select which to install using apt (or aptitude or synaptic).

Update: ubuntu `22.10` (kinetic) has official CodeLite 16.0 packages, so there is no need for unofficial ones.

### CodeLite RPMs
---

There are rpms available for fedora 35 and 36, and openSUSE Leap 15.3:

Distro	| x86_64 GTK+3
--------|----------------
fedora	| [`35`][5] [`36`][6] [`37`][10]
openSUSE |	[`15.3`][7] [`15.4`][8]


First tell rpm about the CodeLite public key. As superuser do:

```bash
 rpm --import https://repos.codelite.org/CodeLite.asc
 ```
Then either download the required rpm and install it as usual, or download and install in one step; e.g.
```bash
  rpm -Uvh https://repos.codelite.org/rpms-16.0/fedora/37/codelite-16.0-1.fc37.x86_64.rpm
```
There are also source rpms for [fedora][2] and [OpenSUSE][3]

### ArchLinux
---

CodeLite is available on ArchLinux via their [AUR repository][4]
To install CodeLite:

```bash
yaourt -S codelite
```

### Release candidates
---

For Linux, we also offer release candidate builds for Ubuntu `18.04` and `20.04`
You can download the deb file from our [download page][1] and install it using `gdebi` :

```bash
sudo apt install gdebi
sudo gdebi -n /path/to/codelite.deb
```

### Previous CodeLite releases
---

See [this page](./oldDownloads/Repositories15.md) for links to Linux packages from previous CodeLite and wxCrafter releases.

### wxCrafter binary packages
---

As the above CodeLite 16.0 packages already contain wxCrafter, its gui-designer plugin, there is no need for a separate download. However if you wish to use wxCrafter with CodeLite 12.0 or older, please see [this page](./oldDownloads/Repositories12.md).

[1]: https://downloads.codelite.org
[2]: https://repos.codelite.org/rpms-16.0/fedora/codelite-16.0-3.fc.src.rpm
[3]: https://repos.codelite.org/rpms-16.0/suse/codelite-16.0-1.suse.src.rpm
[4]: https://aur.archlinux.org/packages/codelite/
[5]: https://repos.codelite.org/rpms-16.0/fedora/35/codelite-16.0-1.fc35.x86_64.rpm
[6]: https://repos.codelite.org/rpms-16.0/fedora/36/codelite-16.0-1.fc36.x86_64.rpm
[10]: https://repos.codelite.org/rpms-16.0/fedora/37/codelite-16.0-1.fc37.x86_64.rpm
[7]: https://repos.codelite.org/rpms-16.0/suse/15.3/codelite-16.0-1.suse.x86_64.rpm
[8]: https://repos.codelite.org/rpms-16.0/suse/15.4/codelite-16.0-1.suse.x86_64.rpm
[9]: https://brew.sh
