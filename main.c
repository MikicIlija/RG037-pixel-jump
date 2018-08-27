#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include<stdio.h>

#define RED 0
#define GREEN 1
#define BLUE 2
#define TIMER_ID 0
#define TIMER_INTERVAL 20
#define MOVEMENT_SPEED 0.1 // brzina kratanja
/* Dimenzije prozora */
static int window_width, window_height;

static float player_x,player_y; // tekuce koordinate centra kvadrata
float self_rotate;
float floor_y = -8;
static float v_x,v_y; //smer kretanja
static int animation_ongoing; //fleg koji pokazduje da li je animacia u toku
static void draw_player(void);
static void draw_platform(int number);
static void init_platforms(int number);
static void setup_lights(void);
static int bounce_check(int plat_num);
int number_of_platforms;
static void progres(void);
int score;
int rotate_flag = 0;
int double_points = 0;
int double_points_counter;
int level_up = 50;

typedef struct{
  float plat_x;
  float plat_y;
  int type; //GREEN BLUE RED
}Platform;


//inicijalizacija platformi
Platform platforme[5];

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
      // diffuse_coeffs[0] = 0.1;
      // diffuse_coeffs[1] = 0.1;
      // diffuse_coeffs[2] = 1;
      // diffuse_coeffs[3] = 1;;
  glutKeyboardFunc(on_keyboard);
  glutReshapeFunc(on_reshape);
  glutDisplayFunc(on_display);


  //na pocetku postavljamo igraca na sredinu poda
  player_x = 0;
  player_y = floor_y;

  self_rotate=0;

  //inicijalizacija smera kretanja
  v_x = MOVEMENT_SPEED;
  v_y = MOVEMENT_SPEED;

  //na pocetku animacija stoji
  animation_ongoing = 0;

  //OpenGL inicijalizacija
  glClearColor(0.75,0.75,1,0);
  glEnable(GL_DEPTH_TEST);

  number_of_platforms = 5; //pocetni broj platformi
  score = 0; //inicijalizujemo rezultat

  //inicijalizujemo platforme
  init_platforms(number_of_platforms);


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
      player_x = 8;
    }
    break;
  case 'd':
  case 'D':
    player_x += v_x;
    if (player_x >= 8){
       player_x = -8;
    }

  }

}

static void on_timer(int value){
  // proveravamo da li callback dolazi od odgovarajuceg tajmera
  if(value!=TIMER_ID)
    return;

  // azuriramo koordinate igraca
  player_y += v_y;
  int i;

  for(i=0;i<number_of_platforms;i++){
    if(bounce_check(i) == 1){
      //racunamo trenutni rezultat u ondosu na mod u kome smo
      if(double_points){
        score+=(platforme[i].type+1)*2;
        printf("SCORE:%d\n",score);
        double_points--;
      }else{
        score+=platforme[i].type+1;
        printf("SCORE:%d\n",score);
      }
      if(platforme[i].type == BLUE){ //odskakanje od plave platfome pokrece/zaustavlja rotaciju
        if(rotate_flag == 1){
          rotate_flag = 0;
        }
        else{
          rotate_flag = 1;
        }
      }else if(platforme[i].type == GREEN){ //odbijanje od zelene platforme pali double poits mod za naredne 5 odskakanja
        double_points = 5;
      }
    }
  }
  if (player_y >= floor_y+5){
    if(v_y >=0){
     v_y *= -1;
     // printf("%f pozi\n",player_y);
     // printf("%s\n","DOLE!");
   }
  }

  if(player_y >= -6 && v_y >= 0){ //pokrecemo kretanje kroz svet
      progres();
  }

  if(player_y < -10){ //ako igrac ispadne iz prozora zaustavlja se igra
    printf("%s\n","GAME OVER!");
    animation_ongoing = 0;
  }

  if(score >= level_up){
    srand(time(NULL));
    number_of_platforms = number_of_platforms-1;
    init_platforms(number_of_platforms);
    level_up = level_up + 50;
    printf("%s\n","LEVEL UP!!" );
      glClearColor(0.8,0.5,0.5,0);
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
    //odredjuje se boja igraca u ondosu ma mod u kome se nalazi
    if(double_points){
      glColor4f(0.2,1,0.2,1);
    }else{
      glColor4f(1,1,0,1);
    }
    //pomeramo igraca na svoju poziciju
    glTranslatef(player_x,player_y,0);
    //
    glRotatef(self_rotate,0,0,1);
    if(rotate_flag == 1){
      self_rotate += 1;
    }
    glScalef(1,1,1);
    glutSolidCube(1);
    glPopMatrix();
}

static void init_platforms(int number){
  int i;
  srand(time(NULL));
  //inicijalizujemo prvu platformu
  platforme[0].plat_x = -5 + rand() % 10;
  platforme[0].plat_y = -5;
  platforme[0].type = rand() % 3;
  int r;
  //nasumicno dodeljujemo mod platfomama
  for(i=1;i<number;i++){
    r = rand()%100;
    if(r <= 50){
      platforme[i].type = RED;
    } else if(r <= 70){
      platforme[i].type = GREEN;
    }
    else{
      platforme[i].type = BLUE;
    }
    //nasumicno odredjujemo pozicije platfomi
    platforme[i].plat_x = -6 + rand() % 12 ;
    platforme[i].plat_y =  platforme[i-1].plat_y + number;
  }
}

static void draw_platform(int number){
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  int i;
  for(i=0;i<number;i++){
    glPushMatrix();
    GLfloat diffuse_coeffs[4];
    if(platforme[i].type == RED){
      // diffuse_coeffs[0] = 1;
      // diffuse_coeffs[1] = 0.1;
      // diffuse_coeffs[2] = 0.1;
      // diffuse_coeffs[3] = 1;
      glColor4f(1,0.2,0.2,1);
    }
    else if(platforme[i].type == GREEN){
      // diffuse_coeffs[0] = 0.1;
      // diffuse_coeffs[1] = 1;
      // diffuse_coeffs[2] = 0.1;
      // diffuse_coeffs[3] = 1;
      glColor4f(0.2,1,0.2,1);
    }
    else if(platforme[i].type == BLUE){
      // diffuse_coeffs[0] = 0.1;
      // diffuse_coeffs[1] = 0.1;
      // diffuse_coeffs[2] = 1;
      // diffuse_coeffs[3] = 1;
      glColor4f(0.2,0.2,1,1);
    }
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glTranslatef(platforme[i].plat_x,platforme[i].plat_y,0);
    glScalef(4,0.2,1);
    glutSolidCube(1);
    glPopMatrix();
  }
}

int bounce_check(int plat_num){
  float plat_x = platforme[plat_num].plat_x;
  float plat_y = platforme[plat_num].plat_y;
  if(player_y <= plat_y+0.7 && player_y >= plat_y-0.7){
    if(player_x >= plat_x - 2 && player_x <= plat_x+2){
      if(v_y <= 0 && plat_y<=9){ //FIX
        floor_y = plat_y;
        // printf("%s\n","GORE!" );
        // printf("%f\n",floor_y);
        v_y *= -1;
        return 1;
      }
    }
  }
  return 0;
}

static void progres(){
  int i;
  srand(time(NULL));
  for(i=0;i<number_of_platforms;i++){
    platforme[i].plat_y -= 0.1;
    if(platforme[i].plat_y <= -8){
      platforme[i].plat_x = -5 + rand()%10;
      platforme[i].plat_y += 5*number_of_platforms;
      platforme[i].type = rand() % 3;
    }
  }
}

static void setup_lights(void){
  /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
  GLfloat light_position[] = { 1, 1, 1, 0 };

  /* Ambijentalna boja svetla. */
  GLfloat light_ambient[] = { 0.5, 0.0, 0.0, 1 };

  /* Difuzna boja svetla. */
  GLfloat light_diffuse[] = { 0.3, 0.3, 0.3, 1 };

  /* Spekularna boja svetla. */
  GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

  /* Koeficijenti ambijentalne refleksije materijala. */
  GLfloat ambient_coeffs[] = { 0.1, 0.1, 0.1, 1 };

  /* Koeficijenti spekularne refleksije materijala. */
  GLfloat specular_coeffs[] = { .6, .6, .6, 1 };

  /* Koeficijent glatkosti materijala. */
  GLfloat shininess = 80;

  // Ukljucujemo osvetljenje i podesavamo parametre
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  //Podesvamo parametre materijala
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
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
  draw_platform(number_of_platforms);

  //crtamo igraca
  draw_player();

  //Menja se slika na ekranu
  glutSwapBuffers();
}
