file(GLOB BUSINESS_SRC "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/src/utils/*.cpp")
message("BUSINESS_SRC = ${BUSINESS_SRC}")

find_package(OpenSSL REQUIRED)

add_library(business SHARED ${BUSINESS_SRC})
target_include_directories(business PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
target_link_libraries(business PRIVATE reactor OpenSSL::SSL OpenSSL::Crypto)