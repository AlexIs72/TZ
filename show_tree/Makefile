TARGET = show_tree
CFLAGS = -MD -I include
SRC = main.cpp simple_tree.cpp simple_tree_node.cpp
OBJ = $(SRC:.cpp=.o)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@ 
 
clean:
	rm -f *.o *.d $(TARGET)

run: $(TARGET)
	./$(TARGET) 

gen:
	./gen_tree.py

-include *.d
