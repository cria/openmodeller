# Added by Tim to install sample data

SET(AQUAMAPS_DATA
  aquamaps.db
)

INSTALL(FILES ${AQUAMAPS_DATA} DESTINATION ${OM_DATA_DIR}/data/)
