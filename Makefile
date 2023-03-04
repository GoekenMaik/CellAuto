all:
	gcc controller.c model.c view.c -o CellAuto -lSDL2 -lSDL2_image

clean:
	rm CellAuto