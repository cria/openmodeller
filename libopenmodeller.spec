%define prefix /usr

Name:          libopenmodeller
License:       GPL
Group:         Libraries/Research
Autoreqprov:   on
Version:       0.6.0
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
Requires:   expat
Requires:   proj >= 4.5.0
Requires:   gdal >= 1.4.2
Requires:   libcurl >= 7.15.4
Requires:   sqlite >= 3.3.6
Requires:   gsl >= 1.4
Requires:   boost >= 1.31
Requires:   gcc-g77

%description devel
Include files and libraries to link programs against openModeller
or develop new algorithms. 

%package -n openmodeller-tools
Group:      Applications/Research
Summary:    Potential Distribution Modelling Library - Console Tools
Requires:   %{name} >= %{version}
Requires:   X11-libs

%description -n openmodeller-tools
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
rm -rf $RPM_BUILD_ROOT
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_libdir}/libopenmodeller.so.*
%dir %{_libdir}/openmodeller
%{_libdir}/openmodeller/*so*
%doc AUTHORS COPYING.txt ChangeLog README.txt INSTALL NEWS

%files devel
%defattr(-,root,root)
%{_libdir}/libopenmodeller.*a
%{_libdir}/libopenmodeller.so
%{_libdir}/openmodeller/*a
%dir %{prefix}/include/openmodeller
%{prefix}/include/openmodeller

%files -n openmodeller-tools
%defattr(-,root,root)
/usr/bin/*

%changelog -n libopenmodeller
* Mon Jun 23 2008 - Renato De Giovanni <renato [at] cria . org . br>
- renamed openmodeller package to openmodeller-tools
- replaced automake commands by cmake ones
* Mon May 13 2008 - Renato De Giovanni <renato [at] cria . org . br>
- removed libopenmodellerxml files
* Mon Nov 27 2006 - Renato De Giovanni <renato [at] cria . org . br>
- included missing libopenmodellerxml files
* Thu Jul 21 2005 - Jens Oberender <j.obi@troja.net>
- renamed to libopenmodeller
- splitted up openmodeller for the tools
* Wed Mar 24 2004 - Jens Oberender <j.obi@troja.net>
- initial build
