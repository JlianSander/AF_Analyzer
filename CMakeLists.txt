cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS off)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("AF_Analyzer" LANGUAGES C CXX)

# Add source to this project's executable.
set(INCLD "${CMAKE_CURRENT_LIST_DIR}/include")
set(INCLD_LOGIC "${INCLD}/logic")
set(INCLD_UTIL "${INCLD}/util")
set(SRC "${CMAKE_CURRENT_LIST_DIR}/src/")
set(SRC_LOGIC "${SRC}logic/")
set(SRC_UTIL "${SRC}util/")

set(HEADERS
	${INCLD}/Main.h
	${INCLD_LOGIC}/AF.h 
	${INCLD_LOGIC}/Decoding.h 
	${INCLD_LOGIC}/Encoding.h 
	${INCLD_LOGIC}/Enums.h 
	${INCLD_LOGIC}/Parser_iccma.h
	${INCLD_LOGIC}/PreProcessor_DS_PR.h
	${INCLD_LOGIC}/Prioritizer.h
	${INCLD_LOGIC}/Reduct.h 
	${INCLD_LOGIC}/SatSolver.h
	${INCLD_LOGIC}/SatSolver_cadical.h
	${INCLD_LOGIC}/ScepticalCheck.h 
	${INCLD_LOGIC}/Solver_DS_PR.h 	

	${INCLD_UTIL}/VectorBitSet.h
	${INCLD_UTIL}/MemoryWatchDog.h
	${INCLD_UTIL}/Printer.h
	)

set(SRCFILES
    ${SRC}Main.cpp
	${SRC_LOGIC}AF.cpp 
	${SRC_LOGIC}Decoding.cpp 
	${SRC_LOGIC}Encoding.cpp 
	${SRC_LOGIC}Enums.cpp
	${SRC_LOGIC}Parser_iccma.cpp
	${SRC_LOGIC}PreProcessor_DS_PR.cpp
	${SRC_LOGIC}Prioritizer.cpp
	${SRC_LOGIC}Reduct.cpp 
	${SRC_LOGIC}SatSolver_cadical.cpp
	${SRC_LOGIC}ScepticalCheck.cpp
	${SRC_LOGIC}Solver_DS_PR.cpp

	${SRC_UTIL}MemoryWatchDog.c
	${SRC_UTIL}Printer.cpp
	)

set(SOURCES ${SRCFILES} ${HEADERS})

# --- cadical part I ---

# Set path to the directory containing libcadical.a
set(CADICAL_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/cadical")

# Add the directory containing libcadical.a to the list of paths to search for libraries
link_directories("${CADICAL_DIR}/build")

# Add include directories to search for headers
include_directories("${CADICAL_DIR}/src")

# ----------------------

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC INCLD)

# Add libraries

# --- OpenMP ---
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
endif()

if(OpenMP_C_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_C)
endif()

# --- cadical part II ---

# Link the static library
target_link_libraries(${PROJECT_NAME} PUBLIC cadical)
# Include the directory containing cadical.hpp
target_include_directories(${PROJECT_NAME} PUBLIC ${CADICAL_DIR})

# further settings

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()