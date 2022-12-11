cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "3" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(allocator VERSION ${PROJECT_VERSION})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(GTest REQUIRED)

configure_file(version.h.in version.h)

#Main target 
add_executable(custom_allocator main.cpp)
add_library(lib_allocator lib.cpp)

#Test target
add_executable(boost_tests boost_tests.cpp)
add_executable(google_tests google_tests.cpp)

set_target_properties(custom_allocator lib_allocator boost_tests google_tests PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_include_directories(lib_allocator
    PRIVATE "${CMAKE_BINARY_DIR}"
)

# Add src to the include directories for the test target
target_include_directories(google_tests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add googletest to the include directories for the test target
target_include_directories(google_tests
    PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(custom_allocator PRIVATE
    lib_allocator
)

target_link_libraries(boost_tests
    ${Boost_LIBRARIES}
    lib_allocator
)

# Link test target against gtest libraries
if (MSVC)
    target_link_libraries(google_tests PRIVATE
    	${GTEST_BOTH_LIBRARIES}
    	lib_allocator
    )
else ()
    target_link_libraries(google_tests PRIVATE
    	${GTEST_BOTH_LIBRARIES}
		pthread
    	lib_allocator
    )
endif()

if (MSVC)
    target_compile_options(custom_allocator PRIVATE
        /W4
    )
    target_compile_options(lib_allocator PRIVATE
        /W4
    )
    target_compile_options(boost_tests PRIVATE
        /W4
    )
    target_compile_options(google_tests PRIVATE
        /W4
    )
else ()
    target_compile_options(custom_allocator PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(lib_allocator PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(boost_tests PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
     target_compile_options(google_tests PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

install(TARGETS custom_allocator RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

enable_testing()

add_test(boost_tests boost_tests)