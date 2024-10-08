cmake_minimum_required(VERSION 3.8)
project(moveTsimP)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(turtlesim REQUIRED)
find_package(tf2 REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(tf2_ros REQUIRED)

include_directories(include)

add_executable(finding_the_labyrinth src/finding_the_labyrinth.cpp)
ament_target_dependencies(finding_the_labyrinth rclcpp geometry_msgs sensor_msgs cv_bridge OpenCV)

add_executable(gpt src/gpt.cpp)
ament_target_dependencies(gpt rclcpp geometry_msgs sensor_msgs cv_bridge OpenCV)

add_executable(new_gpt src/new_gpt.cpp)
ament_target_dependencies(new_gpt rclcpp geometry_msgs sensor_msgs)

add_executable(gpt_img src/gpt_img.cpp)
ament_target_dependencies(gpt_img rclcpp geometry_msgs sensor_msgs cv_bridge OpenCV)

add_executable(gpt_move src/gpt_move.cpp)
ament_target_dependencies(gpt_move rclcpp geometry_msgs sensor_msgs cv_bridge OpenCV)

#install
install(TARGETS
  finding_the_labyrinth
  gpt
  new_gpt
  gpt_img
  gpt_move
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
