cmake_minimum_required(VERSION 3.19)
project(qtCalc LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

add_subdirectory(../shuntingYard/ shuntingYard_build)

qt_add_executable(qtCalc

    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${GENERATED_SRC}
    compute.cpp
    compute.h
)

target_link_libraries(qtCalc
    PRIVATE
        Qt::Core
        Qt::Widgets
        sntYrd
)

install(TARGETS qtCalc
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET qtCalc
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

