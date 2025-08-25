cmake_minimum_required(VERSION 3.16)
project(L2Spice LANGUAGES CXX)

# --- Qt codegen
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 6 یا Qt 5 با ماژول‌های لازم
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts PrintSupport)

# --- سورس‌ها
file(GLOB_RECURSE SRC_CPP CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE SRC_H   CONFIGURE_DEPENDS "src/*.h" "src/*.hpp")
file(GLOB_RECURSE SRC_UI  CONFIGURE_DEPENDS "src/*.ui")

add_executable(${PROJECT_NAME}
  ${SRC_CPP} ${SRC_H} ${SRC_UI}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
          Qt${QT_VERSION_MAJOR}::Charts
          Qt${QT_VERSION_MAJOR}::PrintSupport
)

# --- کپی خودکار پوشه‌ی image کنار فایل اجرایی بعد از بیلد
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/image"
          "$<TARGET_FILE_DIR:${PROJECT_NAME}>/image"
  COMMENT "Copying image/ folder next to the executable"
)

# (اختیاری) قوانین نصب؛ اگر بعداً نصب گرفتی، پوشه image هم کنار exe نصب می‌شود
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/image" DESTINATION .)

# --- کمک برای MinGW وقتی آبجکت‌ها بزرگ می‌شوند
if (MINGW)
  add_compile_options(-Wa,-mbig-obj)
endif()
