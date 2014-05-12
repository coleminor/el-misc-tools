set(SOURCES
  application.cpp
  application.hpp
  box.cpp
  box.hpp
  cache.cpp
  cache.hpp
  canvas.cpp
  canvas.hpp
  endian.hpp
  foreach.hpp
  format.cpp
  format.hpp
  label.cpp
  label.hpp
  main.cpp
  map.cpp
  map.hpp
  mark.cpp
  mark.hpp
  message.hpp
  name.cpp
  name.hpp
  note.hpp
  options.cpp
  options.hpp
  preprocessor.hpp
  sender.hpp
  store.cpp
  store.hpp
  toplevel.cpp
  toplevel.hpp
  utility.cpp
  utility.hpp
  widget.hpp
  widget.cpp
  workspace.hpp
  workspace.cpp
)

add_executable(${EXECUTABLE_NAME}
  ${SOURCES}
)

target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
target_link_libraries(${EXECUTABLE_NAME} ${ZLIB_LIBRARIES})

install(TARGETS ${EXECUTABLE_NAME}
  DESTINATION bin
)

add_custom_command(TARGET ${EXECUTABLE_NAME} PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/resources
  $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/resources
)
