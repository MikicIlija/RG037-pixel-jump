#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>

#define TIMER_ID 0
#define TIMER_INTERVAL 20
#define MOVEMENT_SPEED 0.03 // brzina kratanja
/* Dimenzije prozora */
static int window_width, window_height;

static float player_x,player_y; // tekuce koordinate centra kvadrata
float self_rotate;
static float v_x,v_y; //smer kretanja
static int animation_ongoing; //fleg koji pokazduje da li je animacia u toku
static void draw_player(void);
static void draw_platform(int number);
static void setup_lights(void);

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
  player_x = 0;
  player_y = -8;

  self_rotate=0;

  //inicijalizacija smera kretanja
  v_x = MOVEMENT_SPEED;
  v_y = MOVEMENT_SPEED;

  //na pocetku animacija stoji
  animation_ongoing = 0;

  //OpenGL inicijalizacija
  glClearColor(0.75,0.75,1,0);
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
    player_x -= v_x;
    if (player_x <= -8) {
      player_x = -8;
    }
    break;
  case 'd':
  case 'D':
    player_x += v_x;
    if (player_x >= 8){
       player_x = 8;
    }

  }

}

static void on_timer(int value){
  // proveravamo da li callback dolazi od odgovarajuceg tajmera
  if(value!=TIMER_ID)
    return;

  // azuriramo koordinate igraca
  player_y += v_y;
  if (player_y <= -8){
      v_y *= -1;
  }
  if (player_y >= -5){
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

static void draw_player(void){
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();
    glTranslatef(player_x,player_y,0);
    glRotatef(self_rotate,0,0,1);
    self_rotate += 1;
    glScalef(1,1,1);
    glutSolidCube(1);
    glPopMatrix();
}

static void draw_platform(int number){ //TODO: random da se generisu koordinate
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  int platform_x = -4;
  int platform_y = -6.7;
  int i;
  for(i=0;i<number;i++){
    glPushMatrix();
    platform_x += i;
    platform_y += i;
    glTranslatef(platform_x,platform_y,0);
    glScalef(4,0.2,1);
    glutSolidCube(1);
    glPopMatrix();
  }
}

static void setup_lights(void){
  /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
  GLfloat light_position[] = { 1, 1, 1, 0 };

  /* Ambijentalna boja svetla. */
  GLfloat light_ambient[] = { 0.5, 0.0, 0.0, 1 };

  /* Difuzna boja svetla. */
  GLfloat light_diffuse[] = { 1, 0.3, 0.3, 1 };

  /* Spekularna boja svetla. */
  GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

  /* Koeficijenti ambijentalne refleksije materijala. */
  GLfloat ambient_coeffs[] = { 1.0, 0.1, 0.1, 1 };

  /* Koeficijenti difuzne refleksije materijala. */
  GLfloat diffuse_coeffs[] = { 0.0, 0.0, 0.8, 1 };

  /* Koeficijenti spekularne refleksije materijala. */
  GLfloat specular_coeffs[] = { 1, 1, 1, 1 };

  /* Koeficijent glatkosti materijala. */
  GLfloat shininess = 20;

  // Ukljucujemo osvetljenje i podesavamo parametre
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  //Podesvamo parametre materijala
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}


//Postavlja se boja piksela na zadatu boju pozadine
static void on_display(void){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Podesava se viewport. */
  glViewport(0, 0, window_width, window_height);

  /* Podesava se vidna tacka. */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(15, 0, 0, 0, 0, 0, 0, 1, 0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 1, 100);


  //podesvamo svetla
  setup_lights();

  //postavljamo platforme
  draw_platform(5);

  //crtamo igraca
  draw_player();


  //Menja se slika na ekranu
  glutSwapBuffers();
}
