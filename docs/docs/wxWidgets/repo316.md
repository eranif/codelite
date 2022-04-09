# wxWidgets 3.1 Packages and Repositories
---

## Preface
---

This page is for the latest development release, wx3.1.6. The CodeLite packages for Linux now use wxWidgets 3.1. As wx3.1 is a development branch, few distros will supply it.  I've therefore built 3.1.6 packages for the commonest ones. 

Since wx3.1.1 I had built both against GTK+2 and GTK+3. As GTK+3 build have become more reliable, and in view of multiple issues getting the two build-types to co-exist, since wx3.1.5 I've created only GTK+3 packages. 
Also starting with wx3.1.5, the openSUSE rpms are built only against GTK+3. The fedora ones remain GTK+3, as they have been since wx3.1.0.

## Fetching CodeLite keys
---

The packages are signed with the CodeLite public key so, if you haven't done so before, you should tell apt or rpm about this by doing as superuser (su or sudo depending on your distro): 

```bash
apt-key adv --fetch-keys https://repos.codelite.org/CodeLite.asc
```

or

```bash
rpm --import https://repos.codelite.org/CodeLite.asc
```

## Ubuntu and debian
---

If you use a debian-based distro there are the following wxWidgets repositories available: 

Repository | Available | Versions | Component
-----------|-----------|----------|-----------
debian	| `https://repos.codelite.org/wx3.1.6/debian/` | bullseye | libs
ubuntu | `https://repos.codelite.org/wx3.1.6/ubuntu/` | focal impish | universe



The repositories also contain wx3.1.0 to wx3.1.5 builds (including some i386 ones) for debian stretch and buster, and ubuntu trusty, wily to hirsute.

The ubuntu ones should also work on other *buntus of the same version, and on derivatives e.g. Mint. 


Now let apt know that the repository exits. If you use an apt front-end e.g. synaptic, tell it to Add a repository, using as data the appropriate entry in the table above.
If you do things by hand, add the appropriate url to /etc/apt/sources.list. Become superuser and either use a text editor to edit `/etc/apt/sources.list`, adding a line similar to: 

```bash
deb https://repos.codelite.org/wx3.1.6/debian/ bullseye libs
```

or, in a terminal, do something like: 

```bash
sudo apt-add-repository 'deb https://repos.codelite.org/wx3.1.6/ubuntu/ impish universe'
```

The line to add is different for different distros/versions; choose the appropriate one: 

Distro/release | Line to append
---------------|-------------------
debian bullseye| `deb https://repos.codelite.org/wx3.1.6/debian/ bullseye libs`
ubuntu focal| `deb https://repos.codelite.org/wx3.1.6/ubuntu/ focal universe`
ubuntu impish | `deb https://repos.codelite.org/wx3.1.6/ubuntu/ impish universe`

You then need to update the repositories. In synaptic, click the Reload button. If you're doing things by hand, do: 

```bash
sudo apt-get update
```

Synaptic users can then select and install as usual. By hand you can do: 

```bash
apt-get install libwxbase3.1-0-unofficial \
                libwxbase3.1unofficial-dev \
                libwxgtk3.1-0-unofficial \
                libwxgtk3.1unofficial-dev \
                wx3.1-headers \
                wx-common \
                libwxgtk-media3.1-0-unofficial \
                libwxgtk-media3.1unofficial-dev \
                libwxgtk-webview3.1-0-unofficial \ 
                libwxgtk-webview3.1unofficial-dev \ 
                libwxgtk-webview3.1-0-unofficial-dbg \ 
                libwxbase3.1-0-unofficial-dbg \
                libwxgtk3.1-0-unofficial-dbg \
                libwxgtk-media3.1-0-unofficial-dbg \
                wx3.1-i18n \
                wx3.1-examples
```

The first 6 of the debs are required; the others optional (and of decreasing importance). 

`wx-common` and `wx3.1-i18n` will conflict with the distro's wx2.8 equivalents. That is unlikely to matter too much; 
but `wx-common` contains `wxrc` so, if you use this, be aware that there may be differences after upgrading. 


!!! Tip
    Building some of the samples provided by the wx3.1-examples package may fail because of failure to detect GTK+ files. If so, try this compilation line:  
    ```bash
    LDFLAGS=$(pkg-config --libs gtk+-3.0) CXXFLAGS=$(pkg-config --cflags gtk+-3.0) make -j$(nproc)
    ```  
    If all else fails, you might need to create symlinks.

## Fedora and openSUSE
---
There are currently rpms available for fedora 35 and openSUSE 15.3.

Distro|Release|x86_64
------|-------|------
fedora|`35`|[wx-base-unicode][2] [wxGTK31][3] [wxGTK31-devel][4] [wxGTK31-gl][5] [wxGTK31-media][6] [wxGTK31-debuginfo][7]
openSUSE|`15.3`|[libwx_baseu_net][8] [libwx_baseu][9] [libwx_baseu_xml][10] [libwx_gtk3u_adv][11] [libwx_gtk3u_aui][12] [libwx_gtk3u_core][13] [libwx_gtk3u_gl][14]  [libwx_gtk3u_html][15] [libwx_gtk3u_media][16] [libwx_gtk3u_propgrid][17] [libwx_gtk3u_qa][18] [libwx_gtk3u_ribbon][19] [libwx_gtk3u_richtext][20] [libwx_gtk3u_stc][21]  [libwx_gtk3u_webview][22] [libwx_gtk3u_xrc][23] [wxWidgets-3_1-devel][24] [wxWidgets-3_1-plugin-sound_sdlu][25]]

There are also source rpms for [Fedora][26] and [openSUSE][27]. The fedora srpm should also build on CentOS 8. 

Either download the required rpms and install them as usual, or download and install in one step; e.g. 

```bash
rpm -Uvh https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/wxWidgets-3_1-3.1.6-0.src.rpm
```

!!! Note
    Some of these rpms may conflict with the wx2.8 devel ones. 

 [1]: https://forums.wxwidgets.org/viewtopic.php?f=19&t=47403&p=200198#p200198
 [2]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/fedora/35/wxBase31-3.1.6-1.fc35.x86_64.rpm
 [3]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/fedora/35/wxGTK31-3.1.6-1.fc35.x86_64.rpm
 [4]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/fedora/35/wxGTK31-devel-3.1.6-1.fc35.x86_64.rpm
 [5]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/fedora/35/wxGTK31-gl-3.1.6-1.fc35.x86_64.rpm
 [6]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/fedora/35/wxGTK31-media-3.1.6-1.fc35.x86_64.rpm
 [7]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/fedora/35/wxGTK31-debuginfo-3.1.6-1.fc35.x86_64.rpm
 [8]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_baseu_net-suse15-3.1.6-0.x86_64.rpm
 [9]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_baseu-suse15-3.1.6-0.x86_64.rpm
 [10]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_baseu_xml-suse15-3.1.6-0.x86_64.rpm
 [11]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_adv-suse15-3.1.6-0.x86_64.rpm
 [12]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_aui-suse15-3.1.6-0.x86_64.rpm
 [13]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_core-suse15-3.1.6-0.x86_64.rpm
 [14]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_gl-suse15-3.1.6-0.x86_64.rpm
 [15]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_html-suse15-3.1.6-0.x86_64.rpm
 [16]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_media-suse15-3.1.6-0.x86_64.rpm
 [17]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_propgrid-suse15-3.1.6-0.x86_64.rpm
 [18]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_qa-suse15-3.1.6-0.x86_64.rpm
 [19]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_ribbon-suse15-3.1.6-0.x86_64.rpm
 [20]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_richtext-suse15-3.1.6-0.x86_64.rpm
 [21]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_stc-suse15-3.1.6-0.x86_64.rpm
 [22]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_webview-suse15-3.1.6-0.x86_64.rpm
 [23]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/libwx_gtk3u_xrc-suse15-3.1.6-0.x86_64.rpm
 [24]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/wxWidgets-3_1-devel-3.1.6-0.x86_64.rpm
 [25]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/15.3/wxWidgets-3_1-plugin-sound_sdlu-3_1-3.1.6-0.x86_64.rpm
 
 [26]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/fedora/wxGTK31-3.1.6-1.fc.src.rpm
 [27]: https://repos.codelite.org/wx3.1.6/wx3.1-packages/suse/wxWidgets-3_1-3.1.6-0.src.rpm
 
