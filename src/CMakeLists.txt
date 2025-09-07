cmake_minimum_required(VERSION 3.21)
project(BloodGasAnalyzer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED CMAKE_PREFIX_PATH)
    message(WARNING "\nWARNING: CMAKE_PREFIX_PATH must be defined "
            "as path to your Qt installation, "
            "e.g. 'cmake .. -DCMAKE_PREFIX_PATH=C:/Qt/6.9.1/msvc2022_64 ...' "
            "(or /opt/Qt/6.9.1/macos). Using DEFAULT for Qt 6.9.1.")
    if(APPLE)
        set(CMAKE_PREFIX_PATH "/opt/Qt/6.9.1/macos")
        set(CMAKE_MAKE_PROGRAM /opt/homebrew/bin/ninja)  # this is for Qt Creator
    elseif(UNIX)
        set(CMAKE_MAKE_PROGRAM "/opt/Qt/6.8.1/gcc_arm64")
    elseif(WIN32)
        set(CMAKE_PREFIX_PATH "C:/Qt/6.8.1/msvc2022_64")
    endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Qml
    Quick
    Sql
    Widgets
)

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_standard_project_setup()

set(SOURCES
    src/cpp/${PROJECT_NAME}.cpp
    src/cpp/HistoricalDataModel.cpp
    src/cpp/DatabaseManager.cpp
    src/cpp/AuthenticationManager.cpp
    src/cpp/CalibrationManager.cpp
    src/cpp/HL7Manager.cpp
)

qt6_add_executable(${PROJECT_NAME}
    src/cpp/main.cpp
    qml.qrc
)

qt6_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    VERSION 1.0
    SOURCES
        ${SOURCES}
    QML_FILES
        src/qml/main.qml
        src/qml/LoginView.qml
        src/qml/MainView.qml
        src/qml/CalibrationView.qml
        src/qml/HistoryView.qml
        src/qml/SampleInputView.qml
        src/qml/components/TouchButton.qml
        src/qml/components/InputField.qml
    RESOURCES
        resources/icons/analyzer.png
        resources/icons/calibration.png
        resources/icons/history.png
        resources/icons/user.png
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::Sql
    Qt6::Widgets
)

# Enable debugging symbols
set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX "d"
)

if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/resources/icons/app.icns)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER au.devonline.${PROJECT_NAME}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/resources/icons/Info.plist
    )
endif()
