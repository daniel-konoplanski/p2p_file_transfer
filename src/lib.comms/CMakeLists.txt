add_subdirectory(connection_manager)

add_library(comms STATIC
    $<TARGET_OBJECTS:connection_manager>)
