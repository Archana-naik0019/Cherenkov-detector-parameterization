cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(CherenkovSim)

find_package(Geant4 REQUIRED ui_all vis_all)

include(${Geant4_USE_FILE})

file(GLOB sources *.cc)
file(GLOB headers *.hh)

add_executable(CherenkovSim main.cc ${sources} ${headers})

target_link_libraries(CherenkovSim ${Geant4_LIBRARIES})

file(GLOB MACRO_FILES macros *.mac)
file(GLOB DATA_FILES data *.dat)
file(COPY ${MACRO_FILES} ${DATA_FILES} DESTINATION ${PROJECT_BINARY_DIR})

#add_custom_target(CherenkovSim DEPENDS CherenkovSim)
