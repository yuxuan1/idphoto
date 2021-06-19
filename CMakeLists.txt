cmake_minimum_required(VERSION 3.5)

project(idphoto LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_DIR /Users/jiangkexuan/Downloads/qt-everywhere-src-6.0.4/build/qtbase/lib/cmake)
# find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)
# add opencv
set(OpenCV_DIR /Users/jiangkexuan/Downloads/opencv-4.5.1/output/lib/cmake/opencv4)
find_package(OpenCV 4 REQUIRED)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        idphoto.cpp
        imagelabel.cpp
        changecolor.cpp
        colorlabel.cpp
        colorlabel.h
        changecolor.h
        idphoto.h
        mainwindow.h
        mainwindow.ui
        imagelabel.h
        matqt.h
        photosize.h
        images.qrc
)

qt_add_executable(idphoto
    ${PROJECT_SOURCES}
)

target_link_libraries(idphoto PRIVATE Qt6::Widgets)
target_link_libraries(idphoto PRIVATE Qt6::Gui)
target_link_libraries(idphoto PRIVATE ${OpenCV_LIBS})
