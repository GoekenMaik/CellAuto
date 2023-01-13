all:
	gcc controller.c model.c view.c -o CellAuto -lSDL2

clean:
	rm CellAuto