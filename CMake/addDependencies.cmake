# Include SDL2, GLEW and OpenGL
message(STATUS "Including (if they are installed on the computer) SDL2, SDL2Image, GLEW and OpenGL")

# Looking for the package
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# Special treatment for SDL2Image
INCLUDE(FindPkgConfig)
pkg_check_modules(SDL2_IMG REQUIRED SDL2_image)

# Including all the dependencies to the project
include_directories(${SDL2_INCLUDE_DIRS}
        ${SDL2_IMG_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
		${OPENGL_INCLUDE_DIRS})
