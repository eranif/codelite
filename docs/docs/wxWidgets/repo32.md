# wxWidgets 3.2 Packages and Repositories
---

## Preface
---

This page is for the stable release, wx3.2.2.1. This is a replacement for wx3.2.2.0, which was soon found to have a bug.

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
  
  
!!! Tip
  Since ubuntu 21.10, using apt-key displays the following message:
  ```bash
  Warning: apt-key is deprecated. Manage keyring files in trusted.gpg.d instead (see apt-key(8))
  ```
  Don't panic! it's only a warning. apt-key still works.
  
    
## Ubuntu and debian
---

If you use a debian-based distro there are the following wxWidgets repositories available: 

Repository | Available | Versions | Component
-----------|-----------|----------|-----------
debian	| `https://repos.codelite.org/wx3.2/debian/` | bullseye | libs
ubuntu | `https://repos.codelite.org/wx3.2/ubuntu/` | jammy kinetic | universe


The repositories also contain wx3.1.0 to wx3.2.1 builds (including some i386 ones) for debian stretch and buster, and ubuntu trusty, wily to hirsute.

The ubuntu ones should also work on other *buntus of the same version, and on derivatives e.g. Mint.

Now let apt know that the repository exits. If you use an apt front-end e.g. synaptic, tell it to Add a repository, using as data the appropriate entry in the table above. If you do things by hand, add the appropriate url to /etc/apt/sources.list. Become superuser and either use a text editor to edit /etc/apt/sources.list, adding a line similar to:

```bash
deb https://repos.codelite.org/wx3.2/debian/ bullseye libs
```

or, in a terminal, do something like: 

```bash
sudo apt-add-repository 'deb https://repos.codelite.org/wx3.2/ubuntu/ jammy universe'
```

The line to add is different for different distros/versions; choose the appropriate one: 

Distro/release | Line to append
---------------|-------------------
debian bullseye| `deb https://repos.codelite.org/wx3.2/debian/ bullseye libs`
ubuntu jammy | `deb https://repos.codelite.org/wx3.2/ubuntu/ jammy universe`
ubuntu kinetic| `deb https://repos.codelite.org/wx3.2/ubuntu/ kinetic universe`

You then need to update the repositories. In synaptic, click the Reload button. If you're doing things by hand, do: 

```bash
sudo apt-get update
```

Synaptic users can then select and install as usual. By hand you can do: 

```bash
apt-get install libwxbase3.2-0-unofficial \
                libwxbase3.2unofficial-dev \
                libwxgtk3.2-0-unofficial \
                libwxgtk3.2unofficial-dev \
                wx3.2-headers \
                wx-common \
                libwxgtk-media3.2-0-unofficial \
                libwxgtk-media3.2unofficial-dev \
                libwxgtk-webview3.2-0-unofficial \ 
                libwxgtk-webview3.2unofficial-dev \ 
                libwxgtk-webview3.2-0-unofficial-dbg \ 
                libwxbase3.2-0-unofficial-dbg \
                libwxgtk3.2-0-unofficial-dbg \
                libwxgtk-media3.2-0-unofficial-dbg \
                wx3.2-i18n \
                wx3.2-examples
```

The first 6 of the debs are required; the others optional (and of decreasing importance). 

`wx-common` and `wx3.2-i18n` will conflict with the distro's wx2.8 equivalents. That is unlikely to matter too much; 
but `wx-common` contains `wxrc` so, if you use this, be aware that there may be differences after upgrading. 


!!! Tip
    Building some of the samples provided by the wx3.2-examples package may fail because of failure to detect GTK+ files. If so, try this compilation line:  
    ```bash
    LDFLAGS=$(pkg-config --libs gtk+-3.0) CXXFLAGS=$(pkg-config --cflags gtk+-3.0) make -j$(nproc)
    ```  
    If all else fails, you might need to create symlinks.

## Fedora and openSUSE
---
There are currently rpms available for fedora 37, and openSUSE 15.4 and 15.5.

Distro|Release|x86_64
------|-------|------
fedora|`37`|[wxBase32][2] [wxBase32-debuginfo][81] [wxGTK32][3] [wxGTK32-debuginfo][83] [wxGTK32-debugsource][82] [wxGTK32-devel][4] [wxGTK32-devel-debuginfo][83] [wxGTK32-gl][5] [wxGTK32-gl-debuginfo][84] [wxGTK32-media][6] [wxGTK32-media-debuginfo][85]
openSUSE|`15.4`|[libwx_baseu_net][8] [libwx_baseu][9] [libwx_baseu_xml][10] [libwx_gtk3u_adv][11] [libwx_gtk3u_aui][12] [libwx_gtk3u_core][13] [libwx_gtk3u_gl][14]  [libwx_gtk3u_html][15] [libwx_gtk3u_media][16] [libwx_gtk3u_propgrid][17] [libwx_gtk3u_qa][18] [libwx_gtk3u_ribbon][19] [libwx_gtk3u_richtext][20] [libwx_gtk3u_stc][21]  [libwx_gtk3u_xrc][23] [wxWidgets-3_2-devel][24] 
openSUSE|`15.5`|[libwx_baseu_net][28] [libwx_baseu][29] [libwx_baseu_xml][30] [libwx_gtk3u_adv][31] [libwx_gtk3u_aui][32] [libwx_gtk3u_core][33] [libwx_gtk3u_gl][34]  [libwx_gtk3u_html][35] [libwx_gtk3u_media][36] [libwx_gtk3u_propgrid][37] [libwx_gtk3u_qa][38] [libwx_gtk3u_ribbon][39] [libwx_gtk3u_richtext][40] [libwx_gtk3u_stc][41]  [libwx_gtk3u_xrc][42] [libwx_gtk3u_webview][43] [libwx_webkit2_ext][44] [wxWidgets-3_2-devel][45] 

There are also source rpms for [Fedora][91] and [openSUSE][92]. The fedora srpm should also build on CentOS 8. 

Either download the required rpms and install them as usual, or download and install in one step; e.g. 

```bash
rpm -Uvh https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/wxWidgets-3_2-3.2.2.1-1.src.rpm
```

!!! Note
    Some of these rpms may conflict with the wx2.8 devel ones. 

 [1]: https://forums.wxwidgets.org/viewtopic.php?f=19&t=47403&p=200198#p200198
 [2]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxBase32-3.2.2.1-1.fc37.x86_64.rpm
 [3]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxGTK32-3.2.2.1-1.fc37.x86_64.rpm
 [4]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxGTK32-devel-3.2.2.1-1.fc37.x86_64.rpm
 [5]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxGTK32-gl-3.2.2.1-1.fc37.x86_64.rpm
 [6]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxGTK32-media-3.2.2.1-1.fc37.x86_64.rpm
 
 [8]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_baseu_net-suse15-3.2.2.1-0.x86_64.rpm
 [9]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_baseu-suse15-3.2.2.1-0.x86_64.rpm
 [10]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_baseu_xml-suse15-3.2.2.1-0.x86_64.rpm
 [11]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_adv-suse15-3.2.2.1-0.x86_64.rpm
 [12]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_aui-suse15-3.2.2.1-0.x86_64.rpm
 [13]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_core-suse15-3.2.2.1-0.x86_64.rpm
 [14]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_gl-suse15-3.2.2.1-0.x86_64.rpm
 [15]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_html-suse15-3.2.2.1-0.x86_64.rpm
 [16]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_media-suse15-3.2.2.1-0.x86_64.rpm
 [17]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_propgrid-suse15-3.2.2.1-0.x86_64.rpm
 [18]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_qa-suse15-3.2.2.1-0.x86_64.rpm
 [19]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_ribbon-suse15-3.2.2.1-0.x86_64.rpm
 [20]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_richtext-suse15-3.2.2.1-0.x86_64.rpm
 [21]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_stc-suse15-3.2.2.1-0.x86_64.rpm
 [22]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_webview-suse15-3.2.2.1-0.x86_64.rpm
 [23]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/libwx_gtk3u_xrc-suse15-3.2.2.1-0.x86_64.rpm
 [24]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/wxWidgets-3_2-devel-3.2.2.1-0.x86_64.rpm
 [25]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.4/wxWidgets-3_2-plugin-sound_sdlu-3_2-3.2.2.1-0.x86_64.rpm
 
 [28]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_baseu_net-suse15-3.2.2.1-0.x86_64.rpm
 [29]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_baseu-suse15-3.2.2.1-0.x86_64.rpm
 [30]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_baseu_xml-suse15-3.2.2.1-0.x86_64.rpm
 [31]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_adv-suse15-3.2.2.1-0.x86_64.rpm
 [32]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_aui-suse15-3.2.2.1-0.x86_64.rpm
 [33]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_core-suse15-3.2.2.1-0.x86_64.rpm
 [34]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_gl-suse15-3.2.2.1-0.x86_64.rpm
 [35]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_html-suse15-3.2.2.1-0.x86_64.rpm
 [36]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_media-suse15-3.2.2.1-0.x86_64.rpm
 [37]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_propgrid-suse15-3.2.2.1-0.x86_64.rpm
 [38]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_qa-suse15-3.2.2.1-0.x86_64.rpm
 [39]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_ribbon-suse15-3.2.2.1-0.x86_64.rpm
 [40]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_richtext-suse15-3.2.2.1-0.x86_64.rpm
 [41]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_stc-suse15-3.2.2.1-0.x86_64.rpm
 [42]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_xrc-suse15-3.2.2.1-0.x86_64.rpm
 [43]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_gtk3u_webview-suse15-3.2.2.1-0.x86_64.rpm
 [44]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/libwx_webkit2_extu-suse-3.2.2.1-0.x86_64.rpm
 [45]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/wxWidgets-3_2-devel-3.2.2.1-0.x86_64.rpm
 [45]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/15.5/wxWidgets-3_2-plugin-sound_sdlu-3_2-3.2.2.1-0.x86_64.rpm
   
 [80]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxGTK32-debuginfo-3.2.2.1-1.fc37.x86_64.rpm
 [81]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxBase32-debuginfo-3.2.2.1-1.fc37.x86_64.rpm
 [82]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxGTK32-debugsource-3.2.2.1-1.fc37.x86_64.rpm
 [83]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxGTK32-devel-debuginfo-3.2.2.1-1.fc37.x86_64.rpm
 [84]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxGTK32-gl-debuginfo-3.2.2.1-1.fc37.x86_64.rpm
 [85]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/37/wxGTK32-media-debuginfo-3.2.2.1-1.fc37.x86_64.rpm
 
 [91]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/fedora/wxGTK32-3.2.2.1-1.fc.src.rpm
 [92]: https://repos.codelite.org/wx3.2.2.1/wx3.2-packages/suse/wxWidgets-3_2-3.2.2.1-1.src.rpm
 
