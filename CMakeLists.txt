# RexHelpers: small things rexglue projects need that the SDK doesn't have, kept out of the SDK so a project
# can build on stock rexglue. Which guest address called a hook, which cvars a settings UI shows and which hold
# colours, app defaults for SDK cvars, and where the config file is.
#
#   include(path/to/RexHelpers/CMakeLists.txt)
#   rexglue_setup_target(<target>)
#   rexhelpers_attach(<target>)

include_guard(GLOBAL)

function(rexhelpers_attach target)
  set(dir ${CMAKE_CURRENT_FUNCTION_LIST_DIR})
  target_sources(${target} PRIVATE ${dir}/src/config_path.cpp ${dir}/src/cvar_color.cpp ${dir}/src/cvar_default.cpp ${dir}/src/cvar_visibility.cpp ${dir}/src/guest_caller.cpp)
  target_include_directories(${target} PRIVATE ${dir}/include)
  # For modules built on RexHelpers to check it's there.
  set_property(TARGET ${target} PROPERTY REXHELPERS_ATTACHED TRUE)
endfunction()
