#include<GL/glut.h>
#include<stdlib.h>
#include<stdio.h>

#define screen_width 500
#define screen_height 500

#define x_start 100
#define y_start 100

GLint x_prev;
GLint y_prev;

GLfloat red = 0.0f;
GLfloat green = 0.0f;
GLfloat blue = 0.0f;

int click_menu = 0;
GLint stroke_width = 1;

void save_svg();
typedef struct ln {
	GLint xi;
	GLint yi;
	
	GLint xf;
	GLint yf;
	
	GLfloat r;
	GLfloat g;
	GLfloat b;
	
	GLint stroke;
	
	void * next;
} line;
line * lines = NULL;
line * last = NULL;

void handle_keyboard (GLubyte key, GLint x, GLint y) {
	printf("'%c'\n", key);
	if(key == 'D' || key == 'd') {
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();		
		while(lines != NULL) {
			last = lines;
			lines = last->next;
			free(last);
		}		
	} else if(key == 'S' || key == 's') 	
		save_svg();
}

void handle_click_mouse (GLint button, GLint action, GLint x, GLint y) {
//	printf("%d %d\t%d %d\n",x,y,action,button);	
	
	if(action == GLUT_DOWN) {
		x_prev = x;
		y_prev = y;
	} 	
}

void handle_drag_mouse (GLint x, GLint y) {
//	printf("%d %d\n",x,y);
	
	if(click_menu == 0) {			
		glBegin(GL_LINES);
			glVertex2i(x_prev,screen_height - y_prev);		
			glVertex2i(x,screen_height - y);				
		glEnd();
		glFlush();	
		
		line * l = malloc(sizeof(line));
		if(lines == NULL)
			lines = l;
		else last->next = l; 	
		last = l;
		l->next = NULL;

		l->xi = x;
		l->xf = x_prev;		
		
		l->yi = y;
		l->yf = y_prev;		
		
		l->r = red;
		l->g = green;
		l->b = blue;
		
		l->stroke = stroke_width;
	} else click_menu = 0;

	x_prev = x;
	y_prev = y;	
}

//void handle_move_mouse (GLint x, GLint y) {}

void handle_color_menu (GLint option) {
	int c;
	click_menu = 1;
	
	for(c = 0; c < 3; c++) {		
		blue = green;
		green = red;
		red = option & 1;
		option = option >> 1;
	}
	glColor3f(red, green, blue);	
	printf("New color:\t%f %f %f\n",red,green,blue);
}

void handle_stroke_menu (GLint option) {
	click_menu = 1;
	stroke_width = option;
	glLineWidth(stroke_width);
	printf("New stroke:\t%d\n",stroke_width);
}

void handle_menu (GLint option) {
	click_menu = 1;
	printf("%d\n",option);
}

void menu () {
	GLint color_menu = glutCreateMenu(handle_color_menu);
	
	char color[2];
	color[1] = '\0';
	
	int c;
	for(c = 0; c < 8; c++) {
		color[0] = '0' + c;
		glutAddMenuEntry(color, c);
	}
	
	GLint stroke_menu = glutCreateMenu(handle_stroke_menu); 
	for(c = 1; c <= 8; c = c << 1) {
		color[0] = '0' + c;
		glutAddMenuEntry(color, c);
	}
	
	GLint stroke = glutCreateMenu(handle_menu);
	glutAddSubMenu("Color", color_menu);
	glutAddSubMenu("Stroke", stroke_menu);		
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void display (void) {
//	printf("Starting....\n");	
	glFlush();
	
//	printf("Started!\n");
	glColor3f(red, green, blue); // black lines  		 
	
}

int main (int argc, char **argv) {
	
	printf("Right click:\tSelect Color\nS:\tSave lines as SVG\nD:\tDelete all lines\n\n");		
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	
	glutInitWindowSize(screen_width, screen_height);
	
	glutInitWindowPosition(x_start, y_start); 
	glutCreateWindow("Pafloat");
	
	glClearColor(1.0,1.0,1.0,0.0); // white background  		 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 
	gluOrtho2D(0.0, screen_width, 0.0, screen_height);
	
	glutMouseFunc(handle_click_mouse);
	glutMotionFunc(handle_drag_mouse);
	//glutPassiveMotionFunc(handle_move_mouse);
	glutKeyboardFunc(handle_keyboard);
	
	menu();
	
	glClear(GL_COLOR_BUFFER_BIT); 
	glutDisplayFunc(display); 
	glutMainLoop();
	
	
	return 0;
}

void save_svg () {
	if(lines == NULL)
		return;
	
	char arq[8]; 
	arq[7] = '\0';
	arq[3] = '.';
	arq[4] = 'S';
	arq[5] = 'V';
	arq[6] = 'G';
	
	FILE * a;
	int c, d, b;
	for(c = 0; c < 1000; c++) {
		for(d = 2, b = c; d >= 0; d--, b /= 10) 
			arq[d] = '0' + (b % 10);
		printf("%s\n",arq);	
		
		a = fopen(arq, "r");
		if(a == NULL)
			break;
		fclose(a);	
	}
	
	a = fopen(arq, "w");
		fprintf(a, "<?xml version='1.0' standalone='no'?>\n<svg version='1.1' \nxmlns='http://www.w3.org/2000/svg'\nxmlns:svg='http://www.w3.org/2000/svg'\nxmlns:xlink='http://www.w3.org/1999/xlink'>\n");
		line * l = lines;
		c = 0;
		while(l != NULL) {
			//printf("%d %d\t %d %d\n",l->xi,l->yi,l->xf,l->yf);
			fprintf(a, "<line x1='%d' y1='%d'\t x2='%d' y2='%d' style='stroke-width:%d;stroke:rgb(%d,%d,%d)'/>\n",l->xi,l->yi,l->xf,l->yf,l->stroke,(int)(255 * l->r),(int)(255 * l->g),(int)(255 * l->b));
			l = l->next;
			c++;
		}
		fprintf(a, "</svg>");
	fclose(a);
	printf("%d lines saved.\n",c);
}
