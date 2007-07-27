#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

TEMPLATE=subdirs
SUBDIRS=bioclim.pro \
        distance_to_average.pro \
        minimum_distance.pro \
        environmental_distance.pro

SUBDIRS+=aquamaps.pro
#bioclim_distance.pro 
SUBDIRS+=csm.pro
SUBDIRS+=dg_garp
SUBDIRS+=best_subsets
SUBDIRS+=garp
#SUBDIRS+=svm
