Découpage du code
=================


Avant de commencer à parler code, je vous conseil de lire (au moins en diagonal) ["Game programming patterns"](http://gameprogrammingpatterns.com/), ne serait-ce que pour avoir une vague idée des problématiques qu'on essaye d'adresser dans le moteur.

Je propose de partir sur une architecture modulaire. L'idée étant dans la mesure du possible d'utiliser des interfaces suffisamment neutres pour pouvoir remplacer un module par une nouvelle implémentation (qui typiquement repose sur une autre bibliothèque) en cas de besoin. Par ailleurs, on peut totalement se passer de certains modules si ils ne correspondent pas à nos besoins. Je dis ça pour Doc qui à l'air de s'inquiéter qu'on fasse des choses trop spécifiques et qui s'avèrent inapplicable à nos prochains jeux. C'est en effet une possibilité (peu probable à mon sens), mais dans ce cas il suffit de ne pas utiliser le module correspondant (et de le remplacer par quelque chose d'autre si nécessaire, ou par du code ad-hoc).


Les modules core et utils
-------------------------

Il y a un certain nombre de choses que je ne sais pas où mettre. On peut faire un micro-module pour chaque (bof), ou regrouper tout ça dans un module "core" ou "utils" (voir les deux). Il est toujours possible de bouger les choses qui deviennent trop complexes dans leur propre module.

En vrac:

- Le système de log: tout devrait en dépendre, donc probablement à mettre dans core si on fait deux modules. Pour simplifier l'utilisation du log, on peut mettre ça dans une variable globale.

- Les déclarations globales, comme les typedefs de vecteur, peuvent être placés dans core.

- Lecture/écriture de fichiers de config pour stocker diverse infos (résolution, plein écran, binding des touches, etc.). D'autres fichiers de config peuvent servir, par exemple, à stocker les propriétés des différents objets de jeu (dégât des armes, vitesse de déplacement, etc.) pour faciliter l'équilibrage. Plutôt core.

- La/les boucle(s) de jeu. Plutôt à mettre dans utils. Ses dépendances sont l'accès a une fonction de timing (avec une précision < à 1ms idéalement), probablement fournie par le module sys, et la possibilité de dispatcher les événements, là encore dépendant du module sys.

- La gestion des états. Aucune dépendance. Utils.

- L'objet "Game" qu'on a dans USB Warrior est principalement un point d'accès aux autres modules (c'est un service locator). On a plusieurs façons de l'implémenter. La solution typique est de créer une interface virtuelle pour chaque module, mais ça va être chiant (et c'est rigide: on est obligé d'implémenter l'interface; en même temps c'est nécessaire si on veut faire des modules interchangeables...). Autrement, on crée cet objet "Game" pour chaque jeu et on compte sur le linker pour recoller les morceaux (auquel cas on peut peut-être se passer de service locator). Ou encore on utilise un système de service locator plus intelligent. Des idées ? (Note: il y a pas urgence, on ne devrait pas avoir plusieurs implémentation d'un même module dans l'immédiat.)

- Chargement des niveaux (et autres trucs). Plutôt utils. Pas vraiment de dépendance.

- L'abstraction des entrée (la class InputManager). Dépend de sys. Plutôt utils.


Module système
--------------
(sys, aucune dépendance à d'autres modules):

Le module système se charge de l'interaction avec le système : ouverture de fenêtres, événements, son et graphismes (très) bas-niveau, etc. En pratique ce sera un wrapper par dessus la SDL2.

On pourra y inclure une abstraction du système de fichier. Ça peut permettre de faciliter la gestion des assets (notamment dans le cas de l'internationalisation et des mods; oui, pour l'instant il n'y a pas urgence). On peut aussi y inclure un mécanisme de chargement asynchrone.

En pratique, dans un premier temps on peut se contenter d'une classe qui permet d'ouvrir une fenêtre et de gérer la boucle d'éventements (Boulot effectué par la class Game dans USB Warrior). Par la suite on pourra structurer un peu plus:

- Classe Window pour gérer la fenêtre. A terme on pourrait gérer plusieurs fenêtres, mais l'utilité dans un jeu reste limité.
- Un système d'événements. Dans un premier temps, on peut se reposer lourdement sur la SDL2 (quitte à la faire apparaître dans l'interface) et on pourra bouger vers quelque chose de plus propre (mieux encapsulé) en cas de besoin. (Ceci dit, ça ne devrait pas arriver de si tôt.)


Module de rendu
---------------
(render, dépend de sys)

Le module render contient le code de rendu. Actuellement on utilise le module SDL2, mais il est très limité. Je propose de rapidement le remplacer par de l'OpenGL (ES2) qui nous permettra de faire ce qu'on veut (et une bonne partie de la complexité supplémentaire devait être abstraite par le module).

En pratique, dans un premier temps on utilise SDL_Render. À la limite on a même pas besoin de créer un module pour ça. Pour la version OpenGL j'ai quelques idées. Je pense partir sur une base simple qu'on pourra étendre par la suite.


Module audio
------------
(audio, dépend de sys)

Le son ! Pour l'instant on utilise SDL2_mixer mais on pourra remplacer ça par autre chose plus tard si on est limité (OpenAL ?).

En pratique, dans un premier temps, on utilise SDL_Mixer directement. Actuellement, si on a besoin de jouer plus de sons qu'il n'y a de canaux, ça peut foirer (probablement que le son ne sera simplement pas joué). Idéalement on encapsule ça dans un module en essayant d'abstraire la gestion des canaux (avec un système de priorité ?). Si besoin est, on passe à une bibliothèque un peu plus puissante.


Module collision / physique
---------------------------
(Aucune dépendance)

Pour l'instant on a rien (enfin, rien de modulaire), mais ça vaut le coup de mettre ça dans un module. Ne serait-ce que pour pouvoir avoir plusieurs implémentations si on arrive pas à faire quelque chose de suffisamment générique.

Dans un premier temps on peut faire notre propre code pour apprendre. Si besoin est, on pourra faire une implémentation qui repose sur une autre bibliothèque (ODE, bullet-physics ou Box2D par exemple).


Module objets/composants
------------------------
(game_objects, aucune dépendance, sauf sous-modules)

Le but de ce module est de fournir un framework pour implémenter la logique de jeu. Pour cela il nous faut un modèle assez souple pour ne pas se retrouver avec certaines choses impossible à mettre en œuvre. Ceci dit, comme on crée notre propre implémentation, on a toujours la possibilité de faire des modifications si besoin. De même, bien qu'il s'agisse probablement du gros morceau du moteur (ou tout du moins du plus expérimental), rien n'en dépend : on peut donc s'en passer tout en continuant à utiliser le reste.

Un jeu vidéo est généralement constitué d'un ensemble d'entités disparates qui interagissent. Le modèle qu'on a commercé à utiliser pour USB Warrior représente les objets à l'aide de composants. Chaque composant implémente un aspect du comportement de l'objet ce qui permet de construire tout un tas d'objet juste en créant différent assemblages de composant. Comme des Légos.

Un des avantages de cette approche est qu'on peut contrôler l'ordre des mises à jour par composants. Par exemple, les contrôles du joueur sont pris en compte, puis les IA, puis on calcul les déplacements de tous les objets (en faisant tourner le moteur physique si besoin), puis on peut résoudre d'autres choses qui dépendent de la physique... Ça aide à structurer la logique de jeu.

La plupart des moteur que j'ai vu (Unity, Unreal, ...) fonctionnent plus ou moins sur ce principe, avec des variantes. Dans l'ensemble il semble que c'est un bon moyen de structurer le code d'un jeu. Du coup j'aimerais bien tester cette approche plus en détail. C'est pour ça que j'ai proposé de l'utiliser pour USB Warrior et je compte pousser mes expériences plus loin avec ce moteur.

Je ne sais pas encore très bien comment gérer les détails ici. Il est possible qu'on se retrouve à tester plusieurs implémentations qui fonctionnent différemment. A priori aucun module ne dépendra de celui-ci (sauf les jeux en eux-même et les sous-modules) ce qui permet de tout jeter et de repartir de zéro si besoin.

Bref, ce qui suit sont des notes semi-cohérentes sur la façon dont je vois les choses, qui est elle-même fort susceptible d'évoluer.


### Hiérarchie

Les objets devraient être structurés sous la forme d'un [scenegraph](https://en.wikipedia.org/wiki/Scenegraph). C'est une structure de donné classique pour représenter des scènes. En gros, on crée une hiérarchie d'objets. Chaque objet hérite de la position/orientation de son parent. Avec ce système, on peut faire que le bras d'un perso soit un fils de celui-ci: il se déplace donc avec le perso, mais ajoute son propre décalage/rotation. Bonus: l'objet que tiens le perso peut être ajouté en tant que fils du bras, ce qui fait qu'il est automatiquement bien positionné.

Ainsi, le système de hiérarchie permet de créer de objets encore plus complexes. Il y a cependant des limites : si un objet doit jouer plusieurs sons, il lui faut plusieurs fils, ce qui n'est ni intuitif, ni pratique. On a plusieurs approches pour contourner le problème.

1. Permettre d'avoir plusieurs composants du même type. Par exemple, chaque SpriteComponent peut contenir un offset par rapport à son objet, ce qui permet d'avoir plusieurs sprite pour le même objet. En fait c'est déjà plus ou moins le cas.
2. Certains composants peuvent implémenter eux-même la possibilité de représenter plusieurs choses. Par exemple un seul SpriteComponent peut permettre d'afficher plusieurs sprites. Problème : ça complique l'implémentation de ces composants.
3. On utilise systématiquement la hiérarchie, quitte à ajouter des fils qui ne contiennent que le composant supplémentaire dont on a besoin.
4. Un mélange de tout ça.

En ce moment je penche plus pour la solution 1, plus simple à mettre en œuvre.

Certaines implémentations forment un DAG où un même objet peut avoir plusieurs parents. Ça signifie que le même objet apparaît plusieurs fois dans la scène. C'est une façon de représenter l'instancing. Cependant ça complique pas mal les choses, donc dans un premier temps je propose de structurer la scène avec un bête arbre.

À noter que la hiérarchie du scenegraph ne sert pas qu'à faire hériter des objets de la position d'autres objets. Un autre but important est l'optimisation. Par exemple, en regroupant les objets proches les uns des autre sous un même nœud, on peut les éliminer d'un bloc si il s'avère que ce groupe d'objet n'est pas visible lors du rendu.


### Évenements

Dans un jeu, la notion d'événement est primordiale. Par exemple, certaines choses peuvent se produire quand un certain nombre d'ennemis d'un certain type sont morts. Par exemple, le PJ passe en mode berserk ou un succès peut être débloqué. Il est peu pratique d'ajouter la logique qui gère ces cas dans le code des ennemis. Une solution plus pratique est de déclencher un événement lorsqu'un ennemi meurt. Les intéressés peuvent alors écouter cet événement et faire leur boulot sans pourrir le code des ennemis.

Une façon de mettre ça en œuvre est d'utiliser un système de signal/slot : lorsque l'événement se produit, le signal est émit et tout les slots associés (des callbacks) sont appelés. Potentiellement, on peut faire passer les événement par une file d'événements si nécessaire (même si en l'occurrence je ne vois pas l'utilité dans l'immédiat).


### Propriétés

Chaque objet ou composant est constitué de propriétés : la position des objets, le sprite affiché, la quantité de point de vie d'un perso, etc. Il peut être utile de mettre en œuvre un système pour accéder à la liste des propriétés d'un objet à l'exécution. Cela peut permettre, par exemple :

- D'avoir un bout de code générique qui charge les propriétés des objets depuis des fichiers de config et/ou la map.
- De charger/sauvegarder les propriétés des objets pour les jeux qui nécessitent des sauvegardes.
- D'implémenter un langage de script simple (ou d'en utiliser un existant) pour modifier les propriétés d'un objet. (Scripts probablement déclenchés par des événements.)
- D'automatiser le code de réplication si on veut faire des jeux en réseau.

Certaines propriétés sont nécessaires au fonctionnement de beaucoup de composants, comme la position. Cela peut soit être représenté par un composant dont plein d'autres composants dépendent, soit être une propriété de l'objet directement. (Je penche pour la solution 2.)

Certaines propriétés peuvent utiliser du double-buffering. C'est à dire qu'on connaît leur valeur à la frame courante ainsi qu'à la frame précédente. C'est nécessaire pour certaines propriété dans le cas d'une boucle de jeu dont la fréquence des update est découplés de la fréquence des rendu, afin de pouvoir les interpoler.

Une autre utilité potentielle du double-buffering est d'éviter les race-condition. Par exemple, dans un jeu de baston il est souvent possible de prendre des dégâts sur les hitbox de ses propres attaques si l'adversaire les frappe avec une attaque de priorité supérieure ou égale. Un code naïf updaterait d'abord le perso A : il détecterait que ce perso frappe l'adversaire, lui ferait des dégâts ce qui annulerait son coup et donc ses dégâts. Si par contre le code d'update ne dépend que de l'état du jeu à la frame précédente, les deux coups passent puisque les dégâts infligé au perso B ne seront pris en compte qu'à la frame suivante.

Avantage bonus du double-buffering : ça facilite le multi-threading puisque les donnés de la frame précédente sont en lecture-seule. (Oui, d'accord, on en est pas là.)


### Scripting

Si on décide qu'il est plus pratique de coder la logique de jeu en utilisant un langage de script, c'est principalement ce module qui doit être accessible depuis celui-ci. Par exemple, on peut imaginer implémenter les composants spécifiques à chaque jeux en python, tout en ayant les composants "système" nécessitant des calculs plus coûteux codés en C++. (Oui, on en est pas encore là non plus.)


### Hiérarchie des propriétés

Beaucoup d'objets dans un jeu sont similaires. Par exemple, on peut avoir plusieurs monstres qui partage la même logique mais qui ont des propriétés différentes (force, sprite, vitesse, etc.). Une façon de gérer ça est de simplement créer un objet qui sert de modèle pour chaque type de monstre. Ses objets seraient toujours désactivé. Pour crée un gobelin, il suffit alors de cloner le modèle de gobelin (ce que le système de propriété peut potentiellement faire automatiquement).

D'une certaine façon, le gobelin hérite de son modèle. On peut représenter cette hiérarchie de modèles (rien à voir avec la hiérarchie du scènegraph) de façon explicite. Cela peut permettre d'avoir des propriétés qui ont par défaut la valeur de leur parent, sauf si elles sont explicitement modifié. Il devient alors possible de changer d'un coup, par exemple, la force de tous les gobelins juste en modifiant le modèle.

Bon, là je pense qu'il faut encore y réfléchir, et je ne suis pas sur que ce soit si utile. Ça peut par contre combotre (!) avec le système d'événement : on pourrait par exemple enregistrer un event-listener au modèle monstre pour recevoir un événement quel que soit le type de monstre qui le déclenche, ou juste au modèle de gobelin pour ne recevoir l'événement que si il concerne un gobelin, ou encore à un monstre en particulier.


### Implémentation

Il y aura des "sous-modules": typiquement un pour le son, un pour le rendu, un pour la physique... Il s'occuperont de faire la jonction entre le système d'objet et les modules correspondants en fournissant les composant associés (SpriteComponent, SoundComponent, etc.). Dans un premier temps il n'y aura que le sous module de rendu qui s'occupera de l'affichage (fait par la classe Scene dans USB Warrior).

La hiérarchie d'objets, et éventuellement les composants, seront gérés par un objet scène. Outre s'occuper de la gestion mémoire, la scène permet d'accéder aux objets de diverses façon (par nom, par type, etc. On verra en fonction des besoins).

Dans un premier temps, on peut faire simple. Les objets sont alloués par la scène et gèrent eux-même leurs composants. Chaque objet peut contenir une map de composant, un peu comme c'était le cas pour USB Warrior. Sauf que je pense utiliser une chaîne de caractère comme identifiant et utiliser un système de template (simple, promis) pour garantir la type-safety et éviter d'avoir à faire les casts sois-même.

Je pense essayer d'implémenter un système pour gérer les propriétés. Chaque objet possédant des propriétés héritera d'une classe PropertyContainer. Cette classe possédera un attribut pointant vers un PropertySet : un objet qui décrit l'ensemble des propriétés d'un type de composant donné, ainsi que leur type et toutes les méta-informations qu'on peut vouloir. Chaque type de composant aura son propre objet PropertySet et toutes les instance d'un composant pointeront vers le même. Je suis même en train de me dire qu'on peut ne pas se limiter aux propriétés mais aussi ajouter des méthodes pour faciliter le scripting (faut y réfléchir).

La plupart des objets auront un ensemble de propriétés fixe, représenté par une simple structure C ou classe C++. Il peut être intéressant de pouvoir ajouter à la volé des propriétés custom à certains objets. L'usage auquel je pense est tout simplement la gestion des composants. Chaque composant d'un GameObject pourrait en pratique être une propriété dynamique, ce qui permet d'avoir une façon uniforme d'accéder à un objet. Et par ailleurs le système de propriété posséderait alors toutes les informations nécessaires pour automatiser complètement la serialisation/déserialisation des objets.

Bon, j'ai déjà bien digressé et je pense que plus personne ne suit là, donc je pense que je vais tenter d'implémenter ça et on verra bien ce que ça donne.


### Optimisation

L'optimisation n'est pas vraiment un but premier. On ne devrait pas faire de jeux trop gourmand en ressource de si tôt. Cependant ça reste une bonne idée d'y penser pour se simplifier la vie à l'avenir.

Dans le but de pouvoir jouer à optimiser l'utilisation du cache et de la gestion mémoire, ça peut être bien de faire en sorte de pouvoir déplacer les objets/composants en mémoire. Ça veut dire ne jamais stocker de pointeur vers les objets/composants mais une poignée quelconque.

