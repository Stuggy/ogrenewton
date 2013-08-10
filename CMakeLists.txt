# Copyright (c) <2009> <Newton Game Dynamics>
# 
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely

cmake_minimum_required( VERSION 2.8 )
project (ogreNewtonSDK)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE headerFiles ${CMAKE_SOURCE_DIR}/source/*.h)
file(GLOB_RECURSE sourceFiles ${CMAKE_SOURCE_DIR}/source/*.cpp)


set (LIB_TYPE SHARED)
add_library(ogreNewton ${LIB_TYPE} ${sourceFiles} ${headerFiles})


