# TMChat

**Chat with TMChat now!**

TMChat, a small chat tool. 

## Supported Compiler

|    Compiler     |     Platform     | Target |        C++11        |        C++14        |        C++17        |
| :-------------: | :--------------: | :----: | :-----------------: | :-----------------: | :-----------------: |
| GCC 7 \~ GCC 11 |   Linux x86-64   | `-m64` | Perfectly supported | Perfectly supported | Perfectly supported |
|    Clang 12     |   Linux x86-64   | `-m64` | Perfectly supported | Perfectly supported | Perfectly supported |
|    MSVC 19.2    | Windows (64-bit) |  x64   |    Not supported    |      Supported      |      Supported      |
|    MSVC 19.2    | Windows (64-bit) |  x86   |    Not supported    |  Compiler warnings  |  Compiler warnings  |

## How to build

### Linux

```sh
$ make [options]
```

The `options` can be:

  + `COMPILER`: Specify the compiler you want to build with. The value can be `g++` or `clang++`. `g++` by default.
  + `CPP_STANDARD`: Specify the C++ language standard. The value can be `-std=c++11`, `-std=c++14`, `-std=c++17`, `-std=gnu++11`, `-std=gnu++14`, `-std=gnu++17`, etc. The default value is `-std=c++11`.
  + `OPTIMIZATION`: Specify the level of optimization. The value can be `-O0`, `-O1`, `-Os`, etc. `-O2` by default.  
  + `WARNING_LEVEL`: Specify the warning level. The default value is `-Wall -Wpedantic -Wextra`
  + `PREDEFINED_MACRO`: Specify predefined macros, such as `-DUNICODE`. Empty by default.  

  Example:   

  ```sh
  make CPP_STANDARD=-std=c++17 COMPILER=clang++
  ```

Then the target `build/bin/server.out` and`build/bin/client.out` will be built. 

### Windows

Open `src\TMChat.sln` with Visual Studio 2019 or later versions, choose a target platfrom (Release | x64 recommended) and then build the solution. 

You can also use MSBuild 16 or later versions to build `server\server.vcxproj` and `client\client.vcxproj` directly. 

Then the target `server.exe` and `client.exe` will be built.

