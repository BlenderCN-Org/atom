Postup inicializacie enginu
===========================

1) inicializacia SDL (OpenGL, system vstupov a zvukova karta)  
2) inicializacia enginu (zavolat Core...???), napojenie na inicializovany OpenGL
3) nacitanie hernej kniznice (libgame.so/game.dll)  
4) nacitanie zoznamu podporovanych entit (tvorcov entit) z hernej kniznice  
5) vytvorenie prazdneho sveta, pridanie entit  
6) game loop  
7) zrusenie zoznamu podporovanych entit  
8) uzavretie hernej kniznice  
9) ukoncenie enginu (Core)  
10) zrusenie SDL (OpenGL, vstupy, zvuk)

Aktualna verzia enginu neimplementuje dynamicke nacitanie hernej kniznice. Herna kniznica je linkovana spolu k programu. Kroky `3` a `8` su preto vynechane.


Sucasti enginu
==============

Service
-------

Component
---------

Processor
---------
