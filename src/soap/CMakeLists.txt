SUBDIRS (1.0 2.0)

########################################################
# Install

INSTALL(CODE "MESSAGE(\"Installing shared data...\")")
INSTALL(FILES server.conf DESTINATION ${OM_CONFIG_DIR})
INSTALL(FILES ctables/byte.xml DESTINATION ${OM_CONFIG_DIR}/ctables)
INSTALL(FILES scheduler.sh DESTINATION ${OM_CONFIG_DIR})
