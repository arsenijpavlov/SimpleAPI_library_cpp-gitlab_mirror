include(${SIMPLE_API_MAIN_DIR}/text_colors.cmake)

function(print_project_name NAME)
	message("${Bold}project: ${Green}${NAME}${Reset}")
endfunction()
