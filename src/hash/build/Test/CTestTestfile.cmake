# CMake generated Testfile for 
# Source directory: /root/Dev/tmp/HAMT/Test
# Build directory: /root/Dev/tmp/HAMT/build/Test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(SimplePerfTest "/root/Dev/tmp/HAMT/build/Test/SimplePerfTest")
  set_tests_properties(SimplePerfTest PROPERTIES  WORKING_DIRECTORY "/root/Dev/tmp/HAMT" _BACKTRACE_TRIPLES "/root/Dev/tmp/HAMT/Test/CMakeLists.txt;68;add_test;/root/Dev/tmp/HAMT/Test/CMakeLists.txt;0;")
endif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
subdirs("HAMT")
