@ECHO OFF
MSBuild ".\TMChat.sln" "-t:Build" "-p:Configuration=Release;Platform=x64" "-m:4"
PAUSE
