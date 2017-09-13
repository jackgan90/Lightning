macro(define_dll_export_macro DLLNAME)
	add_definitions(-DLIGHTNINGGE_${DLLNAME}_EXPORT)
endmacro()
