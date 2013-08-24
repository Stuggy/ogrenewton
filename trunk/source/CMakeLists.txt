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


if (buildStaticLibrary)
	set (libraryType STATIC)
	
#	_NEWTON_STATIC_LIB;_CNEWTON_STATIC_LIB
	
else (buildStaticLibrary)
	set (libraryType SHARED)

	set (newtonPreprocessor ";_OGRE_NEWTON_BUILD_DLL")
	
	if (MSVC_VERSION EQUAL 1500)
		set (newtonLibPath project_vs2008_dll)
		set (packagesLibPath visualStudio_2008_dll)
	endif (MSVC_VERSION EQUAL 1500)
	
	if (MSVC_VERSION EQUAL 1600)
		set (newtonLibPath project_vs2010_dll)
		set (packagesLibPath visualStudio_2010_dll)
	endif (MSVC_VERSION EQUAL 1600)
		
	if (MSVC_VERSION EQUAL 1700)
		set (newtonLibPath project_vs2012_dll)
		set (packagesLibPath visualStudio_2012_dll)
	endif (MSVC_VERSION EQUAL 1700)
		
endif (buildStaticLibrary)	

set (projectName ogreNewton)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE headers *.h)
file(GLOB_RECURSE sources *.cpp)
add_library(${projectName} ${libraryType} ${headers} ${sources})


if (NOT buildStaticLibrary)
	target_link_libraries (${projectName} debug ${newtonPath}/packages/projects/${packagesLibPath}/Win32/dMath/debug/dMath_d.lib
										  debug	${newtonPath}/packages/projects/${packagesLibPath}/Win32/dNewton/debug/dNewton_d.lib
										  debug ${newtonPath}/coreLibrary_300/projects/windows/${newtonLibPath}/Win32/newton/Debug/newton_d.lib
										  debug ${newtonPath}/packages/projects/${packagesLibPath}/Win32/dJointLibrary/debug/dJointLibrary_d.lib
										  debug ${ogreEngineDependecies}/lib/Debug/OIS_d.lib
										  debug ${ogreEngineBuild}/lib/Debug/OgreMain_d.lib
										  debug ${ogreEngineBuild}/lib/Debug/OgreTerrain_d.lib)

	target_link_libraries (${projectName} optimized ${newtonPath}/packages/projects/${packagesLibPath}/Win32/dMath/release/dMath.lib
										  optimized ${newtonPath}/packages/projects/${packagesLibPath}/Win32/dNewton/release/dNewton.lib
										  optimized ${newtonPath}/coreLibrary_300/projects/windows/${newtonLibPath}/Win32/newton/Release/newton.lib
										  optimized ${newtonPath}/packages/projects/${packagesLibPath}/Win32/dJointLibrary/release/dJointLibrary.lib
  										  optimized ${ogreEngineDependecies}/lib/Release/OIS.lib
										  optimized ${ogreEngineBuild}/lib/Release/OgreMain.lib
										  optimized ${ogreEngineBuild}/lib/Release/OgreTerrain.lib)
endif (NOT buildStaticLibrary)

if (MSVC)
	set_target_properties(${projectName} PROPERTIES COMPILE_FLAGS "/YuOgreNewtonStdAfx.h
																   /Zm2000
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
																   
	set_target_properties (${projectName} PROPERTIES COMPILE_DEFINITIONS "_WIN_32_VER;_CRT_SECURE_NO_WARNINGS;BOOST_ALL_NO_LIB${newtonPreprocessor}")
	set_source_files_properties(OgreNewtonStdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcOgreNewtonStdAfx.h")
endif(MSVC)



