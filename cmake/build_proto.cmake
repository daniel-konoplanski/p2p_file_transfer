set(PROTO_DIR "${CMAKE_SOURCE_DIR}/messages")

# list of .proto files (full paths)
set(PROTO_FILES
  ${PROTO_DIR}/FileTransferComplete.proto
  ${PROTO_DIR}/FileChunk.proto
  ${PROTO_DIR}/FileTransferProposalReq.proto
  ${PROTO_DIR}/FileTransferProposalResp.proto
  ${PROTO_DIR}/Result.proto
)

# where to put generated .pb.h/.pb.cc (desired output)
set(PROTO_SRC_DIR "${CMAKE_BINARY_DIR}/proto/src")
set(PROTO_INCLUDE_DIR "${CMAKE_BINARY_DIR}/proto/include")
set(PROTO_TEMP_DIR "${CMAKE_BINARY_DIR}/proto_temp")

file(MAKE_DIRECTORY "${PROTO_SRC_DIR}")
file(MAKE_DIRECTORY "${PROTO_INCLUDE_DIR}")
file(MAKE_DIRECTORY "${PROTO_TEMP_DIR}")

set(GENERATED_PROTO_SRCS "")
set(GENERATED_PROTO_HDRS "")

# hint vcpkg-installed protoc under the project build tree
find_program(PROTOC_EXECUTABLE NAMES protoc
  HINTS "${CMAKE_BINARY_DIR}"
)

# pretty-print helper for debugging
message(STATUS "----------------------------")
message(STATUS "         Proto info         ")
message(STATUS "----------------------------")

# single-line values
message(STATUS "PROTO_DIR:         ${PROTO_DIR}")
message(STATUS "PROTO_SRC_DIR:     ${PROTO_SRC_DIR}")
message(STATUS "PROTO_INCLUDE_DIR: ${PROTO_SRC_DIR}")
message(STATUS "PROTO_TEMP_DIR:    ${PROTO_TEMP_DIR}")
message(STATUS "PROTOC_EXECUTABLE: ${PROTOC_EXECUTABLE}")

list(LENGTH PROTO_FILES _total)

# probably move this to a different function
foreach(proto ${PROTO_FILES})
  math(EXPR _count "${_count} + 1")

  if(NOT EXISTS "${proto}")
    message(FATAL_ERROR "Proto file not found: ${proto}")
  else()
    if(_count EQUAL _total)
      message("  └── ${proto}")
    else()
      message("  ├── ${proto}")
    endif()
  endif()

  get_filename_component(pname ${proto} NAME_WE)

  set(temp_cc "${PROTO_TEMP_DIR}/${pname}.pb.cc")
  set(temp_h  "${PROTO_TEMP_DIR}/${pname}.pb.h")
  set(out_cc "${PROTO_SRC_DIR}/${pname}.pb.cc")
  set(out_h  "${PROTO_INCLUDE_DIR}/${pname}.pb.h")

  add_custom_command(
    OUTPUT "${out_cc}" "${out_h}"
    COMMAND ${PROTOC_EXECUTABLE} --cpp_out=${PROTO_TEMP_DIR} -I ${PROTO_DIR} ${proto}
    COMMAND ${CMAKE_COMMAND} -E copy ${temp_cc} ${out_cc}
    COMMAND ${CMAKE_COMMAND} -E copy ${temp_h} ${out_h}
    DEPENDS "${proto}"
    COMMENT "Generating ${pname}.pb.{cc,h}"
    VERBATIM
  )

  list(APPEND GENERATED_PROTO_SRCS "${out_cc}")
  list(APPEND GENERATED_PROTO_HDRS "${out_h}")
endforeach()

# Create proto lib
add_library(p2pft_proto_lib STATIC ${GENERATED_PROTO_HDRS} ${GENERATED_PROTO_SRCS})
target_link_libraries(p2pft_proto_lib PRIVATE protobuf::libprotobuf)
target_include_directories(p2pft_proto_lib PUBLIC ${PROTO_INCLUDE_DIR})
