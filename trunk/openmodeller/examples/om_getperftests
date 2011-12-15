#!/bin/sh

RASTERS_SITE="biogeo.ucdavis.edu"
RASTERS_DIR="/data/climate/worldclim/1_4/grid/cur/"
POINTS_SITE="openmodeller.cria.org.br"
POINTS_DIR="/download/examples/"
LOCAL_DIR="perftests/"

ENV_LAYER1="tmin_30s_esri.zip"
ENV_LAYER2="tmax_30s_esri.zip"
ENV_LAYER3="prec_30s_esri.zip"
POINTS="points_example_1.txt"
REQUEST="request_example_1.txt"

package1=wget
package2=unzip

echo "Checking for utilities needed:"
echo -n $package1...
if [ `whereis $package1` ]; then
    echo " OK."
else
    echo " not found."
    echo " Please install $package1 and try again."
    exit 1
fi

echo -n $package2...
if [ `whereis $package2` ]; then
    echo " OK."
else
    echo " not found."
    echo " Please install $package2 and try again."
    exit 1
fi

echo
echo "Downloading files..."

wget -c -P $LOCAL_DIR                                 \
    http://${RASTERS_SITE}${RASTERS_DIR}${ENV_LAYER1} \
    http://${RASTERS_SITE}${RASTERS_DIR}${ENV_LAYER2} \
    http://${RASTERS_SITE}${RASTERS_DIR}${ENV_LAYER3} \
    http://${POINTS_SITE}${POINTS_DIR}${POINTS}       \
    http://${POINTS_SITE}${POINTS_DIR}${REQUEST}

echo "Extracting layer 1..."
unzip -d${LOCAL_DIR} ${LOCAL_DIR}${ENV_LAYER1}
echo "Extracting layer 2..."
unzip -d${LOCAL_DIR} ${LOCAL_DIR}${ENV_LAYER2}
echo "Extracting layer 3..."
unzip -d${LOCAL_DIR} ${LOCAL_DIR}${ENV_LAYER3}

echo "DONE."
