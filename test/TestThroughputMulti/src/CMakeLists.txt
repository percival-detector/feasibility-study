find_package( Boost 1.41.0
	      REQUIRED
	      COMPONENTS program_options)
find_package(Log4CXX 0.10.0 REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR on)

add_executable(testThroughputMulti appMain.cpp TestThroughputMulti.cpp)

target_link_libraries(testThroughputMulti ${Boost_LIBRARIES} ${LOG4CXX_LIBRARIES})
