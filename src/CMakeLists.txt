# include ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_BUILD_TYPE Debug)

# add libraries
add_library(chip8_util chip8_util.c)
target_include_directories(chip8_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(chip8_util ${CURSES_LIBRARIES})

add_library(chip8_graphics chip8_graphics.c)
target_include_directories(chip8_graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(chip8_graphics ${CURSES_LIBRARIES} chip8_util)

add_library(chip8_emulator chip8_emulator.c)
target_include_directories(chip8_emulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(chip8_emulator ${CURSES_LIBRARIES} chip8_util chip8_graphics)

# add executables
add_executable(test test.c)
target_link_libraries(test ${CURSES_LIBRARIES} chip8_util chip8_graphics)
target_compile_options(test PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(test2 test2.c)
target_link_libraries(test2 ${CURSES_LIBRARIES} chip8_util chip8_graphics)
target_compile_options(test2 PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(test3 test3.c)
target_link_libraries(test3 ${CURSES_LIBRARIES} chip8_util chip8_emulator)
target_compile_options(test3 PRIVATE -Wall -Wextra -pedantic -Werror)

add_executable(test4 test4.c)
target_link_libraries(test4 ${CURSES_LIBRARIES} chip8_util chip8_emulator)
target_compile_options(test4 PRIVATE -Wall -Wextra -pedantic -Werror)
