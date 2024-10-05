
# add_custom_target(PREBUILDER ALL)
# add_custom_command(TARGET PREBUILDER
#                 PRE_BUILD
#                 COMMAND sh ${PROJECT_SOURCE_DIR}/src/realtime/smartctl/configure
#                 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/realtime/smartctl)

add_subdirectory(disk)
add_subdirectory(systeminfo)
add_subdirectory(systemtime)
add_subdirectory(hardware)
add_subdirectory(proc)
add_subdirectory(packages)
add_subdirectory(filesystem)
add_subdirectory(powermanagement)
add_subdirectory(kdkocr)
add_subdirectory(location)
add_subdirectory(logrotate)
add_subdirectory(net)
add_subdirectory(realtime)
add_subdirectory(lingmosdkdbus)
add_subdirectory(lingmo-sdk-system-javascript-http)
add_subdirectory(lingmo-sdk-system-javascript-websocket)
#add_subdirectory(processmanage)
