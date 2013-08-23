# Copyright (c) <2009> <Newton Game Dynamics>
# 
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely


#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
#file(GLOB_RECURSE headerFiles ${CMAKE_SOURCE_DIR}/source/*.h)
#file(GLOB_RECURSE sourceFiles ${CMAKE_SOURCE_DIR}/source/*.cpp)

set (LIB_TYPE SHARED)
#add_library(ogreNewton ${LIB_TYPE} ${sourceFiles} ${headerFiles})



set (projectName ogreNewton)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE headers *.h)
file(GLOB_RECURSE sources *.cpp)
add_library(${projectName} ${LIB_TYPE} ${headers} ${sources})



#target_link_libraries (${projectName} ${corePath}/pthreads.2/dgThreads)
if (MSVC)
	set_target_properties(${projectName} PROPERTIES COMPILE_FLAGS "/YuOgreNewtonStdAfx.h
																   /I${ogreEngine}/OgreMain/include 
																   /I${ogreEngine}/OgreMain/include/OGRE 
																   /I${ogreEngine}/Samples/Common/include
																   /I${ogreEngine}/Components/Terrain/include
																   /I${ogreEngineBuild}/include 
																   /I${ogreEngineDependecies}/include
																   /I${newtonPath}/packages/dMath
																   /I${newtonPath}/packages/dNewton
																   /I${newtonPath}/packages/dCustomJoints
																   /I${newtonPath}/coreLibrary_300/source/newton")
																   
	set_target_properties (${projectName} PROPERTIES COMPILE_DEFINITIONS "_WIN_32_VER;_CRT_SECURE_NO_WARNINGS;_NEWTON_STATIC_LIB;_CNEWTON_STATIC_LIB;BOOST_ALL_NO_LIB;_OGRE_NEWTON_BUILD_DLL")
	set_source_files_properties(OgreNewtonStdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcOgreNewtonStdAfx.h")
endif(MSVC)



