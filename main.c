#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

/***** NAPOMENA ****/
/*
 Kod za 
 1. on_mouse_motion preuzet i prilagodjen sa https://learnopengl.com/book/offline%20learnopengl.pdf
 2. ispis teksta preuzet od koleginice Andjelke Milovanovic
 3. pucanje implementirano takodje zahvaljujuci koleginici Andjelki Milovanovic

 */

/* Dimenzije prozora */
static int window_width, window_height;

/* Deklaracije callback funkcija. */
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_display(void);
static void on_mouse(int button, int state, int x, int y);
static void on_mouse_motion(int x, int y);
void material_and_light();

/* Ugao rotacije oko y ose za pravac kamere*/
float angleY = 0.0;
/* Ugao rotacije oko x ose za pravac kamere*/
float angleX = 0.0;
/* Vektor koji predstavlja pravac kamere */
float kx = 0.0f;
float ky = 0.0f;
float kz = -1.0f;

/* Pozicija kamere */
float x = 0.0f;
float y = 0.0f;
float z = 4.0f;

/* Pozicija misa*/
GLfloat lastX;
GLfloat lastY;
/* Osetljivost misa da ne leti po ekranu*/
float sensitivity = 0.2;

/* Tajemer za odbrojavanje do kraja igre */
int game_over = 0;
time_t Time = 0;
GLfloat diff_time;
GLfloat game_time = 0;
GLfloat beginTime;
char disp_time[1000];
int timing;
/* Fleg koji odredjuje stanje tajmera*/
static int timer_active;

/* Iscrtavanje kocki */
float positionx[17];
float positionz[17];
float positiony[17];


/* Tajmer za ispucavanje kuglice */
float t = 0;
static int move_ball = 0;
/* Pozicija obojene kuglice */
float x_ball; 
float y_ball; 
float z_ball; 
/* Vektor pravca obojene kuglice */
float bx = 0.0f;
float by = 0.0f;
float bz = -1.0f;
/* brzina kuglice */
float v = 7.0f;

static void draw_coordinate_system();
static void draw_box();
void cubepositions();
void cube();
static void on_time(int value);
void drawBitmapText();
void paintball();
static void on_mouse(int button, int state, int x, int y);
void moving_ball(int value);

int main(int argc, char **argv)
{
	srand(time(NULL));
	/* Inicijalizuje se GLUT. */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	
	/* Kreira se prozor. */
	glutInitWindowSize(750, 650);
	glutInitWindowPosition(350, 100);
	glutCreateWindow("Box shooter");
	
	glutWarpPointer(glutGet(GLUT_SCREEN_WIDTH)/2, glutGet(GLUT_SCREEN_HEIGHT)/2);
		
	/* Registruju se callback funkcije. */
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);
	glutMouseFunc(on_mouse);
	glutPassiveMotionFunc(on_mouse_motion);
	
	
	/* Obavlja se OpenGL inicijalizacija. */
	glClearColor(0.4, 0.3, 0.5, 0.5);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(2);
		
	cubepositions();
	
	/* Program ulazi u glavnu petlju. */
	glutMainLoop();
	
	return 0;
}

static void on_keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
			/* Zavrsava se program. */
			exit(0);
			break;
		case 'g':
		case 'G':
			if(!timer_active){
				glutTimerFunc(0, on_time, 0);
				beginTime = glutGet(GLUT_ELAPSED_TIME);
				timer_active = 1;
			}
			break;
		default:
			break;
	}
}

static void on_time(int value)
{
	/* Provera da li callback dolazi od ogovarajuceg tajmera */
	if(value != 0)
		return;
	
	float newTime = glutGet(GLUT_ELAPSED_TIME);
	diff_time = (newTime - beginTime)/1000.;
	beginTime = newTime; 
	game_time += diff_time;
	
	int timing = sprintf(disp_time,"%.2f", game_time);
	if(timing < 0)
		exit(1);
	
	
	
	printf("%.2lf\n", game_time);
	
	if(game_time >= 30.00){
		game_over = 1;
		timer_active = 0;
		/*glutIdleFunc(NULL);*/
		glutPassiveMotionFunc(NULL);
	}
	
	
	if(timer_active)
		glutTimerFunc(0, on_time, 0);
}

static void on_reshape(int width, int height)
{
	/* Pamte se sirina i visina prozora. */
	window_width = width;
	window_height = height;
	
	/* Podesava se viewport. */
	glViewport(0, 0, window_width, window_height);
}

void drawBitmapText() 
{  
	glMatrixMode(GL_PROJECTION); 
	glPushMatrix();  
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); 
	glPushMatrix(); 
	glLoadIdentity();
	glColor3f(1, 0, 0);
	gluOrtho2D(0.0, window_width, window_height, 0.0);                 
	char display_string[32];
	int words = sprintf(display_string,"%s", "Time:");
	if(words < 0)
		exit(1);
	glRasterPos2i(window_width/2, window_height/2); 
	int d = (int) strlen(display_string);
	for (int i = 0; i < d; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, display_string[i]);
	glRasterPos2i(window_width/2+0.5, window_height/2+15); 
	int l = (int) strlen(disp_time);
	for (int i = 0; i < l; i++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, disp_time[i]);
	
	glMatrixMode(GL_PROJECTION); 
	glPopMatrix(); 
	glMatrixMode(GL_MODELVIEW); 
	glPopMatrix(); 
	glutPostRedisplay();
}

static void on_display(void)
{
	/* Brise se prethodni sadrzaj prozora. */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	/* Podesava se projekcija. */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		60,
		1,
		0.0,
		10.0
	);
	/* Namestanje tacke pogleda kamere*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, 1.0f, z, 
		         x+kx, 1.0+ky, z+kz,
			 0.0f, 1.0f, 0.0f
 		);
	glDisable(GL_DEPTH_TEST);
	
	draw_coordinate_system();
	draw_box();
	
	cube();
	
	x_ball = x;
	y_ball = y;
	z_ball = z;
	
	paintball();
	
	/* Ispisivanje vremena */
	drawBitmapText();
	
	glutPostRedisplay();
	
	
	/* Slanje novog sadrzaja na ekran*/
	glutSwapBuffers();
}

/* Postavlja slucajnu poziciju kocke */
void cubepositions (void) {
	
	for (int i=0;i<17;i++)
	{
		positionx[i] = rand()/(float)RAND_MAX*4.5;
		positiony[i] = rand()/(float)RAND_MAX*4.5;
		positionz[i] = rand()/(float)RAND_MAX;
	}
}

void cube (void) {
	
	for (int i=0;i<8-1;i++)
	{
		
		glPushMatrix();
		glColor3f(0.4, 1, 0.6);
		glTranslated(-positionx[i + 1], positiony[i + 1], positionz[i + 1]);
		glutSolidCube(0.5);
		glPopMatrix();
	}
	
	for (int i=8;i<17-1;i++)
	{
		
		glPushMatrix();
		glColor3f(0.4, 1, 0.6);
		glTranslated(positionx[i + 1], positiony[i + 1], -positionz[i + 1]);
		glutSolidCube(0.5); 
		glPopMatrix();
	}
}

static void draw_box()
{
	/*Iscrtavanje poda*/
	glColor3f(0.3, 0.3, 0.3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
		glVertex3f(4.5, 0, 4.5);
		glVertex3f(4.5, 0, -4.5);
		glVertex3f(-4.5, 0, -4.5);
		glVertex3f(-4.5, 0, 4.5);
	glEnd();
	
	/* Iscrtavanje levog zida */
	glColor3f(0.5, 0.5, 0.5);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
		glVertex3f(-4.5, 0, 4.5);
		glVertex3f(-4.5, 0, -4.5);
		glVertex3f(-4.5, 5, -4.5);
		glVertex3f(-4.5, 5, 4.5);
	glEnd();
	
	/* Iscrtavanje prednjeg zida */
	glColor3f(0.5, 0.5, 0.5);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
		glVertex3f(-4.5, 0, -4.5);
		glVertex3f(4.5, 0, -4.5);
		glVertex3f(4.5, 5, -4.5);
		glVertex3f(-4.5, 5, -4.5);
	glEnd();
	
	/* Iscrtavanje desnog zida */
	glColor3f(0.5, 0.5, 0.5);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
		glVertex3f(4.5, 0, -4.5);
		glVertex3f(4.5, 5, -4.5);
		glVertex3f(4.5, 5, 4.5);
		glVertex3f(4.5, 0, 4.5);
	glEnd();
	
	/* Iscrtavanje plafona */
	glColor3f(0.3, 0.3, 0.3);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
		glVertex3f(4.5, 5, 5);
		glVertex3f(4.5, 5, -4.5);
		glVertex3f(-4.5, 5, -4.5);
		glVertex3f(-4.5, 5, 5);
	glEnd();
	
	
}

static void draw_coordinate_system()
{
	
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(-5, 0, 0);
		glVertex3f(100, 0, 0);
		
		glColor3f(0, 1, 0);
		glVertex3f(0, -5, 0);
		glVertex3f(0, 100, 0);
		
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, -5);
		glVertex3f(0, 0, 100);
		
	glEnd();
}

/* Crtam kuglicu u boji. Boja se menja sa promenom koeficijenta pravca :D! */
void paintball(){
	glPushMatrix();
	glColor3f(1, 1, 1);
	
	x_ball = x_ball+t*bx*v;
	/* ovde 0.9 posteriori, da ne puca iz noge, a ni iz glave! */
	y_ball = y_ball+0.9f+t*by*v; 
	z_ball = z_ball+t*bz*v;
	
	glTranslatef(x_ball, y_ball, z_ball);
	glutSolidSphere(0.05f, 30, 30);
	glPopMatrix();
}

static void on_mouse(int button, int state, int x, int y){
	(void)x;
	(void)y;
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){
				if(!move_ball && timer_active == 1){
					bx = kx;
					by = ky;
					bz = kz;
					t = 0;
					glutTimerFunc(30, moving_ball, 1);
					move_ball = 1; 
				}
			}		
			break;
		case GLUT_RIGHT_BUTTON:
			if(state == GLUT_DOWN){
				//printf("Pritisnut je desni klik\n"); 
				glutSetCursor(GLUT_CURSOR_TOP_SIDE);
			}
			break;
		default:
			break;
	}
}
void moving_ball(int value){
	if (value != 1)
		return;
	
	/* Ako je loptica izasla iz sobe -> nema vise loptice */
	if(x_ball > 5.0f || x_ball < -5.5f || 
		y_ball > 5.0f || y_ball < 0.0f  || 
		z_ball > 5.0f || z_ball < -5.0f){
		move_ball = 0;
	t = 0;
	return;        
		}
	
	t += .2f;
	if (move_ball)
		glutTimerFunc(30, moving_ball, value);
}

static void on_mouse_motion(int x, int y){
	
	glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
	// Calculate the offset movement between the last and current
	// frame
	GLfloat xoffset = x - lastX;
	GLfloat yoffset = lastY - y; // reversed since y-coordinates range from bottom to top
	
	lastX = x;
	lastY = y;
	
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	
	angleX += yoffset;
	angleY += xoffset;
	
	if(angleX > 45.0f)
		angleX = 45.0f;
	if(angleX < -20.0f)
		angleX = -20.0f;
	if(angleY > -45.0f)
		angleY = -45.0f;
	if(angleY < -135.0f)
		angleY = -135.0f;

	kx = cos(M_PI/180.0*angleX)*cos(M_PI/180.0*angleY);
	ky = sin(M_PI/180.0*angleX);
	kz = cos(M_PI/180.0*angleX) * sin(M_PI/180.0*angleY);
}

void material_and_light(){
	/* Parametri svetla i podrazumevanog materijala */
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };
	GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
	GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
	
	GLfloat ambient_coeffs[] = { 0.3, 0.7, 0.3, 1 };
	GLfloat diffuse_coeffs[] = { 0.2, 1, 0.2, 1 };
	GLfloat specular_coeffs[] = { 1, 1, 1, 1 };
	GLfloat shininess = 30;   
	
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	
	/* Postavljaju se svojstva materijala */
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}