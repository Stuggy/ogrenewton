# Copyright (c) <2009> <Newton Game Dynamics>
# 
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
# 
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely

cmake_minimum_required (VERSION 2.8.11)
project (ogreNewton)

set(CMAKE_USE_RELATIVE_PATHS ON)

option (buildStaticLibrary "build static library" false)   


set (ogreEngine "$(OGRE_ENGINE)")
set (ogreEngineBuild "$(OGRE_ENGINE_BUILD)")
set (ogreEngineDependecies "$(OGRE_ENGINE_DEPENDENCIES)")
set (newtonPath "$(NEWTON_DYNAMICS)")

set (PlatformName "$(PlatformName)")

set (ogreNewtonPath ${PROJECT_SOURCE_DIR}/source)

add_subdirectory (source)
add_subdirectory (demos)





