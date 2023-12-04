@ECHO OFF
MSBuild ".\TMChat.sln" "-t:Clean" "-p:Configuration=Release;Platform=x64" "-m:4"
PAUSE
