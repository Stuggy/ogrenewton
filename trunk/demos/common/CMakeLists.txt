# Copyright (c) <2009> <Newton Game Dynamics>
# 
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely


set (projectName common)

include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/dotSceneLoader)

file(GLOB_RECURSE headers *.h dotSceneLoader/*.h dotSceneLoader/*.h dotSceneLoader/*.hpp)
file(GLOB_RECURSE sources *.cpp dotSceneLoader/*.cpp)
add_library(${projectName} STATIC ${headers} ${sources})


set_target_properties(${projectName} PROPERTIES COMPILE_FLAGS "/YuHelpersStdAfx.h
															   /Zm2000
															   /I${ogreNewtonPath}
															   /I${newtonPath}/packages/dMath
															   /I${newtonPath}/packages/dNewton
															   /I${newtonPath}/packages/dCustomJoints
															   /I${newtonPath}/coreLibrary_300/source/newton
															   /I${ogreEngine}/OgreMain/include 
															   /I${ogreEngine}/OgreMain/include/OGRE 
															   /I${ogreEngine}/Samples/Common/include
															   /I${ogreEngine}/Components/Terrain/include
															   /I${ogreEngineBuild}/include
															   /I${ogreEngineDependecies}/include")
															   
set_target_properties (${projectName} PROPERTIES COMPILE_DEFINITIONS ";_CRT_SECURE_NO_WARNINGS;BOOST_ALL_NO_LIB")
set_source_files_properties (HelpersStdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcHelpersStdAfx.h")
