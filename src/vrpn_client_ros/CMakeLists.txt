cmake_minimum_required(VERSION 2.8.3)
project(vrpn_client_ros)

find_package(catkin REQUIRED COMPONENTS geometry_msgs roscpp tf2_ros serial message_generation)
find_package(VRPN REQUIRED)

add_message_files(FILES Position_Yaw.msg)
generate_messages(DEPENDENCIES std_msgs)
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES vrpn_client_ros
  CATKIN_DEPENDS geometry_msgs tf2_ros std_msgs message_runtime
  DEPENDS VRPN
)


set_directory_properties(PROPERTIES COMPILE_OPTIONS "-std=c++11;-Wall")

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  SYSTEM ${VRPN_INCLUDE_DIR}
)

add_library(vrpn_client_ros
  src/vrpn_client_ros.cpp
)

#add_executable(vrpn_client_ros src/vrpn_client_ros.cpp)
#add_dependencies(vrpn_client_ros ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(vrpn_client_ros
  ${catkin_LIBRARIES}
  ${VRPN_LIBRARIES}
)

add_executable(vrpn_tracker_node
  src/vrpn_tracker_node.cpp)
add_dependencies(vrpn_tracker_node vrpn_client_ros)
target_link_libraries(vrpn_tracker_node
  vrpn_client_ros
  ${catkin_LIBRARIES}
)

add_executable(vrpn_client_node
  src/vrpn_client_node.cpp)

add_dependencies(vrpn_client_node vrpn_client_ros)
target_link_libraries(vrpn_client_node
  vrpn_client_ros
  ${catkin_LIBRARIES}
)


install(TARGETS vrpn_client_ros vrpn_tracker_node vrpn_client_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

if(CATKIN_ENABLE_TESTING)
  find_package(roslaunch REQUIRED)
  find_package(roslint REQUIRED)
  roslint_cpp()
  roslint_add_test()
  roslaunch_add_file_check(launch)
endif()
add_executable(listener src/listener.cpp)
target_link_libraries(listener ${catkin_LIBRARIES})
add_dependencies(listener ${PROJECT_NAME}_generate_messages_cpp)
add_executable(serial_port src/serial_port.cpp)
     
add_dependencies(serial_port ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
     
target_link_libraries(serial_port
      ${catkin_LIBRARIES}
    )



add_executable(serial_control src/serial_control.cpp)
     
add_dependencies(serial_control ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
     
target_link_libraries(serial_control
      ${catkin_LIBRARIES}
    )

add_executable(socket src/socket.cpp)
     
add_dependencies(socket ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
     
target_link_libraries(socket
      ${catkin_LIBRARIES}
    )



#add_executable(vrpn_client_ros src/vrpn_client_ros.cpp)
#add_dependencies(vrpn_client_ros ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#target_link_libraries(vrpn_client_ros ${catkin_LIBRARIES})



