
This page gives links to the available binary packages for CodeLite 14.0. Its RAD plugin  [wxCrafter](http://wxcrafter.codelite.org/). which has recently become Free Open-Source Software, is now included in all the CodeLite packages.

See [this page](../Repositories13) for links to packages from previous CodeLite and wxCrafter releases.

Note that the CodeLite packages incorporate the required wxWidgets libraries (wx3.1.3), so there is no need to install wxWidgets too. Of course you can if you wish, and you'll need to do so to build apps against wxWidgets.
CodeLite doesn't (yet) ''require'' wx3.1 though, and you can still build your own against wx3.0.x if you prefer.

CodeLite apt repositories
----

Ubuntu 16.04 (xenial) and later have official CodeLite packages. Therefore, to avoid a name-clash, our CodeLite packages have 'unofficial' appended to the name.  See below for more details.


!!! Important
    Except for fedora, CodeLite 13.0 provided both GTK+2 and GTK+3-based packages; previous CodeLite releases used only GTK+2. For CodeLite 14.0 we are providing '''only''' GTK+3-based packages.
    To avoid conflicts and reduce confusion, there are separate GTK+2 and GTK+3 repositories.


To download CodeLite for Debian based systems (Debian / Mint / Ubuntu etc):

  * Add the CodeLite public key to avoid warnings or worse from apt/aptitude

```bash
sudo apt-key adv --fetch-keys http://repos.codelite.org/CodeLite.asc
```

- Now let apt know that the repositories exist by adding the proper line from Table 1 or 2:
    - If you use an apt front-end e.g. synaptic, tell it to Add a repository, using as data the appropriate entry from the tables below
    - If you do things by hand, add the appropriate URLs from the tables to `/etc/apt/sources.list` (you need to become superuser) :

For example, if you use Ubuntu focal (20.04), either open `/etc/apt/sources.list` in an editor (as superuser) and append the proper line
from Table 1, or else use `apt-add-repository` in a terminal:

```bash
 sudo apt-add-repository 'deb https://repos.codelite.org/ubuntu3-14.0/ focal universe'
```

*Table 1: CodeLite repositories (GTK+3):* 

Distro/release 	| Lines to append
----------------|---------------------------
debian buster 	| `deb https://repos.codelite.org/debian3-14.0/ buster devel`
ubuntu bionic 	| `deb https://repos.codelite.org/ubuntu3-14.0/ bionic universe`
ubuntu eoan 	| `deb https://repos.codelite.org/ubuntu3-14.0/ eoan universe`
ubuntu focal 	| `deb https://repos.codelite.org/ubuntu3-14.0/ focal universe`

*Table 2: CodeLite repositories (GTK+2):* 

Distro/release 	| Lines to append
----------------|---------------------------
debian stretch 	| `deb https://repos.codelite.org/debian2/ stretch devel`
debian buster 	| `deb https://repos.codelite.org/debian2/ buster devel`
ubuntu xenial 	| `deb https://repos.codelite.org/ubuntu2/ xenial universe`
ubuntu bionic 	| `deb https://repos.codelite.org/ubuntu2/ bionic universe`
ubuntu cosmic 	| `deb https://repos.codelite.org/ubuntu2/ cosmic universe`
ubuntu disco 	| `deb https://repos.codelite.org/ubuntu2/ disco universe`
ubuntu eoan 	| `deb https://repos.codelite.org/ubuntu2/ eoan universe`


 You then need to update your repositories. In synaptic, click the `Reload` button. If you're doing things by hand, do: 

```bash
sudo apt update
```

* You should now be able to install CodeLite in the normal way through synaptic, or by doing:

```bash
sudo apt install codelite
```

!!! Tip
    The Ubuntu packages should also work on other *buntus, and on derivatives e.g. Mint.   
    
    Ubuntu 20.10 (groovy) supplies official CodeLite 14.0 packages which work well on ubuntu, kubuntu and xubuntu, so I've not created unofficial ones. However (at least in a VirtualBox install) on Lubuntu, CodeLite 14.0 crashes on starting. A work-around is to install the 20.04 (focal) unofficial CodeLite 14.0, which installs and runs correctly.

#### Apt

You can see which versions are available by doing: 

```bash
apt-cache madison codelite
```

Then you can select which version to install. For example, to prefer the official 12.0 package in debian buster, use the line: 

```bash
sudo apt-get install codelite=12.0*
```

#### Synaptic

You can see which versions are available in the 'Versions' tab of Properties. Then use Package > Force Version followed by Apply. 
You may wish also to use Package > Lock Version. 

#### Official versus unofficial CodeLite packages
---

For a long time the official debian and ubuntu repositories supplied only the ancient CodeLite 6.1. However since ubuntu 16.04 (xenial) it has been updated to CodeLite 9.1 or later, and debian stretch has 10.0. This is very welcome, but creates a name-clash with any packages we supply. Therefore, starting with CodeLite 9.2, our debian packages have 'unofficial' added to the version e.g. codelite_14.0-1unofficial3.buster_amd64.deb. This allows you to select which to install using apt (or aptitude or synaptic). 


CodeLite RPMs
---

There are rpms available for fedora 31, 32 and 33, and openSUSE Leap 15.1 and 15.2: 

Distro| x86_64 GTK+3
--------|----------------
fedora| [`31`][5]  	  [`32and33`][6]
openSUSE | [`15.1and15.2`][7]


First tell rpm about the CodeLite public key. As superuser do:
```bash
rpm --import https://repos.codelite.org/CodeLite.asc
```


Then either download the required rpm and install it as usual, or download and install in one step; e.g.
```bash
rpm -Uvh https://repos.codelite.org/rpms-14.0/fedora/codelite-14.0-2.fc32.x86_64.rpm
```

There are also source rpms for [fedora](https://repos.codelite.org/rpms-14.0/fedora/codelite-14.0-2.fc.src.rpm)  and [openSUSE](https://repos.codelite.org/rpms-14.0/suse/codelite-14.0-1.gtk3suse.src.rpm).

! ArchLinux
----

CodeLite is available on ArchLinux via their [AUR repository][4]

To install codelite:

```bash
yaourt -S codelite
```




!wxCrafter binary packages
----

As the above CodeLite 14.0 packages already contain wxCrafter, its gui-designer plugin, there is no need for a separate download. However if you wish to use wxCrafter with CodeLite 12.0 or older, please see [this page](../Repositories12).

[1]: https://downloads.codelite.org
[4]: https://aur.archlinux.org/packages/codelite/
[5]: https://repos.codelite.org/rpms-14.0/fedora/codelite-14.0-2.fc31.x86_64.rpm
[6]: https://repos.codelite.org/rpms-14.0/fedora/codelite-14.0-2.fc32.x86_64.rpm
[7]: https://repos.codelite.org/rpms-14.0/suse/gtk3/codelite-14.0-1.gtk3suse.x86_64.rpm
