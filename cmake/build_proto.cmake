set(PROTO_FILES
  ${CMAKE_SOURCE_DIR}/messages/FileTransferComplete.proto
  ${CMAKE_SOURCE_DIR}/messages/FileChunk.proto
  ${CMAKE_SOURCE_DIR}/messages/FileTransferProposalReq.proto
  ${CMAKE_SOURCE_DIR}/messages/FileTransferProposalReq.proto
  ${CMAKE_SOURCE_DIR}/messages/Result.proto
)

add_library(p2pft_proto_lib)

protobuf_generate(
  TARGET p2pft_proto_lib
  PROTOS ${PROTO_FILES}
  IMPORT_DIRS ${CMAKE_SOURCE_DIR}/messages
  PROTOC_OUT_DIR ${CMAKE_BINARY_DIR}/generated/proto
)

target_link_libraries(p2pft_proto_lib PUBLIC protobuf::libprotobuf)
target_include_directories(p2pft_proto_lib PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}  # Generated files go here by default
)