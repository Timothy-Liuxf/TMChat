@ECHO OFF
MSBuild src\TMChat.sln -t:Build -p:Configuration=Release;Platform=x64 -m:4
PAUSE
