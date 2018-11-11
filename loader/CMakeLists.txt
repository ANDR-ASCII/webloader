cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

set(AppName loader)
project(${AppName})

include(ExternalProject)

set(AppSourcesDir src)
set(AppHeadersDir include)

aux_source_directory(${AppSourcesDir} AppSourcesList)
file(GLOB_RECURSE AppHeadersList "include/*.h")

list(REMOVE_ITEM AppSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" AppSourcesList)
source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

configure_msvc_runtime()

add_executable(
	${AppName} 
	${AppSourcesList}
	${AppHeadersList}
)

if(MSVC)
	add_definitions(
		/EHsc
		/MP
		/std:c++latest
		/Zi
		/W4
		/WX
		-D_CRT_SECURE_NO_WARNINGS
		-D_SCL_SECURE_NO_WARNINGS
	)
	
	set_target_properties(
		${AppName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${AppName} 
		PROPERTIES
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup /MAP"
	)
endif()

#json
externalproject_add(JsonDeps
	PREFIX nlohmann
	GIT_REPOSITORY https://github.com/nlohmann/json
	GIT_TAG f86090aafc0b83245d2fdebeba1a2dd64c39958a
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
)
	
externalproject_get_property(JsonDeps SOURCE_DIR)
include_directories("${SOURCE_DIR}")
list(APPEND Dependencies JsonDeps)

#openssl
# externalproject_add(OpensslDeps
# 	PREFIX openssl
# 	GIT_REPOSITORY https://github.com/openssl/openssl
# 	GIT_TAG e37b7014f3f52124b787ca1b5b51b0111462a0ac
# 	BUILD_COMMAND ${CMAKE_COMMAND} -DWINST_DIR=${PROJECT_SOURCE_DIR} -DOPENSSL_SOURCE_DIR=${PROJECT_BINARY_DIR}/openss/src/OpensslDeps/ -DOPENSSL_GENASM_COMMAND=${OPENSSL_GENASM_COMMAND} -DOPENSSL_CONFIGURE_COMMAND=${OPENSSL_CONFIGURE_COMMAND} -DOPENSSL_BUILD_COMMAND=${OPENSSL_BUILD_COMMAND} -DOPENSSL_INSTALL_COMMAND="${OPENSSL_BUILD_COMMAND} install" -P ${PROJECT_SOURCE_DIR}/configure-openssl.cmake
#     BUILD_IN_SOURCE 1
# )
# 	
# externalproject_get_property(OpensslDeps SOURCE_DIR BINARY_DIR)
# include_directories("${SOURCE_DIR}")
# link_directories("${BINARY_DIR}")
# list(APPEND Libraries libeay32 ssleay32)
# list(APPEND Dependencies OpensslDeps)
# 
# message("OpenSSL binary dir ${BINARY_DIR}")

# curl
externalproject_add(CurlDeps
	URL https://curl.haxx.se/download/curl-7.62.0.tar.gz
	PREFIX curl
	CMAKE_ARGS -DCURL_STATICLIB=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_CURL_EXE=OFF -DBUILD_CURL_TESTS=OFF -DCMAKE_USE_OPENSSL=ON -DCURL_ZLIB=OFF -DHTTP_ONLY=ON
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
)

externalproject_get_property(CurlDeps SOURCE_DIR BINARY_DIR)
include_directories("${SOURCE_DIR}/include")
link_directories("${BINARY_DIR}/lib")
list(APPEND Libraries wsock32 ws2_32)
list(APPEND Dependencies CurlDeps)
add_definitions(-DCURL_STATICLIB)

set(CURL_DEBUG_DIR ${BINARY_DIR}/lib/debug)
set(CURL_RELEASE_DIR ${BINARY_DIR}/lib/release)
message(STATUS "CURL Debug directory ${CURL_DEBUG_DIR}")
message(STATUS "CURL Release directory ${CURL_RELEASE_DIR}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	# in Debug mode
	list(APPEND Libraries ${CURL_DEBUG_DIR}/libcurl-d.lib)
	link_directories(${CURL_DEBUG_DIR})
else (CMAKE_BUILD_TYPE STREQUAL "Debug")
	# in Release mode
	list(APPEND Libraries ${CURL_RELEASE_DIR}/libcurl.lib)
	link_directories(${CURL_RELEASE_DIR})
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

if (CMAKE_PROGRAM_BITNESS STREQUAL "x64")
	set(OpenSslLibs "D:/deps/openssl/openssl_x64/lib/libssl.lib" "D:/deps/openssl/openssl_x64/lib/libcrypto.lib")
else (CMAKE_PROGRAM_BITNESS STREQUAL "x64")
	set(OpenSslLibs "D:/deps/openssl/openssl_win32/lib/libssl.lib" "D:/deps/openssl/openssl_win32/lib/libcrypto.lib")
endif ()

include_directories(${AppHeadersDir})
add_dependencies(${AppName} ${Dependencies})
target_link_libraries(${AppName} ${Libraries} ${OpenSslLibs})