
# 🧾 Manuel d'utilisation — Système de fichiers virtuel

Ce manuel présente pas à pas les instructions nécessaires pour utiliser le système de fichiers virtuel, depuis la décompression jusqu’à la navigation, la manipulation de fichiers et la fermeture de l’environnement.

---

## 🛠️ Prérequis et environnement nécessaire

Avant d’installer et d’utiliser le système de fichiers virtuel, assurez-vous de disposer de :

- Un système d’exploitation Linux (ou compatible POSIX)
- Le compilateur `gcc` installé *(optionnel)*
- L’outil `make`
- L’archive `systeme_fichier.tgz`

---

## 1. 📁 Se positionner dans le répertoire contenant l'archive

```bash
cd ~/chemin/vers/dossier
```

---

## 2. 📦 Décompression de l'archive `.tgz`

```bash
tar -xvzf systeme_fichier.tgz
```

**Explication des options :**

- `-x` : extraire les fichiers
- `-v` : mode verbeux (affiche les fichiers extraits)
- `-z` : gzip
- `-f` : suivre du nom de l’archive

**Résultat :** un dossier `systeme_fichier/` est créé avec les fichiers du projet.

---

## 3. 📂 Accéder au dossier du projet

```bash
cd systeme_fichier
```

---

## 4. ⚙️ Compilation du projet

### Avec `Makefile` :

```bash
make
```

### Avec `gcc` :

```bash
gcc -o programme main.c Bib/Bib.c
```

**Résultat :** un exécutable `programme` est généré.

---

## 5. 🚀 Lancement du système de fichiers

```bash
./programme
```

**Résultat :** vous entrez dans l’environnement du système de fichiers virtuel.

```
[root@myfs /]$
```

---

## 6. 📚 Liste des commandes disponibles

### 🔹 `help`
Affiche la liste des commandes disponibles.

```bash
help
```

---

### 🔹 `ls [chemin]`
Liste le contenu d’un répertoire.

```bash
ls
ls /dossier1
```

---


### 🔹 `mkdir chemin_vers_répertoire`
Crée un répertoire.

```bash
mkdir /rep1/new_rep
```

---

### 🔹 `touch chemin_vers_fichier`
Crée un fichier vide.

```bash
touch ./rep1/fichier.txt
```

---

### 🔹 `cd chemin_vers_répertoire`
Change de répertoire.

```bash
cd ./rep1
cd ..
```

---

### 🔹 `rm chemin_vers_fichier_ou_répertoire`
Supprime un fichier ou un répertoire.

```bash
rm ./rep1
rm /rep1/fichier.txt
```

---

### 🔹 `ln -s src dest`
Crée un lien symbolique.
src : fichier ou  répertoire cible.
dst : nom du lien.

```bash
ln -s /data/org.txt ./nom_lien
```

---

### 🔹 `chmod mode chemin`
Modifie les permissions d’un fichier.
mode : numérique (3 chiffres).
chemin : chemin complet du fichier ou répertoire.

```bash
chmod 755 ./script.sh
```

---

### 🔹 `pwd`
Affiche le chemin courant.

```bash
pwd
```

---

### 🔹 `cp src dest`
Copie un fichier ou un répertoire.
src : fichier ou  répertoire cible.
dest : chemin complet du  répertoire destination.

```bash
cp ./notes.txt ./copie.txt
```

---

### 🔹 `mv src dest`
Déplace ou renomme un fichier.
src : fichier ou  répertoire cible
dest : chemin complet du  répertoire destination

```bash
mv ./notes.txt ./dossier/
```

---

### 🔹 `write chemin_vers_répertoire`
Ouvre un fichier en mode écriture.

```bash
write ./journal.txt
```

---

### 🔹 `cat chemin_vers_répertoire`
Affiche le contenu d’un fichier.

```bash
cat /home/utilisateur/journal.txt
```

---

### 🔹 `ln src dest`
Crée un lien physique.
src: chemin absolu vers le le répertoire source.
dest: chemin absolu vers le lien physique.

```bash
ln /data/org.txt /data/nom_lien
```

---

### 🔹 `exit`
Sauvegarde et quitte l’environnement virtuel.

```bash
exit
```

---

## ✍️ Utilisation de la commande `write`

Une fois la commande `write` tapée dans le terminal virtuel, une interface d’écriture s’affiche.

> Après avoir terminé, revenir à une nouvelle ligne, taper **CTRL+X**, puis appuyer sur **Entrée** pour valider la sortie et sauvegarder.

---

## 7. 💾 Fermeture et sauvegarde

Quand vous avez terminé votre session, tapez :

```bash
exit
```

Toutes les modifications sont enregistrées dans le fichier de partition avant la fermeture du programme.
