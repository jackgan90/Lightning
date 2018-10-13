macro(define_dll_export_macro DLLNAME)
	add_definitions(-DLIGHTNING_${DLLNAME}_EXPORT)
endmacro()

function(get_msvc_postfix postfix)
	if(${MSVC_VERSION} EQUAL 1200)
		set(${postfix} "60" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1300)
		set(${postfix} "70" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1310)
		set(${postfix} "71" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1400)
		set(${postfix} "80" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1500)
		set(${postfix} "90" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1600)
		set(${postfix} "100" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1700)
		set(${postfix} "110" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1800)
		set(${postfix} "120" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1900)
		set(${postfix} "140" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1910)
		set(${postfix} "150" PARENT_SCOPE)
	endif()
endfunction()

function(get_msvc_major_version ver)
	if(${MSVC_VERSION} EQUAL 1200)
		set(${ver} "6" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1300)
		set(${ver} "7" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1310)
		set(${ver} "7" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1400)
		set(${ver} "8" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1500)
		set(${ver} "9" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1600)
		set(${ver} "10" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1700)
		set(${ver} "11" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1800)
		set(${ver} "12" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1900)
		set(${ver} "14" PARENT_SCOPE)
	elseif(${MSVC_VERSION} EQUAL 1910)
		set(${ver} "15" PARENT_SCOPE)
	endif()
endfunction()
