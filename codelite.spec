Name:           codelite
Version:        2.5.0
Release:        1%{?dist}
License:        GPLv2+
Group:          Development/Tools
Summary:        CodeLite is a powerful open-source, cross platform code editor for C/C++
URL:            http://codelite.sourceforge.net
Source:         http://downloads.sourceforge.net/%{name}/%{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Requires: wxGTK xterm
BuildRequires: wxGTK-devel

%description
CodeLite uses a sophisticated, yet intuitive interface which allows
users to easily create, build and debug complex projects.

%prep
%setup -q

%build
./configure --prefix=%{_prefix} --plugins-dir=%{_libdir}/%{name} \
               --disable-debian --disable-desktop_icon --disable-make_symlink
%{__make}

%install
%{__rm} -rf $RPM_BUILD_ROOT
%{__make} DESTDIR=$RPM_BUILD_ROOT install
desktop-file-install  --delete-original                            \
   --vendor codelite                                               \
          --dir $RPM_BUILD_ROOT%{_datadir}/applications            \
                $RPM_BUILD_ROOT%{_datadir}/applications/codelite.desktop
%clean
%{__rm} -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS LICENSE COPYING
%{_bindir}/codelite
%{_bindir}/codelite_indexer
%{_bindir}/codelite_fix_files
%{_bindir}/codelite_exec
%{_bindir}/codelite_kill_children
%{_datadir}/codelite
%{_datadir}/applications/codelite.desktop
%{_libdir}/%{name}

%changelog
* Tue Feb 24 2009 DH
- Spec file: Corrected names. Disabled unwanted things in configure
* Tue Feb 24 2009 Jess Portnoy <kernel01@gmail.com> 1.0.2782-1
- Spec file: Added call to desktop-file-install and %doc
  code: fixed perms and other rpmlint issues.
* Sat Feb 21 2009 Jess Portnoy <kernel01@gmail.com> 1.0.2781-1
- Reworked the rpm package to satisfy Fedora Core conventions.
* Wed Jan 27 2010 Jess Portnoy <kernel01@gmail.com>
- Removed obsolete switch (--copy-generic-name-to-name)
  updated version number to 2.2.0
  updated version number to 2.5.0

