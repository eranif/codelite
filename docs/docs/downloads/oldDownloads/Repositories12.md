
This page gives links to the available binary packages for CodeLite 12.0 and its RAD plugin  [wxCrafter](http://wxcrafter.codelite.org/).

Note that the CodeLite packages now incorporate the required wxWidgets libraries (mostly wx3.1.1), so there is no need to install wxWidgets too. Of course you can if you wish, and you'll need to do so to build apps against wxWidgets. CodeLite doesn't (yet) require wx3.1 though, and you can still build your own against wx3.0.x if you prefer. 

CodeLite apt repositories
----

!!! Note
    Ubuntu 16.04 (xenial) and later have official CodeLite packages. Therefore, to avoid a name-clash, their CodeLite 12.0 packages have 'unofficial' appended to the name. See below for more details. Ubuntu artful, bionic and cosmic, and debian stretch, have official 10.0 packages. I don't know when/if they will upgrade to 12.0, so meanwhile we provide unofficial 12.0 ones. Ubuntu 19.04 (disco) has 12.0 packages so there is no need for an unofficial one.
    There are wxCrafter 2.9 packages for the corresponding unofficial CodeLite 12.0 ones and for the official disco CodeLite 12.0. wxCrafter 2.7 packages are available for the artful, zesty and stretch official 10.0 ones (the artful ones should work for bionic and cosmic too). Again, see below for important details.

To download CodeLite for Debian based systems (Debian / Mint / Ubuntu etc):

  * Add the CodeLite public key to avoid warnings or worse from apt/aptitude

```bash
sudo apt-key adv --fetch-keys http://repos.codelite.org/CodeLite.asc
```

- Now let apt know that the repositories exist:
    - If you use an apt front-end e.g. synaptic, tell it to Add a repository, using as data the appropriate entry from Table 1 below
    - If you do things by hand, add the appropriate URLs from Table 1 to `/etc/apt/sources.list` (you need to become superuser) :

For example, if you use Ubuntu cosmic (18.10), either open `/etc/apt/sources.list` in an editor (as superuser) and append the proper line
from Table 1, or else use `apt-add-repository` in a terminal:

```bash
 sudo apt-add-repository 'deb https://repos.codelite.org/ubuntu-12.0/ cosmic universe'
```

*Table 1: CodeLite repositories:* 

Distro/release 	| Lines to append
----------------|---------------------------
debian stretch 	| `deb https://repos.codelite.org/debian-12.0/ stretch devel`
ubuntu xenial 	| `deb https://repos.codelite.org/ubuntu-12.0/ xenial universe`
ubuntu artful 	| `deb https://repos.codelite.org/ubuntu-12.0/ artful universe`
ubuntu bionic 	| `deb https://repos.codelite.org/ubuntu-12.0/ bionic universe`
ubuntu cosmic 	| `deb https://repos.codelite.org/ubuntu-12.0/ cosmic universe`


 You then need to update your repositories. In synaptic, click the `Reload` button. If you're doing things by hand, do: 

```bash
sudo apt update
```

* You should now be able to install CodeLite in the normal way through synaptic, or by doing:

```bash
sudo apt install codelite wxcrafter
```

!!! Tip
    The Ubuntu packages should also work on other *buntus, and on derivatives e.g. Mint.

#### Apt

You can see which versions are available by doing: 

```bash
apt-cache madison codelite
```

Then you can select which version to install. For example, to prefer the official 9.1 package, use the line: 

```bash
sudo apt-get install codelite=9.1*
```

#### Synaptic

You can see which versions are available in the 'Versions' tab of Properties. Then use Package > Force Version followed by Apply. 
You may wish also to use Package > Lock Version. 

#### Official versus unofficial CodeLite packages
---
For a long time the official debian and ubuntu repositories supplied only the ancient CodeLite 6.1. However since ubuntu 16.04 (xenial) it has been updated to CodeLite 9.1 or later, and debian stretch has 10.0. This is very welcome, but creates a name-clash with any packages we supply. Therefore, starting with CodeLite 9.2, our debian packages have 'unofficial' added to the version e.g. codelite_12.0-1unofficial.cosmic_amd64.deb. This allows you to select which to install using apt (or aptitude or synaptic). 

CodeLite RPMs
---

There are rpms available for fedora 27, 28, 29 and 30, and openSUSE Leap 42.3 and 15: 

Distro|  i386 | x86_64
--------|--------|--------
fedora|  [`27`][5]  |  [`27`][6]
fedora|  [`28`][7]  |  [`28`][8]
fedora|  [`29`][9]  |  [`29`][10]
fedora|  [`30`][11]  |  [`30`][12]
openSUSE | 	(No 32-bit build)  | [`42.3`][13]  	  [`15`][14]



First tell rpm about the CodeLite public key. As superuser do:
```bash
rpm --import https://repos.codelite.org/CodeLite.asc
```


Then either download the required rpm and install it as usual, or download and install in one step; e.g.
```bash
rpm -Uvh https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-1.fc28.x86_64.rpm
```

There are also source rpms for [fedora](https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-2.fc.src.rpm)  and [openSUSE](https://repos.codelite.org/rpms-12.0/suse/codelite-12.0-2.suse.src.rpm). Note that these do not contain wxCrafter. To build binary rpms containing wxCrafter, you'll need to download the appropriate wxCrafter.so, plus the file wxgui.zip that contains its bitmaps, and put both in the rpmbuild/SOURCES dir before you build. 


ArchLinux
----

CodeLite is available on ArchLinux via their [AUR repository][4]

To install codelite:

```bash
yaourt -S codelite
```


wxCrafter binary packages
----

The above packages supply the CodeLite IDE. Also available is [wxCrafter](http://wxcrafter.codelite.org/), its gui-designer plugin. There are wxCrafter packages, labelled 2.9, here for disco and for the **unofficial** CodeLite packages.

There are 32 and 64 bit packages here for: debian stretch; ubuntu xenial, artful, bionic and (64 bit only) cosmic and disco ; for fedora 27, 28 and 29; and for openSUSE 42.3. Apart from artful/bionic/cosmic/disco, they are built against wxWidgets-3.1; so these unofficial ones will **not** work on earlier distro versions (though they *might* work if you build your own CodeLite against wx3.1...). 
Another complication: openSUSE Leap 15 has now been released. It has its own official wx3.1.1 packages, one of which (wxWidgets-3_2-devel) I have used to build both CodeLite and wxCrafter. Therefore, if you build your own CodeLite against a different wxWidgets version, this wxCrafter rpm  will be very unlikely to work with it; however perhaps the 42.3 one will.


In the following table I've provided direct links to the debian/ubuntu packages but these are more easily installed from the CodeLite repos in the usual way.


*wxCrafter packages:* 

|Distro|  i386 | x86_64 |
--------|--------|--------
debian stretch unofficial | [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/stretch/32/wxcrafter_2.9-1unofficial.stretch_i386.deb)  |  [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/stretch/64/wxcrafter_2.9-1unofficial.stretch_amd64.deb)
ubuntu 16.04 (xenial) unofficial | [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/xenial/32/wxcrafter_2.9-1unofficial.xenial_i386.deb)  | [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/xenial/64/wxcrafter_2.9-1unofficial.xenial_amd64.deb)
ubuntu 17.10 (artful) unofficial | [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/artful/32/wxcrafter_2.9-1unofficial.artful_i386.deb)  | [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/artful/64/wxcrafter_2.9-1unofficial.artful_amd64.deb)  | wxcrafter.deb  |
ubuntu 18.04 (bionic) unofficial | [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/bionic/32/wxcrafter_2.9-1unofficial.bionic_i386.deb)  | [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/bionic/64/wxcrafter_2.9-1unofficial.bionic_amd64.deb)  | 
ubuntu 18.10 (cosmic) unofficial | (No 32-bit build) | [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/cosmic/64/wxcrafter_2.9-1unofficial.cosmic_amd64.deb) |
ubuntu 19.04 (disco) | (No 32-bit build) | [wxcrafter.deb](https://repos.codelite.org/wxCrafter-2.9/disco/64/wxcrafter_2.9-1.disco_amd64.deb) |
fedora 27 28 29 | [wxcrafter.rpm](https://repos.codelite.org/wxCrafter-2.9/fedora/32/wxcrafter-2.9-1.i686.rpm)  | [wxcrafter.rpm](https://repos.codelite.org/wxCrafter-2.9/fedora/64/wxcrafter-2.9-1.x86_64.rpm) |
fedora 30 | [wxcrafter.rpm](https://repos.codelite.org/wxCrafter-2.9/fedora/30/32/wxcrafter-2.9-1.i686.rpm)  | [wxcrafter.rpm](https://repos.codelite.org/wxCrafter-2.9/fedora/30/64/wxcrafter-2.9-1.x86_64.rpm)  |
openSUSE 42.3 | (No 32-bit build) | [wxcrafter.rpm](https://repos.codelite.org/wxCrafter-2.9/openSUSE/42.3/wxcrafter-2.9-1.x86_64.rpm) |
openSUSE 15 | (No 32-bit build) | [wxcrafter.rpm](https://repos.codelite.org/wxCrafter-2.9/openSUSE/15/wxcrafter-2.9-1.x86_64.rpm)  |

Download and install them with alongside CodeLite (it will do no harm to install wxCrafter *without* CodeLite, but it won't run!).
Some of the debs are also available from the CodeLite repo (see above); the setup and public key are the same, but the packages are called 'wxcrafter'.

wxCrafter libs
----

What if you don't want to use a package, or there isn't one that your distro supports? If you want to build your own CodeLite (using wxWidgets 3.1), or are using the official xenial ones (which use wx3.0.2), here are the wxCrafter libs that the packages contain.
Choose the nearest match, but there's no guarantee they will work on distant versions or different distros.

*wxCrafter libs:*

|Distro|  i386 | x86_64 |
--------|--------|--------
|debian stretch  | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/stretch/32/wxCrafter.so) | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/stretch/64/wxCrafter.so) |
|ubuntu xenial unofficial | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/xenial/32/wxCrafter.so) | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/xenial/64/wxCrafter.so) |
|ubuntu disco | (No 32-bit build) | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/disco/64/wxCrafter.so) |
|fedora 27 28 29 | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/fedora/32/wxCrafter.so) | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/fedora/64/wxCrafter.so)  |
|fedora 30 | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/fedora/30/32/wxCrafter.so) | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/fedora/30/64/wxCrafter.so)  |
|openSUSE 42.3 | (No 32-bit build)  | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/openSUSE/42.3/wxCrafter.so) |
|openSUSE 15 | (No 32-bit build)  | [wxCrafter.so](https://repos.codelite.org/wxCrafter-2.9/openSUSE/15/wxCrafter.so) |

You will also need the file [wxgui.zip](https://repos.codelite.org/wxCrafter-2.9/wxgui.zip ) that contains some of wxCrafter's bitmaps. This needs to end up inside the CodeLite 'share' installation (e.g. in /usr/share/codelite/). The wxCrafter.so itself needs to go in CodeLite's 'plugins' dir, which by default is probably /usr/lib/codelite/.
How to do this? Well, you could just add them by hand as superuser. However there are other alternatives:

  * If you are building from a tarball, 'configure' and 'make' as usual. Then before 'make install' add wxCrafter.so to Runtime/plugins/; it'll then be installed with the other plugin libs. There is no similar solution for wxgui.zip at present, but this might be fixed sometime...
  * If you are building a CodeLite .deb package using the debian/rules from the repo, just add both wxCrafter.so and wxgui.zip to the same dir as the source tarball. 'rules' knows to look for them there.
  * If you are using the fedora or openSUSE spec files to build an rpm, just add both wxCrafter.so and wxgui.zip to rpmbuild/SOURCES/. You can copy the .spec hack if you create a .spec for other distros.




[1]: https://downloads.codelite.org
[4]: https://aur.archlinux.org/packages/codelite/
[5]: https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-1.fc27.i686.rpm
[6]: https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-1.fc27.x86_64.rpm
[7]: https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-1.fc28.i686.rpm
[8]: https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-1.fc28.x86_64.rpm
[9]: https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-1.fc29.i686.rpm
[10]: https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-1.fc29.x86_64.rpm
[11]: https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-1.fc30.i686.rpm
[12]: https://repos.codelite.org/rpms-12.0/fedora/codelite-12.0-1.fc30.x86_64.rpm
[13]: https://repos.codelite.org/rpms-12.0/suse/codelite-12.0-1.suse42.3.x86_64.rpm
[14]: https://repos.codelite.org/rpms-12.0/suse/codelite-12.0-1.suse15.x86_64.rpm
