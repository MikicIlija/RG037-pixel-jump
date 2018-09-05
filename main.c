#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include<stdio.h>
#include<math.h>
#include<string.h>

#define RED 3
#define GREEN 1
#define BLUE 2
#define BLACK 0
#define TIMER_ID 0
#define TIMER_INTERVAL 20
#define MOVEMENT_SPEED 0.1 // brzina kratanja
/* Dimenzije prozora */
static int window_width, window_height;

static float player_x,player_y; // tekuce koordinate centra kvadrata
float self_rotate; // parametar za rotaciju igraca
float floor_y = -8; // trenutna y koordinata poda
static float v_x,v_y; //smer kretanja
static int animation_ongoing; //fleg koji pokazduje da li je animacia u toku
static void draw_player(void); //funkcija za crtanje igraca
static void draw_platform(int number); //funkcija za crtanje platformi
static void init_platforms(int number,char* path); //odradjuju se parametri platfomi
static void setup_lights(void); //funkcija koja postavlja svetla
static void display_text(const char *text,int length , int x , int y); // funkcija ispisuje tekst na prozoru
static int bounce_check(int plat_num); //fukncija koja proverava koliziju
static void progres(void); //funkcija simulira kretanje kroz mapu
int number_of_platforms; //trenutni broj platformi koje iscrtavamo
int score; //trenutni rezultat
int rotate_flag = 0; //flag za rotaciju
int double_points = 0; //flag za duple poene
int level_up = 100; //flag za level up
int level_number = 0; //trenutni level
char* path;

typedef struct{
  float plat_x;
  float plat_y;
  int type; //TIP PLATFORME RED GREEN BLUE BLACK
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
  glutKeyboardFunc(on_keyboard);
  glutReshapeFunc(on_reshape);
  glutDisplayFunc(on_display);


  //na pocetku postavljamo igraca na sredinu poda
  player_x = 0;
  player_y = floor_y;

  //Na pocetku se igrac ne rotira
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


  //proveravamo da li je zadato ime fajla
  if(argc < 2){
    printf("Morate zadati ime fajla po kome se generisu platforme!\n");
    exit(0);
  }

  //pamtimo ime fajla
  path = argv[1];


  //inicijalizujemo platforme
  init_platforms(number_of_platforms,path);


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
    //na slovo A se krecemo u levo
    player_x -= v_x;
    //ukoliko dodjemo do ivice pojavljujemo se na drugoj strani
    if (player_x <= -8) {
      player_x = 8;
    }
    break;
  case 'd':
  case 'D':
    //na slovo D se krecemo u desno
    player_x += v_x;
    //ukoliko dodjemo do ivice pojavljujemo se na drugoj strani
    if (player_x >= 8){
       player_x = -8;
    }
    break;
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
        double_points--;
      }else{
        score+=platforme[i].type+1;
      }
      if(platforme[i].type == BLUE){ //odskakanje od plave platforme pokrece/zaustavlja rotaciju
        if(rotate_flag == 1){
          rotate_flag = 0;
        }
        else{
          rotate_flag = 1;
        }
      }else if(platforme[i].type == GREEN){ //odskakanje od zelene platforme pali double poits mod za naredna 3 odskakanja
        double_points = 3;
      }
      else if(platforme[i].type == RED){ //odskakanje od crvene platforme je pomera po x osi
        platforme[i].plat_x = -5 + rand() % 10;
      }
    }
  }
  //kada dostignemo maximalnu visinu skoka krece padanje
  if (player_y >= floor_y+5){
    if(v_y >=0){
     v_y *= -1;
   }
  }

  if(player_y >= -6){ //pokrecemo kretanje kroz svet
      progres();
  }

  if(player_y < -10){ //ako igrac ispadne iz prozora zaustavlja se igra
    printf("%s\n","GAME OVER!");
    animation_ongoing = 0;
    glClearColor(0.75,0.75,0.75,0);
  }

  //kada dostignemo odredjeni rezultat podize se nivo igre
  if(score >= level_up){
    srand(time(NULL));
    //smanjuje se broj platformi
    number_of_platforms = number_of_platforms-1;
    init_platforms(number_of_platforms,path);
    level_up = level_up + 100;
    level_number ++;
    printf("%s\n","LEVEL UP!!" );
    if(level_number == 1){
      glClearColor(0.8,0.5,0.5,0);
    }
    else if(level_number == 2){
      glClearColor(0.5,0.5,0.8,0);
    }
    else{
      glClearColor(0.5,0.8,0.5,0);
    }
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
    //rotiramo igraca za dati parametar
    glRotatef(self_rotate,0,0,1);
    if(rotate_flag == 1){
      self_rotate += 1;
    }
    glScalef(1,1,1);
    glutSolidCube(1);
    glPopMatrix();
}

static void init_platforms(int number,char* path){
  //otvaramo falj stream
  FILE *f = fopen(path,"r");
  //proveravamo da li je fajl stream uspesno otvoren
  if(f == NULL){
    printf("%s\n", "Neuspelo otvaranje fajla!");
    exit(0);
  }
  float value;
  int i;
  //citamo vrednosti iz fajla i dodeljujemo ih platformama
  for(i=0;i<number_of_platforms;i++){
    fscanf(f,"%f",&value);
    platforme[i].plat_x = value;
    fscanf(f,"%f",&value);
    platforme[i].plat_y = value;
    fscanf(f,"%f",&value);
    platforme[i].type = value;
  }
  //zatvaramo fajl stream
  fclose(f);
}

static void draw_platform(int number){
  glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  int i;
  for(i=0;i<number;i++){
    glPushMatrix();
    if(platforme[i].type == RED){
      glColor4f(1,0.2,0.2,1);
    }
    else if(platforme[i].type == GREEN){
      glColor4f(0.2,1,0.2,1);
    }
    else if(platforme[i].type == BLUE){
      glColor4f(0.2,0.2,1,1);
    }
    else if(platforme[i].type == BLACK){
      glColor4f(0,0,0,1);
    }
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
      if(v_y <= 0 && plat_y<=9){
        floor_y = plat_y;
        v_y *= -1;
        return 1;
      }
    }
  }
  return 0;
}

static void progres(){ //simuliramo prolazak kroz mapu
  int i;
  srand(time(NULL));
  for(i=0;i<number_of_platforms;i++){
    if(player_y >= 5){
      platforme[i].plat_y -= 0.08 + level_number * 0.005;
    }else{
      platforme[i].plat_y -= 0.045 + level_number * 0.005;
    }
    if(platforme[i].plat_y <= -8){
      platforme[i].plat_x = -5 + rand()%10;
        platforme[i].plat_y += 5*number_of_platforms - (level_number*2-1);
      platforme[i].type = rand() % 4;
    }

  }
}

static void display_text(const char *text,int length , int x , int y) {
    if(player_y < -8){
        text = "GAME OVER \0";
        length = 10;
        x = 450/2;
        y = 600/2;
    }

    if((score >= 100 && score <= 105) || (score >= 200 && score <= 205) || (score >= 300 && score <= 305)){
      text = "LEVEL UP!!! SCORE: \0";
      length = 10;
      x = 450/2;
      y = 600/2;
    }
    if(score==0 && player_y >= -8){
        text = "SCORE: 0\0";
        length = 8;
    }


    glMatrixMode(GL_PROJECTION);
    double matrix[16];
    glGetDoublev(GL_PROJECTION_MATRIX,matrix);
    glLoadIdentity();
    glOrtho(0,600,0,450,-5,5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x,y);

    for ( int i = 0 ; i < length ; i++)
        glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,(int)text[i]);

    int num = floor(log10(abs(score))) + 1;
    char str[20];
    sprintf(str, "%d", score);

    if(player_y >= -8){
        for ( int i = 0 ; i < num ; i++)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,(int)str[i]);
    } else {
        char *final_score = "Final Score: ";
        glRasterPos2i(x,y-15);
        for ( int i = 0 ; i < strlen(final_score) ; i++)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,(int)final_score[i]);
        for ( int i = 0 ; i < num ; i++)
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18,(int)str[i]);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
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

  //crtamo tekst
  display_text("SCORE: \0", 10, 450, 420);

  //Menja se slika na ekranu
  glutSwapBuffers();
}
