# TMChat

**Start chatting with TMChat now!**

TMChat, a small chat tool with a self-written TCP/IPv4 communication library, TMSocket. 

## Supported Compiler

|       Compiler       |     Platform     | Target |        C++11        |   C++14 \~ C++20    |
| :------------------: | :--------------: | :----: | :-----------------: | :-----------------: |
|   GCC 7 \~ GCC 11    |   Linux x86_64   | `-m64` | Perfectly supported | Perfectly supported |
| Clang 12 \~ Clang 14 |   Linux x86_64   | `-m64` | Perfectly supported | Perfectly supported |
|      MSVC 19.37      | Windows (64-bit) |  x64   |    Not supported    |      Supported      |
|      MSVC 19.37      | Windows (64-bit) |  x86   |    Not supported    |  Compiler warnings  |
|    Apple Clang 13    |   macOS arm64    | `-m64` |      Supported      |      Supported      |

## Structure

+ tmsocket: A communication library using TCP/IPv4 which provides message pack oriented communication.  
+ server: The server of TMChat.  
+ client: The client of TMChat.  

## How to build

### Linux

```sh
# Requires GNU Autotools and GNU Make

$ autoreconf -i
$ ./configure
$ make -j$(nproc)
```

And the target `build/lib/libtmsocket.a`, `build/bin/tmchat_server` and `build/bin/tmchat_client` will be built. 

Then install it:

```shell
sudo make install
```

After installation, you can use the third party library `libtmsocket.a`: 

```c++
#include <tmsocket/include/server_communicator.hpp>
#include <tmsocket/include/client_communicator.hpp>
```

with link option `-ltmsocket` and use the chat tool:

```shell
$ tmchat_server
$ tmchat_client
```

### Windows

#### Use Visual Studio  

Open `TMChat.sln` with Visual Studio 2022 or later versions, choose a target platfrom (Release | x64 recommended) and then build the solution. 

Then the target `server.exe` and `client.exe` will be built. 

#### Use MSBuild  

You can also use MSBuild to build the targets. If so, you should have MSBuild in your path. You can add it to your path manually or use "Developer command Prompt for Visual Studio". 

You can run `Build.cmd` to build and run `Clean.cmd` to clean, or run the following command in cmd:  

```cmd
> MSBuild ".\TMChat.sln" [options]
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
> MSBuild ".\TMChat.sln" "-t:Build" "-p:Configuration=Release;Platform=x64" "-m:8"
```

Then the target `server.exe` and `client.exe` will be built in the directory below:  

|      |      Debug      |      Release      |
| :--: | :-------------: | :---------------: |
| x86  |   `src\Debug`   |   `src\Release`   |
| x64  | `src\x64\Debug` | `src\x64\Release` |

### macOS

```sh
# Requires GNU Autotools and GNU Make

$ autoreconf -i
$ CXX='clang++ -std=c++11' ./configure
$ make -j$(sysctl -n hw.logicalcpu)
```

And the target `build/lib/libtmsocket.a`, `build/bin/tmchat_server` and `build/bin/tmchat_client` will be built. 

Then install it:

```shell
sudo make install
```

After installation, you can use the third party library `libtmsocket.a`:

```c++
#include <tmsocket/include/server_communicator.hpp>
#include <tmsocket/include/client_communicator.hpp>
```

with link option `-ltmsocket` and use the chat tool:

```shell
$ tmchat_server
$ tmchat_client
```

