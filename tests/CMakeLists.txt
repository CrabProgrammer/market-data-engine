file(GLOB MDE_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(mde_tests ${MDE_TEST_SOURCES})
target_link_libraries(mde_tests PRIVATE mde GTest::gtest_main mde_warnings)
gtest_discover_tests(mde_tests DISCOVERY_MODE PRE_TEST)
