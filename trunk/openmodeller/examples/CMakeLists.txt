SET(EXAMPLE_DATA
  furcata_boliviana.txt
  model_request.xml
  projection_request.xml
  rain_coolest.tif
  README
  request.txt
  temp_avg.tif
)

INSTALL(FILES ${EXAMPLE_DATA} DESTINATION ${OM_EXAMPLES_DIR})
