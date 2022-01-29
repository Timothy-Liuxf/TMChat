# TMChat

**Start chatting with TMChat now!**

TMChat, a small chat tool with a self-written TCP/IPv4 communication library, TMSocket. 

## Supported Compiler

|    Compiler     |     Platform     | Target |        C++11        |        C++14        |        C++17        |
| :-------------: | :--------------: | :----: | :-----------------: | :-----------------: | :-----------------: |
| GCC 7 \~ GCC 11 |   Linux x86-64   | `-m64` | Perfectly supported | Perfectly supported | Perfectly supported |
|    Clang 12     |   Linux x86-64   | `-m64` | Perfectly supported | Perfectly supported | Perfectly supported |
|    MSVC 19.2    | Windows (64-bit) |  x64   |    Not supported    |      Supported      |      Supported      |
|    MSVC 19.2    | Windows (64-bit) |  x86   |    Not supported    |  Compiler warnings  |  Compiler warnings  |

## Structure

+ tmsocket: A communication library using TCP/IPv4 which provides message pack oriented communication.  
+ server: The server of TMChat.  
+ client: The client of TMChat.  

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
  + `ADDITIONAL_OPTIONS`: Additional compiler options. Empty by default.  

  Example:   

  ```sh
  $ make CPP_STANDARD=-std=c++17 COMPILER=clang++
  ```

Then the target `build/bin/server.out` and`build/bin/client.out` will be built. 

### Windows

#### Use Visual Studio  

Open `src\TMChat.sln` with Visual Studio 2019 or later versions, choose a target platfrom (Release | x64 recommended) and then build the solution. 

Then the target `server.exe` and `client.exe` will be built. 

#### Use MSBuild  

You can also use MSBuild to build the targets. If so, you should have MSBuild in your path. You can add it to your path manually or use "Developer command Prompt for Visual Studio". 

You can run `Build.cmd` to build and run `Clean.cmd` to clean, or run the following command in cmd:  

```cmd
> MSBuild "src\TMChat.sln" [options]
```

 The options can be:  

+ `-t:<value>`: Specify the task. The `value` can be: 
  + `Build`: Incremental compile. This is the recommended value.  
  + `Recompile`: Recompile. This will delete all files built in the last compilation and rebuild the targets. 
  + `Clean`: Delete all files in the last compilation. 
+ `-p:<property>=<value>;<property>=<value>;...`. The `property` can be: 
  + `Configuration`: The value can be `Debug` or `Release`  
  + `Target`: The value can be `x86` or `x64`  
+ `-m:<value>`: The `value` specifies  the maximum number of concurrent processes to use when building. The default value is `1`  

Example: 

```cmd
> MSBuild "src\TMChat.sln" "-t:Build" "-p:Configuration=Release;Platform=x64" "-m:8"
```

Then the target `server.exe` and `client.exe` will be built in the directory below:  

|      |      Debug      |      Release      |
| :--: | :-------------: | :---------------: |
| x86  |   `src\Debug`   |   `src\Release`   |
| x64  | `src\x64\Debug` | `src\x64\Release` |



