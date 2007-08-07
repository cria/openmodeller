export DEBFULLNAME="Tim Sutton"
export DEBEMAIL=tim@linfiniti.com
dch -v 0.5.0+svn`date +%Y%m%d`
fakeroot dpkg-buildpackage
