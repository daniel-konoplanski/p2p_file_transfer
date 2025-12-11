add_subdirectory(connection_manager)
add_subdirectory(message_sender)

add_library(comms STATIC
    $<TARGET_OBJECTS:connection_manager>
    $<TARGET_OBJECTS:message_sender>)
