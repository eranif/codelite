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

This page gives links to the available binary packages for CodeLite 18.1. Its RAD plugin wxCrafter, which is now Free Open-Source Software,
is included in all the CodeLite packages.

Note that the CodeLite packages incorporate the required wxWidgets libraries (`wx3.2.2` or `wx3.2.4`), so there is no need to install wxWidgets too.
Of course you can if you wish, and you'll need to do so to build apps against wxWidgets.

Ubuntu `16.04` ( `xenial` ) and later have official CodeLite packages. Therefore, to avoid a name-clash, our CodeLite packages have `unofficial` appended to the name. (Update: with the exception of debian 12 (bookworm); see below.)

To download CodeLite for Debian based systems (Debian / Mint / Ubuntu etc):

- Add the CodeLite public key to avoid warnings or worse from apt/aptitude. Unfortunately this has recently become much more complicated: see [this post and thread][20] for a summary. It is currently also different for the two distros.

For debian 12 (bookworm) and 13 (trixie) and Mint 22.2 (Zara), and earlier, do:
```bash
wget -qO- http://repos.codelite.org/CodeLite.asc | sudo tee /etc/apt/trusted.gpg.d/CodeLite.asc
```
The new, complicated, ubuntu method: for ubuntu 24.04 (noble), 24.10 (oracular) and 25.04 (plucky), all of which use the same CodeLite package, do:
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


- Now let apt know that the repositories exist by adding the proper line from Table 1 or 2:
    - If you use an `apt` front-end e.g. synaptic, tell it to Add a repository, using as data the appropriate entry from the tables below
    - If you do things by hand, add the appropriate URLs from the tables to `/etc/apt/sources.list` (you need to become superuser) :

For example, if you use Ubuntu noble (24.04), either open `/etc/apt/sources.list` in an editor (as superuser) and append the proper line
from Table 1, or else use `apt-add-repository` in a terminal:

```bash
 sudo apt-add-repository 'deb https://repos.codelite.org/ubuntu/ noble universe'
```

*Table 1: CodeLite repositories:*

Distro/release 	| Lines to append
----------------|---------------------------
debian bookworm	| `deb https://repos.codelite.org/debian/ bookworm devel`
debian trixie	| `deb https://repos.codelite.org/debian/ trixie devel`
ubuntu noble oracular plucky and questing   |	`deb https://repos.codelite.org/ubuntu/ noble universe`

- You then need to update your repositories. In synaptic, click the `Reload` button. If you're doing things by hand, do:

```bash
 sudo apt update
 sudo apt install codelite
```

!!! Tip
    The Ubuntu packages should also work on other ubuntu flavours e.g. xubuntu, and on derivatives e.g. Mint.

#### Apt

You can see which versions are available by doing:

```bash
 apt-cache madison codelite
```

Then you can select which version to install. For example, to prefer the official 17.0 package in debian bookworm, use the line:

```bash
 sudo apt install codelite=17.0*
```


#### Synaptic

You can see which versions are available in the 'Versions' tab of Properties. Then use Package > Force Version followed by Apply.
You may wish also to use Package > Lock Version.

#### Official versus unofficial CodeLite packages
---

For a long time the official debian and ubuntu repositories supplied only the ancient CodeLite 6.1. However since ubuntu `16.04` (xenial) it has been updated to
CodeLite 9.1 or later, and debian stretch has 10.0. This is very welcome, but creates a name-clash with any packages we supply.
Therefore, starting with CodeLite 9.2, our debian packages have `unofficial` added to the version e.g. `codelite_17.0-1unofficial.bullseye_amd64.deb`
This allows you to select which to install using apt (or aptitude or synaptic).

HOWEVER: In debian 12 (bookworm) apt fails to recognise packages with 'unoffical' in the name, so I've removed the word. debian 13 (trixie) is OK, so it's officially 'unofficial' again.


### CodeLite RPMs
---

There are rpms available for fedora 41 and openSUSE Leap 15.6:

Distro	| x86_64 GTK+3
--------|----------------
fedora	| [`41`][12] 
openSUSE | [`15.6`][15]


First tell rpm about the CodeLite public key. As superuser run:

```bash
 rpm --import https://repos.codelite.org/CodeLite.asc
```

Then either download the required rpm and install it as usual, or download and install in one step; e.g.

```bash
  rpm -Uvh https://repos.codelite.org/rpms-18.1/fedora/41/codelite-18.1.0-1.fc41.x86_64.rpm
```

There are also source rpms for [fedora 41][2] and [OpenSUSE][4]

### ArchLinux
---

CodeLite is available on ArchLinux via their [AUR repository][5]
To install CodeLite:

```bash
yaourt -S codelite
```


### Previous CodeLite releases
---

See [this page](./oldDownloads/Repositories16.md) for links to Linux packages from previous CodeLite and wxCrafter releases.

### wxCrafter binary packages
---

As the above CodeLite packages already contain wxCrafter, its gui-designer plugin, there is no need for a separate download. However if you wish to use wxCrafter with CodeLite 12.0 or older, please see [this page](./oldDownloads/Repositories12.md).

[1]: https://downloads.codelite.org
[2]: https://repos.codelite.org/rpms-18.1/fedora/codelite-18.1.0-1.fc41.src.rpm

[4]: https://repos.codelite.org/rpms-18.1/suse/codelite-18.1.0-1.suse.src.rpm
[5]: https://aur.archlinux.org/packages/codelite/


[12]: https://repos.codelite.org/rpms-18.1/fedora/41/codelite-18.1.0-1.fc41.x86_64.rpm

[13]: https://repos.codelite.org/rpms-17.0/suse/15.4/codelite-17.0-1.suse.x86_64.rpm

[15]: https://repos.codelite.org/rpms-18.1/suse/15.6/codelite-18.1.0-1.suse15.6.x86_64.rpm

[9]: https://brew.sh

[20]: [https://askubuntu.com/questions/1286545/what-commands-exactly-should-replace-the-deprecated-apt-key/1307181#1307181]
