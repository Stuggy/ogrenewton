Testing the wrapper:
1- Go to folder:  ..\ogrenewt\bin\
2-You will find a series of compiled demos. Try some or all them before you try anything else.

-Using the library:
1-The wrapper comes with precompiled release dll , you can simple copy that DLL to you Ogre folder and set a path to where the wrapper was unpacked.
For header: ..\ogrenewt\source 
For library: ..\ogrenewt\projects\vs_2010\Win32\ogreNewton\ReleaseDll

Building the library:
1-DownLoad Newton SDK 3.13 from github (minimum)

2-Open visual studio 2010 solution in folder: 
..\newton-dynamics\packages\projects\visualStudio_2010\build.sln

3-Build debug and release 

4-You will need to set few environment variables: 
Say your Ogre SDK is installed: 

For example the Newton engine in my system is set to 
NEWTON_DYNAMICS=..\newton-dynamics

For example the OGRE sdk in my system is set to 
OGRE_ENGINE=C:\Development\OgreSDK_vc10_v1-9-0

5-Now with visual studio 2010, open solution:
..\ogrenewton\projects\vs_2010\ogreNewton.sln


Building the demos
1- do the step on previous section
2- Now with visual studio 2010, open solution:
..\ogrenewton\projects\vs_2010\demos\allDemos.sln

All solution should build without warning or errors.


 



