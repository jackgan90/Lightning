## ======================================================================== ##
## Copyright 2009-2015 Intel Corporation                                    ##
##                                                                          ##
## Licensed under the Apache License, Version 2.0 (the "License");          ##
## you may not use this file except in compliance with the License.         ##
## You may obtain a copy of the License at                                  ##
##                                                                          ##
##     http://www.apache.org/licenses/LICENSE-2.0                           ##
##                                                                          ##
## Unless required by applicable law or agreed to in writing, software      ##
## distributed under the License is distributed on an "AS IS" BASIS,        ##
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. ##
## See the License for the specific language governing permissions and      ##
## limitations under the License.                                           ##
## ======================================================================== ##

if (WIN32)
	IF (${CMAKE_GENERATOR} MATCHES "(Win64|IA64)")
		SET(RTTR_ARCH x64)
	ELSE()
		SET(RTTR_ARCH x32)
	ENDIF()

	set(RTTR_LIB_DIR ${RTTR_ROOT}/lib)
	set(RTTR_BIN_DIR ${RTTR_ROOT}/bin)
	set(RTTR_BIN_DIR_DEBUG ${RTTR_BIN_DIR}/${RTTR_ARCH}/Debug)
	set(RTTR_BIN_DIR_RELEASE ${RTTR_BIN_DIR}/${RTTR_ARCH}/Release)
	set(RTTR_INCLUDE_DIR ${RTTR_ROOT}/src)
	set(RTTR_LIBRARY_DEBUG ${RTTR_LIB_DIR}/${RTTR_ARCH}/Debug/rttr_core_d.lib)
	set(RTTR_LIBRARY_RELEASE ${RTTR_LIB_DIR}/${RTTR_ARCH}/Release/rttr_core.lib)
	set(RTTR_DLLS ${RTTR_BIN_DIR_DEBUG}/rttr_core_d.dll ${RTTR_BIN_DIR_RELEASE}/rttr_core.dll)
endif()

SET(RTTR_LIBRARIES debug ${RTTR_LIBRARY_DEBUG} optimized ${RTTR_LIBRARY_RELEASE})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RTTR DEFAULT_MSG RTTR_INCLUDE_DIR RTTR_LIBRARIES)
