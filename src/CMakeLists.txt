include_directories(${CMAKE_SOURCE_DIR}/src)

#cmake_policy(SET CMP0167 OLD)



find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(spdlog REQUIRED)
find_package(ethercat REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Boost REQUIRED  system)
pkg_check_modules(Libmodbus REQUIRED libmodbus)
pkg_check_modules(XLSXWRITER REQUIRED xlsxwriter)

add_executable(${PROJECT_NAME}
    ${SOURCES}  
)
file(GLOB_RECURSE QRC_FILES "${CMAKE_SOURCE_DIR}/qrc/*.qrc")
qt6_add_resources(QRC_SOURCES ${QRC_FILES})
target_sources(${PROJECT_NAME} PRIVATE ${QRC_SOURCES})

target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets Qt6::Gui spdlog::spdlog ethercat
 ${Libmodbus_LIBRARIES} ${XLSXWRITER_LIBRARIES} Boost::system SQLite::SQLite3)




# 生成调试/发布版本
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -g -Wall -Wextra)
endif()

