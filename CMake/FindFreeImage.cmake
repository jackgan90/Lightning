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
		SET(FREEIMAGE_ARCH x64)
	ELSE()
		SET(FREEIMAGE_ARCH x32)
	ENDIF()

	set(FREEIMAGE_INCLUDE_DIR ${FREEIMAGE_ROOT})
	set(FREEIMAGE_LIBRARY_DEBUG ${FREEIMAGE_ROOT}/${FREEIMAGE_ARCH}/FreeImaged.lib)
	set(FREEIMAGE_LIBRARY_RELEASE ${FREEIMAGE_ROOT}/${FREEIMAGE_ARCH}/FreeImage.lib)
	set(FREEIMAGE_DLLS ${FREEIMAGE_ROOT}/${FREEIMAGE_ARCH}/FreeImaged.dll ${FREEIMAGE_ROOT}/${FREEIMAGE_ARCH}/FreeImage.dll)
endif()

SET(FREEIMAGE_LIBRARIES debug ${FREEIMAGE_LIBRARY_DEBUG} optimized ${FREEIMAGE_LIBRARY_RELEASE})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FREEIMAGE DEFAULT_MSG FREEIMAGE_INCLUDE_DIR FREEIMAGE_LIBRARIES)
