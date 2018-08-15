%define _srcname      ime-wrapper
%define _name         %{_srcname}
%if !%{defined _version}
%define _version      1
%endif
%define _release      0
%define _sub_release  0
%define _group        System Environment/Base

%define _prefix       /opt/ddn/ime
%define _pkgdoc       %{_prefix}/share/%{_name}
%define _libdir       %{_prefix}/lib
%define _mandir       %{_prefix}/share/man

%define is_fedora     %(test -e /etc/fedora-release && echo 1 || echo 0)
%define is_redhat     %(test -e /etc/redhat-release && echo 1 || echo 0)
%define is_suse       %(test -e /etc/SuSE-release && echo 1 || echo 0)
%define is_mandrake   %(test -e /etc/mandrake-release && echo 1 || echo 0)

# Detect if systemd is used for the services
%define is_systemd  %(rpm -qf /sbin/init | grep -q "systemd" && echo 1 || echo 0)

%if %is_mandrake
%define disttag mdk
%endif

%if %is_suse
%define disttag suse
%define distver %(relpackage="`rpm -qf /etc/SuSE-release`"; release="`rpm -q --queryformat='%{VERSION}' $relpackage 2> /dev/null | tr . : | sed s/:.*$//g`" ; if test $? != 0 ; then release="" ; fi ; echo "$release")
%endif

%if %is_fedora
%define disttag fc
%endif

%if %is_redhat
%define disttag el
%define distver %(relpackage="`rpm -qf /etc/redhat-release`"; release="`rpm -q --queryformat='%{VERSION}' $relpackage 2> /dev/null | tr . : | sed s/:.*$//g`" ; if test $? != 0 ; then release="" ; fi ; echo "$release")
%endif

%if %{defined disttag}
%define dist %{disttag}%{distver}
%else
%define dist generic
%endif

Summary:            Infinite Memory Engine POSIX Wrapper
Name:               %{_name}
Version:            %{_version}
Release:            %{_release}.%{dist}
Source:             %{_srcname}-%{_version}.tar.gz
Vendor:             DataDirect Networks Inc.
URL:                http://www.ddn.com
Group:              %{_group}
Packager:           IME Development Team <support@ddn.com>
License:            Proprietary - COPYRIGHT DataDirect Networks, Inc. 2018
BuildRoot:          %{_tmppath}/build-%{_name}-%{_version}
Prefix:             %{_prefix}
Distribution:       IME POSIX Wrapper

%description
This is a meta package that can be safely removed after the update
Revision: %{_version}-%{_release}-%{_sub_release}-%{dist}

%prep
%setup -q -n "%{_srcname}-%{_version}"

%{__make} clean
%{__make} %{?_smp_mflags} V=1

%install
%{__rm} -rf "${RPM_BUILD_ROOT}"
%{__mkdir} -p "${RPM_BUILD_ROOT}"
%{__make} %{?_smp_mflags} \
    DESTDIR="${RPM_BUILD_ROOT}" \
    install

%clean
%{__rm} -rf "${RPM_BUILD_ROOT}"

%files
%defattr(-,root,root)
%{_libdir}/libim_client.*
%{_includedir}/*

%changelog
* Tue Aug 14 2018 IME Development Team <support@ddn.com>
- initial packaging
