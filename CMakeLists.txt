cmake_minimum_required(VERSION 3.19)

project(qtCalc LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY "")
set(CMAKE_CXX_FLAGS "")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# bring in your math engine library
add_subdirectory(../shuntingYard shuntingYard_build)

qt_add_executable(qtCalc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    compute.cpp
    compute.h
    ${GENERATED_SRC}
)

target_link_libraries(qtCalc
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        sntYrd
)

install(TARGETS qtCalc
    RUNTIME DESTINATION bin
)

set_target_properties(qtCalc PROPERTIES OUTPUT_NAME calc)
