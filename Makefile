house:
	gcc paint.c -o paint -lglut -lGLU -lGL
	
clean:	
	rm paint
	
clear:
	make clean 	
