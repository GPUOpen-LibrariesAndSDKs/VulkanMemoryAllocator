# Find the fplutil directory and set it in `fplutil_dir`.
#
# We search some standard locations, such as
# (1) the cached variable ${dependencies_fplutil_dir}, which can be specified
#     on the command line,
#         cmake -Ddependencies_fplutil_dir=your_fplutil_directory
# (2) under ${fpl_root}, which is another cached variable that can be
#     specified on the command line,
#         cmake -Dfpl_root=your_fpl_root_directory
# (3) the "dependencies" directory that gets created when cloning from GitHub,
# (4) several levels up in the directory tree.
#
# Notes
# -----
# - fplutil is the project where we keep all our shared code, so the code in
#   this file (which locates fplutil) can unfortunately not be shared.
# - Since this file is duplicated in all FPL projects (except fplutil itself),
#   please copy new versions to all FPL projects whenever you make a change.

set(fplutil_dir_possibilities
    "${dependencies_fplutil_dir}"
    "${fpl_root}/fplutil"
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/fplutil"
    "${CMAKE_CURRENT_LIST_DIR}/../../fplutil"
    "${CMAKE_CURRENT_LIST_DIR}/../../../fplutil"
    "${CMAKE_CURRENT_LIST_DIR}/../../../../fplutil"
    "${CMAKE_CURRENT_LIST_DIR}/../../../../../fplutil")

foreach(dir ${fplutil_dir_possibilities})
  if(EXISTS ${dir})
    set(fplutil_dir ${dir})
    return()
  endif()
endforeach(dir)

# Define this cached variable so that cmake GUIs can expose it to the user.
set(dependencies_fplutil_dir ""
    CACHE PATH "Directory containing the fplutil library.")

MESSAGE(ERROR
    "Can't find fplutil directory. Try cmake -Ddependencies_fplutil_dir=your_location.")
