# Projet HPC - Detection de mouvement

## Auteurs : Julien GENTY - Youssef LAKHDHAR 


### Généralités
**Telecharger le fichier `car3.zip` (en bas de la page) et le decompresser dans votre dossier principal. Les deux autres libraireies `MIPP` et `nrc2` sont fournies dans l'archive'**
**Code de base** : `optimMouvementSIMD.c`
**Pour changer le code** (par exemple : utiliser le code sans optimisation) :
⇒ Copier-coller le code souhaité du dossier `alt-code` dans le dossier principal et enlevez le fichier précédent de ce dossier.**
**Pour changer le flag de compilation :**
⇒ Ouvrir `Makefile` et changer le paramètre `-Ox` en remplacant *x* par le niveau d'optimisation souhaité

### Executions 
***Compiler et executer le code :*** `make && ./execHPC `
***Supprimer les fichiers*** *`*.o`* ***et*** *executables* ***:*** `make clean`
***Supprimer les fichiers existants, recompiler et executer le code :*** `make clean && make && ./execHPC `

### Informations
***Fichiers sources et leur fonctionnalités :***

- `mouvement.c` ⇒ code sans amélioration 
	Appel de fonction et maximum de boucle `for`
-  `optimMouvement.c` ⇒ optimisation de base
	Inlining et réduction des boucles `for`
- `optimMouvementThread.c` ⇒ optimisation par multi-threading 
- `optimMouvementSIMD.c` ⇒ optimisation par la méthode SIMD avec la libraire *MIPP* 

### Librairies
**Sources des libraires :**
 
- MIPP : [aff3ct/MIPP](https://github.com/aff3ct/MIPP)
- nrc2 :  [alsoc/nrc2](https://github.com/alsoc/nrc2)
- Banque d'image `car3` : [car3.zip](http://www-soc.lip6.fr/~lacas/ImageProcessing/MotionDetection/car3.zip)

