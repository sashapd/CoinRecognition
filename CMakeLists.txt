cmake_minimum_required(VERSION 3.8)
project(CoinRecognition)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( OpenCV REQUIRED )
set(SOURCE_FILES main.cpp Coin.cpp Coin.h CoinCounter.cpp CoinCounter.h)
add_executable(CoinRecognition ${SOURCE_FILES})
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries( CoinRecognition ${OpenCV_LIBS} )