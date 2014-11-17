%define prefix /usr

Name:          libopenmodeller
License:       GPL
Group:         Libraries/Research
Autoreqprov:   on
#Requires:   expat
#Requires:   proj >= 4.5.0
#Requires:   gdal >= 1.4.2
#Requires:   libcurl >= 7.18.1
#Requires:   sqlite >= 3.3.6
#Requires:   gsl >= 1.4
Version:       1.5.0
Release:       0
Source:        %{name}-src-%{version}.tar.gz
Url:           http://openmodeller.sf.net
Summary:       Library for Potential Distribution Modelling
BuildRoot:     %{_tmppath}/%{name}-%{version}-build

%description
openModeller is an open source C++ library to determine potential 
distribution in environment and geographic space. Models are generated 
by algorithms that receive as input a set of occurrence points 
(latitude/longitude/abundance) and a set of raster files with environment
data. Models can then be projected to generate distribution maps. 
Available algorithms include Bioclim, GARP Best Subsets, Support Vector 
Machines, Climate Space Model, Environmental Distance, AquaMaps and
Maximum Entropy. The library uses GDAL to read different raster formats 
and proj4 to convert between different coordinate systems and projections. 

%package devel
Group:      Development/Libraries/C and C++
Summary:    Potential Distribution Modelling Library - Development Files
Requires:   %{name} >= %{version}

%description devel
Include files and libraries to link programs against openModeller
or develop new algorithms. 

%package -n openmodeller
Group:      Applications/Research
Summary:    Potential Distribution Modelling Library - Console Tools
Requires:   %{name} >= %{version}
#Requires:   X11-libs

%description -n openmodeller
Console and command-line tools for using openModeller.

%define srcdirname %{name}-src-%{version}

%prep
%setup -q -n %{srcdirname}

%build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=%{prefix} -DPEDANTIC=OFF ../
make

%install
cd build
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_libdir}/libopenmodeller.so*
%{_libdir}/openmodeller
%doc AUTHORS COPYING.txt ChangeLog README.txt INSTALL NEWS
%{prefix}/share/openmodeller/data

%files devel
%defattr(-,root,root)
%{prefix}/include/openmodeller

%files -n openmodeller
%defattr(-,root,root)
%{_bindir}/om_*
%{prefix}/man/man1/*
%{prefix}/share/openmodeller/examples
#/etc/openmodeller/scheduler.sh
#/etc/openmodeller/server.conf
#/etc/openmodeller/vrts/bytehfa.vrt

%changelog -n libopenmodeller
* Thu Oct 30 2008 - Renato De Giovanni <renato [at] cria . org . br>
- do not install files related to the soap server (should be a new separate package in the future)
* Mon Sep 22 2008 - Renato De Giovanni <renato [at] cria . org . br>
- install virtual raster template file
* Mon Jun 23 2008 - Renato De Giovanni <renato [at] cria . org . br>
- replaced automake commands by cmake ones
- included data dir in libopenmodeller package
- included man pages and examples dir in openmodeller package
- fixed devel package
* Mon May 13 2008 - Renato De Giovanni <renato [at] cria . org . br>
- removed libopenmodellerxml files
* Mon Nov 27 2006 - Renato De Giovanni <renato [at] cria . org . br>
- included missing libopenmodellerxml files
* Thu Jul 21 2005 - Jens Oberender <j.obi@troja.net>
- renamed to libopenmodeller
- splitted up openmodeller for the tools
* Wed Mar 24 2004 - Jens Oberender <j.obi@troja.net>
- initial build
