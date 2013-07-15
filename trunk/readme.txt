Testing the wrapper:
1- Go to folder:  ..\ogrenewt\bin\
2-You will find a serious of compiled demos. Try some or all them before you try anything else.

-Using the library:
1-The wrapper comes with precompiled release dll , you can simple copy that DLL to you Ogre folder and set a path to where the wrapper was unpacked.
For header: ..\ogrenewt\source 
For library: ..\ogrenewt\projects\vs_2009\Win32\ogreNewton\ReleaseDll

Building the library:
1-DonndLoad Newton SDK 3.11 (minimum)
2--open visual studio 2009 solution in folder: 
..\newton-dynamics\packages\projects\visualStudio_2008\build.sln
-You will need to set few environment variables: 
Say your Ogre SDK is installed: 

OGRE_ENGINE=C:\tmp\ogre_src_v1-8-1
OGRE_ENGINE_BUILD=C:\tmp\ogre_src_v1-8-1\project
OGRE_ENGINE_DEPENDENCIES=C:\tmp\ogre_src_v1-8-1\Dependencies

For the Newton engine
NEWTON_DYNAMICS=..\newton-dynamics

3-Now with visual studio 2009, open solution:
..\ogrenewton\projects\vs_2009\ogreNewton.sln


Building the demos
1-do the step on previous section
2- Now with visual studio 2009, open solution:
..\ogrenewton\projects\vs_2009\demos\allDemos.sln

All solution should build without warning or errors.


 



