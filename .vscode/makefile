#source file search paths
VPATH = ./src ./models/basic ../raven-set ../../code_ext/sqlite

# compiler include search paths
INCS = \
	-I./src \
	-I./src/ext \
	-I./models \
	-I./models/basic \
	-I../windex/include \
	-I../raven-set \

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs -lComctl32 \


static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_ -Dvase

# sources

OBJ_recipe = \
	$(ODIR)/cRecipeGUI.o \
	$(ODIR)/cVase.o \
	$(ODIR)/flower.o \
	$(ODIR)/raven_sqlite.o \
	$(ODIR)/sqlite3.o \
	$(ODIR)/cxy.o \

### COMPILE

$(ODIR)/sqlite3.o : 
	gcc	 -o $@ -c ../../code_ext/sqlite/sqlite3.c

$(ODIR)/%.o: %.cpp  
	g++  $(CCflags) \
	-c -o $@ $< $(INCS) 


recipe: $(OBJ_recipe)
	g++ $(OPT) \
	-o bin/recipe.exe $^  \
	$(static_link_flags) $(LIBS)


.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

rebuild: clean recipe

