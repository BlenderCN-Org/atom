Atom Project (Engine + Game + Editor)
=====================================

**Project features:**  
 - grip mechanics  
 - vast open world  
 - giant colosi  

**Project structure:**
 - src
  - libcore - engine
  - libgame - game
  - editor - level editor
  - runner - game launcher
  - flext - generated OpenGL header/extension loader
 - test - libcore unit tests
 

Dependencies
------------

 - SDL v1.2
 - OpenGL v4.2
 - Bullet physics v2.8.2
 - libpng v1.6
 - libogg v1.3.1
 - zlib v1.2.8
 - vorbisfile v1.3.4
 - rapidjson v0.11 (included)
 - Qt4 v4.8.6


Building from source
--------------------

This project use WAF meta build system (http://code.google.com/p/waf/).
WAF is written in Python so Python (v2 or v3) is required.  
To configure project use `./waf configure` command:  
`./waf configure -t debug`  
`./waf configure -t release`  
`CC=clang CXX=clang++ ./waf configure`

After successful configuration phase execute build command:  
`./waf`
