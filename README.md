# akinator
Educational project at MIPT. It is mostly made for _fun_, but it uses my own `tree` library, which is the main educational purpose of this project.

## Build
Repo includes `makefile`, so you can build `.exe` using command `make`.

## Brief description
This program allows to create `knowledge databases`, which are represented by binary trees. Each leaf is an object, each non-leaf node is an attribute; if the object you are looking for 
has the attribute, it is located in the right subtree of this node, otherwise in the left.

## Key features
1. `Guessing objects` User thinks of an object, program asks questions to the user concerning its atrributes. If it leads to an object
in the database, program names it; otherwise program suggests to add the object to the database.
2. `Definition of an object` User names the object, the program gives its definition, according to the database.
3. `Comparing two objects` User names two objects, the program gives the comparison of their definitions.
4. `Show database` The program generates image of the database using `graphviz dot` program, which must be installed and specified in PATH in order for this to work.
5. The program has a temper! Because it is a fun project. Also, it is connected with one of the characters of Steven King's "The Dark Tower".

### An example of a database image:
![tmp](https://github.com/Feanor-19/akinator/assets/142310227/ae4a37f3-4551-4b2b-a48d-63ad6e180a40)
