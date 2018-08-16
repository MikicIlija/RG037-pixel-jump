#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>

#define TIMER_ID 0
#define TIMER_INTERVAL 20
#define MOVEMENT_SPEED 0.03 // brzina kratanja
/* Dimenzije prozora */
static int window_width, window_height;

const static float size = 0.1; // Velicina kvadrata
static float curr_X,curr_Y; // tekuce koordinate centra kvadrata
static float v_x,v_y; //smer kretanja
static int animation_ongoing; //fleg koji pokazduje da li je animacia u toku
static void draw_player();

// Deklaracije callback funkcija
static void on_keyboard(unsigned char key,int x, int y);
static void on_timer(int value);
static void on_reshape(int width, int height);
static void on_display(void);

int main(int argc,char** argv){
  //Inicijalicija za GLUT
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

  //Kreiramo prozor
  glutInitWindowSize(450,600);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  //Registruju se funkcije za obradu dogadjaja
  glutKeyboardFunc(on_keyboard);
  glutReshapeFunc(on_reshape);
  glutDisplayFunc(on_display);


  //na pocetku postavljamo igraca na sredinu poda
  curr_X = -(1 - size / 2)/20;
  curr_Y = -(1 - size / 2);

  //inicijalizacija smera kretanja
  v_x = MOVEMENT_SPEED;
  v_y = MOVEMENT_SPEED;

  //na pocetku animacija stoji
  animation_ongoing = 0;

  //OpenGL inicijalizacija
  glClearColor(0.75,0.75,0.75,0);
  glEnable(GL_DEPTH_TEST);

  glutMainLoop();

  return 0;
}

static void on_keyboard(unsigned char key,int x,int y){
  switch(key){
    //na ESC gasimo program
    case 27:
      exit(0);
      break;
    case 'g':
    case 'G':
      //pokrece se animacija
      if(!animation_ongoing){
        glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
        animation_ongoing=1;
      }
      break;
   case 'p':
   case 'P':
    //zaustavlja se animacija
    animation_ongoing = 0;
    break;
  case 'a':
  case 'A':
    curr_X -= v_x;
    if (curr_X <= -(1 - size / 2) ) {
      curr_X = 1 - size / 2;
    }
    break;
  case 'd':
  case 'D':
    curr_X += v_x;
    if (curr_X >= 1 - size / 2) {
       curr_X = -(1 - size / 2);
    }

  }

}

static void on_timer(int value){
  // proveravamo da li callback dolazi od odgovarajuceg tajmera
  if(value!=TIMER_ID)
    return;

  // azuriramo koordinate centra kvadrata
  curr_Y += v_y;
  if (curr_Y <= -(1 - size / 2)){
      v_y *= -1;
  }
  if (curr_Y >= ((1 - size / 2)/5)){
     v_y *= -1;
  }

  // Ponovo iscrtavanje prozora
  glutPostRedisplay();

  // Ponovo se postavlja tajmer
  if(animation_ongoing){
    glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
  }
}

static void on_reshape(int width, int height)
{
    /* Pamte se sirina i visina prozora. */
    window_width = width;
    window_height = height;
}

static void draw_player(){
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();
    glTranslatef(curr_X,curr_Y,0);
    glColor4f(1,0,0,0);
    glutSolidCube(1);
    glPopMatrix();
}


static void on_display(void){
  //Postavlja se boja piksela na zadatu boju pozadine
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Podesava se viewport. */
  glViewport(0, 0, window_width, window_height);

  /* Podesava se vidna tacka. */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 1, 100);

  //crtamo igraca
  draw_player();

  //Menja se slika na ekranu
  glutSwapBuffers();
}
