# the exercise name is nothing else than the directory
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# these files are copied only once
file(GLOB TEXTURES
    skybox/miramar
    skybox/ely_cloudtop
    mountains_render/grass.jpg
mountains_render/rock.jpg
mountains_render/snow.jpg
mountains_render/sand.jpg
water/normalmap.jpg)
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
  perlin_noise/*.glsl
  grid/*.glsl
  skybox/*.glsl
  mountains_render/*.glsl
  mountains_create/*.glsl
  water/*.glsl
  shadowmap/*.glsl)
deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
