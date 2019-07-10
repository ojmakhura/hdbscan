set(HDBSCAN_CUDA_INCLUDE_DIR, ${HDBSCAN_CUDA_DIR}/include)

include_directories (${HDBSCAN_CUDA_INCLUDE_DIR})

set_target_properties(hdbscan_cuda PROPERTIES
  PUBLIC_HEADER "include/cuda/*.h")