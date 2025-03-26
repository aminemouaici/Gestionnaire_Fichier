# Définition du compilateur
CC = gcc
CFLAGS = -Wall -Wextra -g  # Options de compilation : warnings et debug

# Définition des fichiers sources et objets
SRC = main.c \
      Bib/Bib.c \
      ListeInode/ListeInode.c \
	  ListePage/ListePage.c
      

OBJ = $(SRC:.c=.o)

# Nom de l'exécutable
EXEC = programme

# Compilation de l'exécutable
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Compilation des fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage des fichiers objets et de l'exécutable
clean:
	rm -f $(OBJ) $(EXEC)

# Pour recompiler tout
rebuild: clean all
