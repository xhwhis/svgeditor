cmake_minimum_required(VERSION 3.5)

project(unit_test LANGUAGES CXX)

find_package(GTest REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_executable(unit_test
	main.cpp
	unit_test_lcanvasitem.h
	unit_test_lcanvasitem.cpp
	unit_test_lcanvasview.h
	unit_test_lcanvasview.cpp
	../include/lcanvasitem.h
	../include/lcanvasview.h
)

target_link_libraries(unit_test PRIVATE ${GTEST_BOTH_LIBRARIES})
target_link_libraries(unit_test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
