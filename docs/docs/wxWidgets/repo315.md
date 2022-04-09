# wxWidgets 3.1 Packages and Repositories
---

## Preface
---

**This page is for the previous development release, wx3.1.5. The latest release is currently [wx3.1.6](https://docs.codelite.org/wxWidgets/repo316/).**  
  
The CodeLite packages for Linux now use wxWidgets 3.1. As wx3.1 is a development branch, few distros will supply it. 
I've therefore built 3.1.5 packages for the commonest ones. 

Since wx3.1.1 I had built both against GTK+2 and GTK+3. As GTK+3 build have become more reliable, and in view of multiple issues getting the two build-types to co-exist, for wx3.1.5 I've created only GTK+3 packages. That itself risked causing problems, especially for debian-based distros. I have therefore made them 'conflict' with wx3.1.4 GTK+3 builds, and they also should replace wx3.1.4 GTK+2 ones.
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
debian	| `https://repos.codelite.org/wx3.1.5/debian/` | buster bullseye | libs
ubuntu | `https://repos.codelite.org/wx3.1.5/ubuntu/` | focal groovy impish | universe

!!! Note
    The `groovy` packages also install and work in ubuntu 21.04 (hirsute)

The repositories also contain wx3.1.0 to wx3.1.4 builds (including some i386 ones) for debian stretch, and ubuntu trusty, wily to eoan

The ubuntu ones should also work on other *buntus of the same version, and on derivatives e.g. Mint. 

!!! WARNING
    For Mint 20.0 (Ulyana) the ubuntu focal dev packages don't work. That is because that Mint release puts its grep and egrep in `/bin/` instead of `/usr/bin/`. 
    As a result the `wx-config` script doesn't work properly. See [this wxForum thread][1] for possible workarounds. Mint 20.1 and later should not be affected.

Now let apt know that the repository exits. If you use an apt front-end e.g. synaptic, tell it to Add a repository, using as data the appropriate entry in the table above.
If you do things by hand, add the appropriate url to /etc/apt/sources.list. Become superuser and either use a text editor to edit `/etc/apt/sources.list`, adding a line similar to: 

```bash
deb https://repos.codelite.org/wx3.1.5/debian/ buster libs
```

or, in a terminal, do something like: 

```bash
sudo apt-add-repository 'deb https://repos.codelite.org/wx3.1.5/ubuntu/ impish universe'
```

The line to add is different for different distros/versions; choose the appropriate one: 

Distro/release | Line to append
---------------|-------------------
debian buster | `deb https://repos.codelite.org/wx3.1.5/debian/ buster libs`
debian bullseye| `deb https://repos.codelite.org/wx3.1.5/debian/ bullseye libs`
ubuntu focal| `deb https://repos.codelite.org/wx3.1.5/ubuntu/ focal universe`
ubuntu groovy and hirsute | `deb https://repos.codelite.org/wx3.1.5/ubuntu/ groovy universe`
ubuntu impish | `deb https://repos.codelite.org/wx3.1.5/ubuntu/ impish universe`

You then need update the repositories. In synaptic, click the Reload button. If you're doing things by hand, do: 

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
    Building some of the samples provided by the wx3.1-examples package may fail because of failure to detect GTK+. If so, try this compilation line:
    ```bash
    LDFLAGS=$(pkg-config --libs gtk+-3.0) CXXFLAGS=$(pkg-config --cflags gtk+-3.0) make -j$(nproc)
    ```

## Fedora and openSUSE
---
There are currently rpms available for fedora 33 and 34. Fedora 35 supplies official wx3.1.5 rpms (but our fedora 33 unofficial ones do also work). openSUSE 15 provides its own wxWidgets 3.1.1 but is unlikely to update that to 3.1.5 so I've provided these too

Distro|Release|x86_64
------|-------|------
fedora|`33` & `34`|[wx-base-unicode][2] [wxGTK31][3] [wxGTK31-devel][4] [wxGTK31-gl][5] [wxGTK31-media][6] [wxGTK31-debuginfo][7]
openSUSE|`15.2`|[libwx_baseu_net][8] [libwx_baseu][9] [libwx_baseu_xml][10] [libwx_gtk3u_adv][11] [libwx_gtk3u_aui][12] [libwx_gtk3u_core][13] [libwx_gtk3u_gl][14]  [libwx_gtk3u_html][15] [libwx_gtk3u_media][16] [libwx_gtk3u_propgrid][17] [libwx_gtk3u_qa][18] [libwx_gtk3u_ribbon][19] [libwx_gtk3u_richtext][20] [libwx_gtk3u_stc][21]  [libwx_gtk3u_webview][22] [libwx_gtk2u_xrc][23] [wxWidgets-3_1-devel][24] [wxWidgets-3_1-plugin-sound_sdlu][25]
openSUSE|`15.3`|[libwx_baseu_net][28] [libwx_baseu][29] [libwx_baseu_xml][30] [libwx_gtk3u_adv][31] [libwx_gtk3u_aui][32] [libwx_gtk3u_core][33] [libwx_gtk3u_gl][34]  [libwx_gtk3u_html][35] [libwx_gtk3u_media][36] [libwx_gtk3u_propgrid][37] [libwx_gtk3u_qa][38] [libwx_gtk3u_ribbon][39] [libwx_gtk3u_richtext][40] [libwx_gtk3u_stc][41]  [libwx_gtk3u_webview][42] [libwx_gtk2u_xrc][43] [wxWidgets-3_1-devel][44] [wxWidgets-3_1-plugin-sound_sdlu][45]

There are also source rpms for [Fedora][26] and [openSUSE][27]. The fedora srpm should also build on CentOS 8. 

Either download the required rpms and install them as usual, or download and install in one step; e.g. 

```bash
rpm -Uvh https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/wxWidgets-3_1-3.1.5-1.src.rpm
```

!!! Note
    Some of these rpms may conflict with the wx2.8 devel ones. 

 [1]: https://forums.wxwidgets.org/viewtopic.php?f=19&t=47403&p=200198#p200198
 [2]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/fedora/33/wxBase31-3.1.5-1.fc33.x86_64.rpm
 [3]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/fedora/33/wxGTK31-3.1.5-1.fc33.x86_64.rpm
 [4]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/fedora/33/wxGTK31-devel-3.1.5-1.fc33.x86_64.rpm
 [5]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/fedora/33/wxGTK31-gl-3.1.5-1.fc33.x86_64.rpm
 [6]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/fedora/33/wxGTK31-media-3.1.5-1.fc33.x86_64.rpm
 [7]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/fedora/33/wxGTK31-debuginfo-3.1.5-1.fc33.x86_64.rpm
 [8]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_baseu_net-suse15-3.1.5-0.x86_64.rpm
 [9]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_baseu-suse15-3.1.5-0.x86_64.rpm
 [10]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_baseu_xml-suse15-3.1.5-0.x86_64.rpm
 [11]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_adv-suse15-3.1.5-0.x86_64.rpm
 [12]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_aui-suse15-3.1.5-0.x86_64.rpm
 [13]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_core-suse15-3.1.5-0.x86_64.rpm
 [14]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_gl-suse15-3.1.5-0.x86_64.rpm
 [15]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_html-suse15-3.1.5-0.x86_64.rpm
 [16]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_media-suse15-3.1.5-0.x86_64.rpm
 [17]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_propgrid-suse15-3.1.5-0.x86_64.rpm
 [18]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_qa-suse15-3.1.5-0.x86_64.rpm
 [19]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_ribbon-suse15-3.1.5-0.x86_64.rpm
 [20]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_richtext-suse15-3.1.5-0.x86_64.rpm
 [21]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_stc-suse15-3.1.5-0.x86_64.rpm
 [22]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_webview-suse15-3.1.5-0.x86_64.rpm
 [23]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/libwx_gtk3u_xrc-suse15-3.1.5-0.x86_64.rpm
 [24]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/wxWidgets-3_1-devel-3.1.5-0.x86_64.rpm
 [25]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.2/wxWidgets-3_1-plugin-sound_sdlu-3_1-3.1.5-0.x86_64.rpm
 
 [26]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/fedora/wxGTK31-3.1.5-1.fc.src.rpm
 [27]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/wxWidgets-3_1-3.1.5-1.src.rpm
 
 [28]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_baseu_net-suse15-3.1.5-0.x86_64.rpm
 [29]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_baseu-suse15-3.1.5-0.x86_64.rpm
 [30]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_baseu_xml-suse15-3.1.5-0.x86_64.rpm
 [31]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_adv-suse15-3.1.5-0.x86_64.rpm
 [32]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_aui-suse15-3.1.5-0.x86_64.rpm
 [33]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_core-suse15-3.1.5-0.x86_64.rpm
 [34]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_gl-suse15-3.1.5-0.x86_64.rpm
 [35]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_html-suse15-3.1.5-0.x86_64.rpm
 [36]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_media-suse15-3.1.5-0.x86_64.rpm
 [37]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_propgrid-suse15-3.1.5-0.x86_64.rpm
 [38]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_qa-suse15-3.1.5-0.x86_64.rpm
 [39]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_ribbon-suse15-3.1.5-0.x86_64.rpm
 [40]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_richtext-suse15-3.1.5-0.x86_64.rpm
 [41]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_stc-suse15-3.1.5-0.x86_64.rpm
 [42]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_webview-suse15-3.1.5-0.x86_64.rpm
 [43]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/libwx_gtk3u_xrc-suse15-3.1.5-0.x86_64.rpm
 [44]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/wxWidgets-3_1-devel-3.1.5-0.x86_64.rpm
 [45]: https://repos.codelite.org/wx3.1.5/wx3.1-packages/suse/15.3/wxWidgets-3_1-plugin-sound_sdlu-3_1-3.1.5-0.x86_64.rpm

 
