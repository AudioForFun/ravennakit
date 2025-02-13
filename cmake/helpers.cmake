# Adds a CMake variable and a compile definition.
#
# Usage:
#   add_project_variable(NAME VALUE)
#
# Arguments:
#   NAME  - The name of the variable and compile definition.
#   VALUE - The value to assign to both the CMake variable and compile definition.
#
# Example:
#   add_project_variable(ENABLE_FEATURE_X ON)
#   add_project_variable(MAX_BUFFER_SIZE 1024)
function(add_project_variable NAME VALUE)
    # Set the CMake variable
    set(${NAME} ${VALUE} PARENT_SCOPE)

    # Add it as a compile definition
    add_compile_definitions(${NAME}="${VALUE}")
endfunction()
