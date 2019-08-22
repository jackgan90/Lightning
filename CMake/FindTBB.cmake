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

IF (WIN32)
	IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
		SET(TBB_ARCH intel64)
	ELSE()
		SET(TBB_ARCH ia32)
	ENDIF()

	IF (MSVC10)
		SET(TBB_VCVER vc10)
	ELSEIF (MSVC11)
		SET(TBB_VCVER vc11)
	ELSEIF (MSVC12)
		SET(TBB_VCVER vc12)
	ELSE()
		SET(TBB_VCVER vc14)
	ENDIF()

  SET(TBB_LIBDIR ${TBB_ROOT}/lib/${TBB_ARCH}/${TBB_VCVER})
  SET(TBB_BINDIR ${TBB_ROOT}/bin/${TBB_ARCH}/${TBB_VCVER})
  SET(TBB_DEBUG_DLLS ${TBB_BINDIR}/tbb_debug.dll ${TBB_BINDIR}/tbbmalloc_debug.dll)
  SET(TBB_RELEASE_DLLS ${TBB_BINDIR}/tbb.dll ${TBB_BINDIR}/tbbmalloc.dll)
  set(TBB_DLLS ${TBB_DEBUG_DLLS} ${TBB_RELEASE_DLLS})


  SET(TBB_INCLUDE_DIR TBB_INCLUDE_DIR-NOTFOUND)
  FIND_PATH(TBB_INCLUDE_DIR tbb/task_scheduler_init.h PATHS ${TBB_ROOT}/include NO_DEFAULT_PATH)
  SET(TBB_LIBRARY_DEBUG ${TBB_LIBDIR}/tbb_debug.lib)
  SET(TBB_LIBRARY_RELEASE ${TBB_LIBDIR}/tbb.lib)
  SET(TBB_LIBRARY_MALLOC_DEBUG ${TBB_LIBDIR}/tbbmalloc_debug.lib)
  SET(TBB_LIBRARY_MALLOC_RELEASE ${TBB_LIBDIR}/tbbmalloc.lib)
ELSE ()

  FIND_PATH(TBB_ROOT include/tbb/task_scheduler_init.h
    DOC "Root of TBB installation"
    PATHS ${PROJECT_SOURCE_DIR}/tbb /opt/intel/composerxe/tbb
  )
  
  IF (TBB_ROOT STREQUAL "")
    FIND_PATH(TBB_INCLUDE_DIR tbb/task_scheduler_init.h)
    FIND_LIBRARY(TBB_LIBRARY tbb)
    FIND_LIBRARY(TBB_LIBRARY_MALLOC tbbmalloc)
  ELSE()
    SET(TBB_INCLUDE_DIR TBB_INCLUDE_DIR-NOTFOUND)
    SET(TBB_LIBRARY TBB_LIBRARY-NOTFOUND)
    SET(TBB_LIBRARY_MALLOC TBB_LIBRARY_MALLOC-NOTFOUND)
    IF (APPLE)
      FIND_PATH(TBB_INCLUDE_DIR tbb/task_scheduler_init.h PATHS ${TBB_ROOT}/include NO_DEFAULT_PATH)
      FIND_LIBRARY(TBB_LIBRARY tbb PATHS ${TBB_ROOT}/lib NO_DEFAULT_PATH)
      FIND_LIBRARY(TBB_LIBRARY_MALLOC tbbmalloc PATHS ${TBB_ROOT}/lib NO_DEFAULT_PATH)
    ELSE()
      FIND_PATH(TBB_INCLUDE_DIR tbb/task_scheduler_init.h PATHS ${TBB_ROOT}/include NO_DEFAULT_PATH)
      FIND_LIBRARY(TBB_LIBRARY tbb PATHS ${TBB_ROOT}/lib/intel64/gcc4.4 ${TBB_ROOT}/lib ${TBB_ROOT}/lib64 NO_DEFAULT_PATH)
      FIND_LIBRARY(TBB_LIBRARY_MALLOC tbbmalloc PATHS ${TBB_ROOT}/lib/intel64/gcc4.4 ${TBB_ROOT}/lib ${TBB_ROOT}/lib64 NO_DEFAULT_PATH)
    ENDIF()
  ENDIF()

ENDIF()

SET(TBB_LIBRARY debug ${TBB_LIBRARY_DEBUG} optimized ${TBB_LIBRARY_RELEASE})
SET(TBB_LIBRARY_MALLOC debug ${TBB_LIBRARY_MALLOC_DEBUG} optimized ${TBB_LIBRARY_MALLOC_RELEASE})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TBB DEFAULT_MSG TBB_INCLUDE_DIR TBB_LIBRARY TBB_LIBRARY_MALLOC)

IF (TBB_FOUND)
  SET(TBB_INCLUDE_DIRS ${TBB_INCLUDE_DIR})
  SET(TBB_LIBRARIES ${TBB_LIBRARY} ${TBB_LIBRARY_MALLOC})
ENDIF()

MARK_AS_ADVANCED(TBB_INCLUDE_DIR)
MARK_AS_ADVANCED(TBB_LIBRARY)
MARK_AS_ADVANCED(TBB_LIBRARY_MALLOC)

##############################################################
# Install TBB
##############################################################

#IF (WIN32)
  #message("Before Install ${TBB_BINDIR}/tbb_debug.dll")
  #message("CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")
  #INSTALL(FILES ${TBB_BINDIR}/tbb.dll ${TBB_BINDIR}/tbbmalloc.dll DESTINATION bin)
  #INSTALL(FILES ${TBB_BINDIR}/tbb_debug.dll ${TBB_BINDIR}/tbbmalloc_debug.dll DESTINATION bin)
#ELSEIF (NOT ENABLE_INSTALLER)
  #IF (APPLE)
    #INSTALL(PROGRAMS ${TBB_ROOT}/lib/libc++/libtbb.dylib ${TBB_ROOT}/lib/libc++/libtbbmalloc.dylib DESTINATION lib COMPONENT lib)
  #ELSE()
    #INSTALL(PROGRAMS ${TBB_ROOT}/lib/intel64/gcc4.4/libtbb.so.2 ${TBB_ROOT}/lib/intel64/gcc4.4/libtbbmalloc.so.2 DESTINATION lib COMPONENT lib)
  #ENDIF()
#ENDIF()
