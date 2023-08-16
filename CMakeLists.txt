project(scancontext_ros2)
cmake_minimum_required(VERSION 3.5) # ROS2 minimum version

# Find required non ROS2 packages
find_package(PCL REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

message(STATUS "PCL_INCLUDE_DIRS:" ${PCL_INCLUDE_DIRS})
message(STATUS "PCL_LIBRARY_DIRS:" ${PCL_LIBRARY_DIRS})
message(STATUS "PCL_DEFINITIONS:" ${PCL_DEFINITIONS})

# ROS 2 integration, enable c++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Use -std=c++14 instead of -std=gnu++14
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compile optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops")

# Enable debug flags (use if you want to debug in gdb)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -Wuninitialized -fno-omit-frame-pointer")

# Find necessary ROS2 packages
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(pcl_conversions REQUIRED)

# Include local hdl_graph_slam headers
include_directories(include)
include_directories(
  ${PCL_INCLUDE_DIRS}
)

# Make this package usable as a library in other packages according to 
# https://docs.ros.org/en/rolling/How-To-Guides/Ament-CMake-Documentation.html#adding-resources
add_library(${PROJECT_NAME} SHARED
  src/scancontext_ros2/Scancontext.cpp
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
    ${PCL_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
)
# Link non ROS2 libraries
target_link_libraries(${PROJECT_NAME}
 ${PCL_LIBRARIES}
)
# Link ROS2 libraries
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  pcl_conversions
)

# Install include directory
install(DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

install(TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

# ament export 
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

ament_package()