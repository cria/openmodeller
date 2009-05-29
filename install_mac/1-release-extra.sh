#!/bin/sh
# Copy supporting libraries to openModeller bundle
# and make search paths for them relative to bundle

APP_PREFIX=/Applications/openmodeller.app/Contents/MacOS
APP_LIB_DIR=${APP_PREFIX}/lib
APP_BIN_DIR=${APP_PREFIX}/bin
LIB_DIR=/usr/local/om_universal_deps/lib
set -x
cp -r ${LIB_DIR}/libexpat.dylib  ${APP_LIB_DIR}
cp -r ${LIB_DIR}/libgdal.1.dylib  ${APP_LIB_DIR}
cp -r ${LIB_DIR}/libgeos.dylib  ${APP_LIB_DIR}
cp -r ${LIB_DIR}/libproj.dylib  ${APP_LIB_DIR}
cp -r ${LIB_DIR}/libgsl.0.dylib ${APP_LIB_DIR}
cp -r ${LIB_DIR}/libgslcblas.0.dylib ${APP_LIB_DIR}
cp -r ${LIB_DIR}/libsqlite3.0.dylib  ${APP_LIB_DIR}
pushd .
cd ${APP_LIB_DIR}
ln -s libgdal.1.dylib libgdal.dylib
ln -s libgsl.0.dylib libgsl.dylib
ln -s libgslcblas.0.dylib libgslcblas.dylib
ln -s libsqlite3.0.dylib libsqlite3.dylib
popd

LIBS="libexpat.1.dylib \
      libgsl.0.dylib \
      libgsl.0.dylib \
      libgslcblas.0.dylib \
      libopenmodeller.dylib \
      libexpat.dylib \
      libgdal.dylib \
      libgeos.dylib \
      libgeos_c.dylib \
      libproj.dylib \
      libsqlite3.dylib \
      openmodeller/libaquamaps.so \
      openmodeller/libdesktop_garp_bs.so \
      openmodeller/libgarp_best_subsets.so \
      openmodeller/libbioclim.so \
      openmodeller/libenvelope_score.so \
      openmodeller/libomneuralnet.so \
      openmodeller/libcsm_bs.so \
      openmodeller/libenvironmental_distance.so \
      openmodeller/libdesktop_garp.so \
      openmodeller/libgarp.so \
      openmodeller/libomsvm.so"

#
# update the library ids and paths for these libs
#
for LIB in $LIBS
do
  install_name_tool -id @executable_path/../lib/$LIB ${APP_LIB_DIR}/${LIB}
  echo "install_name_tool -id @executable_path/lib/$LIB ${APP_LIB_DIR}/${LIB}"
  for LIBPATH in `otool -L ${APP_LIB_DIR}/${LIB} \
                  | sed 's/(\([a-zA-Z0-9\., ]*\))//g' \
                  | grep  $LIB_DIR`
  do 
    #echo $LIBPATH 
    BASELIB=`basename "$LIBPATH"`
    #echo $BASELIB
    install_name_tool -change ${LIBPATH} \
                      @executable_path/../lib/${BASELIB} \
                      ${APP_LIB_DIR}/${LIB}
  done
  install_name_tool -change libopenmodeller.dylib \
                    @executable_path/../lib/libopenmodeller.dylib \
                    ${APP_LIB_DIR}/${LIB}
  otool -L ${APP_LIB_DIR}/${LIB}
  echo ${LIB}
done

BINS="om_console \
      om_model \
      om_project \
      om_algorithm \
      om_points \
      om_pseudo \
      om_sampler \
      om_test"

#
# and update the library ids and paths for these libs
#
for BIN in $BINS
do
  echo "install_name_tool -id @executable_path/lib/$LIB ${APP_LIB_DIR}/${LIB}"
  for LIBPATH in `otool -L ${APP_BIN_DIR}/${BIN} \
                  | sed 's/(\([a-zA-Z0-9\., ]*\))//g' \
                  | grep  $LIB_DIR`
  do 
    #echo $LIBPATH 
    BASELIB=`basename "$LIBPATH"`
    #echo $BASELIB
    install_name_tool -change ${LIBPATH} \
                      @executable_path/../lib/${BASELIB} \
                      ${APP_BIN_DIR}/${BIN}
  done
  install_name_tool -change libopenmodeller.dylib \
                    @executable_path/../lib/libopenmodeller.dylib \
                    ${APP_BIN_DIR}/${BIN}
  otool -L ${APP_BIN_DIR}/${BIN}
  echo ${LIB}
done
