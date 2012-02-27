#!/bin/sh

RASTERS_SITE="biogeo.ucdavis.edu"
RASTERS_DIR="/data/climate/worldclim/1_4/grid/cur/"
OM_SITE="openmodeller.cria.org.br"
EXAMPLES_DIR="/download/examples/"
LOCAL_DIR="perftests/"

ENV_LAYER1="tmin_30s_esri.zip"
ENV_LAYER2="tmax_30s_esri.zip"
ENV_LAYER3="prec_30s_esri.zip"
ALL_POINTS="all_points_1.txt"
CONSOLE_REQUEST="req_1.txt"
TEST_POINTS="test_1.txt"
CONSOLE_REQUEST="req_1.txt"
MODEL_REQUEST="model_req_1.xml"

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

wget -c -P $LOCAL_DIR                                  \
    http://${RASTERS_SITE}${RASTERS_DIR}${ENV_LAYER1}  \
    http://${RASTERS_SITE}${RASTERS_DIR}${ENV_LAYER2}  \
    http://${RASTERS_SITE}${RASTERS_DIR}${ENV_LAYER3}  \
    http://${OM_SITE}${EXAMPLES_DIR}${ALL_POINTS}      \
    http://${OM_SITE}${EXAMPLES_DIR}${TEST_POINTS}     \
    http://${OM_SITE}${EXAMPLES_DIR}${CONSOLE_REQUEST} \
    http://${OM_SITE}${EXAMPLES_DIR}${MODEL_REQUEST}

echo "Extracting layer 1..."
unzip -d${LOCAL_DIR} ${LOCAL_DIR}${ENV_LAYER1}
echo "Extracting layer 2..."
unzip -d${LOCAL_DIR} ${LOCAL_DIR}${ENV_LAYER2}
echo "Extracting layer 3..."
unzip -d${LOCAL_DIR} ${LOCAL_DIR}${ENV_LAYER3}

echo "DONE."
