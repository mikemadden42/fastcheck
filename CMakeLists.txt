cmake_minimum_required(VERSION 3.10)

project(fastcheck C)

set(CMAKE_C_STANDARD 17)
if (NOT MSVC)
  set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic")
endif()

add_executable(fastcheck
  main.c
)

install(TARGETS fastcheck
  RUNTIME
  DESTINATION bin
)

set(CPACK_PACKAGE_NAME "fastcheck")
set(CPACK_PACKAGE_VENDOR "Setec Astronomy")
set(CPACK_PACKAGE_CONTACT "Setec Astronomy")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "fastcheck - quickly check Linux specs")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Console Internet Tools")

include(CPack)
