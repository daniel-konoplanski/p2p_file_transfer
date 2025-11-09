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
set(PROTO_GEN_DIR "${CMAKE_BINARY_DIR}/src/proto/messages")
file(MAKE_DIRECTORY "${PROTO_GEN_DIR}")

set(GENERATED_SRCS "")
set(GENERATED_HDRS "")

# hint vcpkg-installed protoc under the project build tree
find_program(PROTOC_EXECUTABLE NAMES protoc
  HINTS "${CMAKE_BINARY_DIR}"
)

# pretty-print helper for debugging
message(STATUS "----------------------------")
message(STATUS "         Proto info")
message(STATUS "----------------------------")

# single-line values
message(STATUS "PROTO_DIR:         ${PROTO_DIR}")
message(STATUS "PROTO_GEN_DIR:     ${PROTO_GEN_DIR}")
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
  set(out_cc "${PROTO_GEN_DIR}/${pname}.pb.cc")
  set(out_h  "${PROTO_GEN_DIR}/${pname}.pb.h")

  add_custom_command(
    OUTPUT "${out_cc}" "${out_h}"
    COMMAND ${PROTOC_EXECUTABLE}
    ARGS --cpp_out=${PROTO_GEN_DIR} -I ${PROTO_DIR} ${proto}
    DEPENDS "${proto}"
    COMMENT "Generating protobuf ${pname} -> ${PROTO_GEN_DIR}"
    VERBATIM
  )

  list(APPEND GENERATED_SRCS "${out_cc}")
  list(APPEND GENERATED_HDRS "${out_h}")
endforeach()

# ensure generation runs as part of the build
add_custom_target(p2pft_protos ALL DEPENDS ${GENERATED_SRCS} ${GENERATED_HDRS})
