#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
#          Project template           #
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
cmake_minimum_required(VERSION 3.11)


#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
#             USER INPUT              #
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#

# NOTE: Remember to copy the nsg executable
#  into same folder as this file.

# Change to your projects name
project(TEMPLATE_NAME)

# Add all new files to the following list (including headers)
set(generatedSourceFiles
	# EXAMPLE: "generated/FILENAME.generated.EXTENTION"
	"generated/scripts.generated.cpp"
	"generated/scripts.generated.h"
)

# Add all new files to the following list (including headers)
set(sourceFiles
	# EXAMPLE: "src/FILENAME.EXTENTION"
)


#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
#              INTERNAL               #
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#

set_source_files_properties(${generatedSourceFiles} PROPERTIES GENERATED TRUE)

add_library(${PROJECT_NAME} SHARED
	${generatedSourceFiles}
)

add_custom_command(
	OUTPUT ${generatedSourceFiles}
	DEPENDS ${sourceFiles}
	COMMAND "${CMAKE_CURRENT_LIST_DIR}/nsg.exe" "${CMAKE_CURRENT_LIST_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/generated"

	COMMENT "Generating source files"
)

target_link_libraries(${PROJECT_NAME} PUBLIC ns)
