EditorApplication
=================

Aplikacia sa spusta vo funkcii main(), kde sa vytvara instancia triedy **EditorApplication**. **EditorApplication** poskytuje metody na pracu s jadrom enginu (inicializacia, nacitanie hernej kniznice, pristup k instancii enginu...).

Inicializacia editoru je trojfazova:

1) V prvom kroku sa vytvori objekt **EditorApplication** a **EditorWindow**. Vytvorenie a spravne nastavenie okna **EditorWindow** je nutne kvoli inicializacii OpenGL, ktory engine vyzaduje.

2) Inicializaciu samotneho enginu. V tejto faze je potrebny uz inicializovany OpenGL, preto sa okno editoru **EditorWindow** vytvorilo uz v prvom kroku a nie neskor. Engine sa inicializuje s pasivnym **InputSystem-om**. Vsetky vstupne udalosti zachytava editor a preposiela ich do enginu (len v rezime GAME).

3) Po uspesnej inicializacii enginu sa moze nacitat herna kniznica (libgame.so/game.dll). Ak sa niektora z inicializacnych faz nepodari, editor vypise chybu a skonci. Zoznamu tvorcov sa naplni UI panel.

EditorWindow
============
Predstavuje hlavne okno aplikacie, aplikacia ma len jedno taketo okno. Editor dovoluje pracovat len s jednou scenou. Hlavne okno implementuje dva rezimy prace (**EditorWindowState**).

**1) Rezim editoru**  
V tomto rezime je povolena editacia sceny, su aktivovane vsetky panely a akcie (menu). Z edit rezimu sa stlacenim `F5` spusti simulacia sceny (herny rezim). Opatovnym stlacenim `F5` sa program vrati do editovacieho rezimu. Scena sa automaticky neaktualizuje/neprekresluje (casovas je vypnuty). Vykreslovanie sceny sa vynucuje manualne len pri zmene sceny. Mysou je mozno ovladat aktualny pohlad.

**2) Rezim hry**  
Aktivaciou tohto rezimu sa naklonuje aktualna scena a zacne sa jej simulacia (povodna scena je nedotknuta). Zaroven sa deaktivuje vacsina akcii (aby nemohlo dojst k editacii, ale napr. debug akcie su povolene) a spusti sa casovac, ktory kazdych 33ms simuluje a vykresluje scenu. Skryje sa kurzor mysi a vsetky stlacenia klaves a pohyb mysi sa posiela do enginu, ktory potom ovlada pohyb kamery.


GameView
========

Dva rezimy prace:

NORMAL
Pohyb mysi meni poohu kamery, funguje drag & drop (pouziva sa na pridanie entity do scene), je mozne editovat scenu.

GAME??
nedostava vstup, nepreposiela ho do enginu (vstup sa zachytava v EditorWindow prostrednictvom installEventFilter), nepovoluje drag & drop, editovanie sceny je zakazane.
