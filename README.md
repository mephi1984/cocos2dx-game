# How to run

Install cocos2dx with submodules, branch v4: https://github.com/cocos2d/cocos2d-x


Download Python 2.7, install somewhere.

Run x86 Visual Studio Command Prompt

Set path for Python, for example: `set PATH=C:\Work\Python27;%PATH%`

Instead of `C:\Work\Python27` write where is your Python installed.

go to cocos2d directory using the same command line.

Run:

`python download-deps.py`

Run:

`python setup.py`

When asking from NDK_ROOT and ANDROID_SDK_ROOT, skip them.

Then restart the x86 Visual Studio Command Prompt.

Again set path to Python:
`set PATH=C:\Work\Python27;%PATH%`

Create a directory for the game, for example: `C:/Work/Projects/fishrungames.testgame001`

Run command:

`C:\Work\cocos2d-x-4.0\tools\cocos2d-console\bin\cocos.bat new -l cpp -p fisrungames.testgame001 -d C:/Work/Projects/fishrungames.testgame001 testgame001`

Don't forget to replace path to cocos2d and to your project.

When project is created, use CMake to make a VS solution:
```
mkdir build32
cd build32
cmake -G "Visual Studio 17 2022" -A Win32 -B "build32" ..
```

Then open Visual Studio, select testgame001 as startup project.

Then build and run it.

# License
MIT
