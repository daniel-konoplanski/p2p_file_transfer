set(PROTO_FILES
  ${CMAKE_SOURCE_DIR}/messages/FileTransferComplete.proto
  ${CMAKE_SOURCE_DIR}/messages/FileChunk.proto
  ${CMAKE_SOURCE_DIR}/messages/FileTransferProposalReq.proto
  ${CMAKE_SOURCE_DIR}/messages/FileTransferProposalReq.proto
  ${CMAKE_SOURCE_DIR}/messages/Result.proto
)

add_library(p2pft_proto_lib)

set(PROTO_GEN_DIR "${CMAKE_BINARY_DIR}/generated/proto")

protobuf_generate(
  TARGET p2pft_proto_lib
  PROTOS ${PROTO_FILES}
  IMPORT_DIRS ${CMAKE_SOURCE_DIR}/messages
  PROTOC_OUT_DIR ${PROTO_GEN_DIR}
)

target_link_libraries(p2pft_proto_lib PUBLIC protobuf::libprotobuf)
target_include_directories(p2pft_proto_lib PUBLIC ${PROTO_GEN_DIR})