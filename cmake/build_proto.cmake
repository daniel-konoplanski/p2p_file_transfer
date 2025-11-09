set(PROTO_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/messages/definitions/FileTransferComplete.proto
  ${CMAKE_CURRENT_SOURCE_DIR}/messages/definitions/FileChunk.proto
  ${CMAKE_CURRENT_SOURCE_DIR}/messages/definitions/FileTransferProposalReq.proto
  ${CMAKE_CURRENT_SOURCE_DIR}/messages/definitions/FileTransferProposalResp.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(p2pft_protos ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(p2pft_protos PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(p2pft_protos PUBLIC protobuf::libprotobuf)
