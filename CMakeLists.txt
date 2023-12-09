cmake_minimum_required (VERSION 3.8)

# Abilita Ricaricamento rapido per i compilatori MSVC, se supportato.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("GameEngine")

set(COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS} -Wall -pedantic -Wextra -m64 -mavx2 -mfma -ffast-math")
set( GLFW_BUILD_DOCS      OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS     OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE )


# Aggiungere i percorsi di #include aggiuntivi
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Externals/Include")



# Aggiungere l'origine all'eseguibile di questo progetto.
add_executable (GameEngine 
  "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Include/glad/glad.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/Source/GameEngine.cpp"
)


# linking
target_link_libraries(GameEngine "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/glfw3.lib") 
target_link_libraries(GameEngine "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/spdlogd.lib")



if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GameEngine PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
endif()
