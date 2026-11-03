add_executable(mde_tests smoke_test.cpp)
target_link_libraries(mde_tests PRIVATE GTest::gtest_main mde_warnings)
gtest_discover_tests(mde_tests DISCOVERY_MODE PRE_TEST)
