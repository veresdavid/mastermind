// This is my C server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 1024                      // buffer mérete
#define PORT_NO 6969                      // alapértelmezett portszám

// színek tömbje
// P - piros , K - kék, Z - zöld, L - lila, S - sárga, N - narancssárga
char colors[] = { 'P', 'K', 'Z', 'L', 'S', 'N' };

// függvény, ami összeszámolja, hogy a megadott tippben hány karakter van pontosan a helyén
int numberOfColorsInCorrectPlace(char guess[8], char sequence[5]){

  int cnt = 0;

  int i;
  for(i=0; i<4; i++){
    if(guess[i]==sequence[i]){
      cnt++;
    }
  }

  return cnt;

}

int numberOfColorsInSequence(char guess[8], char sequence[5]){

  int cnt = 0;

  int i, j;
  for(i=0; i<4; i++){
    for(j=0; j<4; j++){
      if(guess[i]==sequence[j]){
        cnt++;
        break;
      }
    }
  }

  return cnt;

}

int main(int argc, char* argv[]){

  // random
  srand(time(0));

  // Deklarációk
  int serverSocket;                 // szerver socket
  int playerSocket1;                // 1. játékos socket-je
  int playerSocket2;                // 2. játékos socket-je
  int flags;                      	// rcv flags
  struct sockaddr_in server;      	// socket name (addr) of server
  struct sockaddr_in client;	     	// socket name of client
  int server_size;                	// length of the socket addr. server
  int client_size;                	// length of the socket addr. client
  int bytes;		           					// length of buffer
  int rcvsize;                    	// received bytes
  int trnmsize;                   	// transmitted bytes
  int err;                        	// error code
  char on;                        	//
  char buffer[BUFSIZE+1];           // datagram dat buffer area

  // Inicializálás
  on                     = 1;
  flags                  = 0;
  bytes                  = BUFSIZE;
  server_size            = sizeof server;
  client_size            = sizeof client;
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  // server.sin_port        = htons(PORT_NO);

  // lehet egy parancssori argumentum, ami a portszámot adja meg
  if(argc==2){
    server.sin_port = htons( atoi( argv[1] ) );
  }else{
    server.sin_port = htons(PORT_NO);
  }

  // Szerver socket létrehozása
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(serverSocket<0){
    fprintf(stderr, "Hiba! Nem tudtuk letrehozni a szerver socketet!\n");
    exit(1);
  }

  // Szerver socket opcióinak beállítása
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
  setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);

  // Szerver socket bind
  err = bind(serverSocket, (struct sockaddr *) &server, server_size);
  if(err<0){
    fprintf(stderr, "Hiba! Nem tudtuk bindelni a szerver socketet!\n");
    exit(2);
  }

  // Szerver hallgató módba kapcsolása, várakozás a játékosokra
  err = listen(serverSocket, 2);
  if(err<0){
    fprintf(stderr, "Hiba! Nem tudtuk hallgató módba kapcsolni a szervert!\n");
    exit(3);
  }

  // Kapcsolatok elfogadása
  // Első játékos
  playerSocket1 = accept(serverSocket, (struct sockaddr *) &client, &client_size);
  if(playerSocket1<0){
    fprintf(stderr, "Hiba! Nem tudtuk elfogadni az 1. jatekos socketjet!\n");
    exit(4);
  }
  // Második játékos
  playerSocket2 = accept(serverSocket, (struct sockaddr *) &client, &client_size);
  if(playerSocket2<0){
    fprintf(stderr, "Hiba! Nem tudtuk elfogadni a 2. jatekos socketjet!\n");
    exit(5);
  }

  // Üzenet
  printf("A ket jatekos sikeresen felcsatlakozott!\n");

  // játékosok pontszámai
  int score1 = 0;
  int score2 = 0;

  // indul a játék
  while(1){

    // tudatjuk a játékosokat hogy kezdünk
    sprintf(buffer, "GAME_START\n");
    bytes = strlen(buffer);
    // üzenet küldése az 1. játékosnak
    trnmsize = send(playerSocket1, buffer, bytes, flags);
    if(trnmsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk ertesiteni az 1. jatekost a jatek kezdeterol!\n");
      exit(6);
    }
    // üzenet küldése az 2. játékosnak
    trnmsize = send(playerSocket2, buffer, bytes, flags);
    if(trnmsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk ertesiteni a 2. jatekost a jatek kezdeterol!\n");
      exit(7);
    }

    // feladvány generálása
    char sequence[5];
    int i;
    for(i=0; i<4; i++){
      sequence[i] = colors[rand()%6];
    }
    sequence[4] = '\0';

    printf("A generalt feladvany: %s\n", sequence);

    // a játékosok tippjei
    char guess1[8];
    char guess2[8];

    // olvassuk a játékosok tippjeit, és választ küldünk nekik
    while(1){

      // felszólítjuk a játékosokat hogy küldjék el a tippjüket, és azt beolvassuk
      sprintf(buffer, "ENTER_GUESS\n");
      bytes = strlen(buffer);
      // 1. játékos
      trnmsize = send(playerSocket1, buffer, bytes, flags);
      if(trnmsize<0){
        fprintf(stderr, "Hiba! Nem tudtuk ertesiteni az 1. jatekost, hogy kuldje el a tippjet!\n");
        exit(8);
      }
      // 1. játékos tippjének fogadása
      rcvsize = recv(playerSocket1, guess1, 8, flags);
      if(rcvsize<0){
        fprintf(stderr, "Hiba! Nem tudtuk fogadni az 1. jatekos tippjet!\n");
        exit(9);
      }
      guess1[rcvsize-1] = '\0';
      // 2. játékos
      trnmsize = send(playerSocket2, buffer, bytes, flags);
      if(trnmsize<0){
        fprintf(stderr, "Hiba! Nem tudtuk ertesiteni a 2. jatekost, hogy kuldje el a tippjet!\n");
        exit(10);
      }
      // 2. játékos tippjének fogadása
      rcvsize = recv(playerSocket2, guess2, 8, flags);
      if(rcvsize<0){
        fprintf(stderr, "Hiba! Nem tudtuk fogadni a 2. jatekos tippjet!\n");
        exit(11);
      }
      guess2[rcvsize-1] = '\0';

      printf("Tipp 1: %s\tTipp 2: %s\n", guess1, guess2);

      // üzenünk a játékosoknak, hogy a kör adatait fogadni tudják
      sprintf(buffer, "ROUND_STATS\n");
      bytes = strlen(buffer);
      // 1. játékos
      trnmsize = send(playerSocket1, buffer, bytes, flags);
      if(trnmsize<0){
        fprintf(stderr, "Hiba! Nem tudtuk ertesiteni az 1. jatekost, hogy fogadja a kor adatait!\n");
        exit(18);
      }
      // 2. játékos
      trnmsize = send(playerSocket2, buffer, bytes, flags);
      if(trnmsize<0){
        fprintf(stderr, "Hiba! Nem tudtuk ertesiteni a 2. jatekost, hogy fogadja a kor adatait!\n");
        exit(19);
      }

      // kör statisztikájának előállítása, és kiküldése a játékosoknak
      sprintf(buffer, " -- KOR ADATAI --\n -- 1. jatekos: tipp: %s , jo szinek: %d , szereplo szinek: %d --\n -- 2. jatekos: tipp: %s , jo szinek: %d , szereplo szinek: %d --\n", guess1, numberOfColorsInCorrectPlace(guess1, sequence), numberOfColorsInSequence(guess1, sequence), guess2, numberOfColorsInCorrectPlace(guess2, sequence), numberOfColorsInSequence(guess2, sequence));
      bytes = strlen(buffer);
      // 1. játékos
      trnmsize = send(playerSocket1, buffer, bytes, flags);
      if(trnmsize<0){
        fprintf(stderr, "Hiba! Nem tudtuk elkuldeni a kor adatait az 1. jatekosnak!\n");
        exit(20);
      }
      // 2. játékos
      trnmsize = send(playerSocket2, buffer, bytes, flags);
      if(trnmsize<0){
        fprintf(stderr, "Hiba! Nem tudtuk elkuldeni a kor adatait a 2. jatekosnak!\n");
        exit(21);
      }

      // ha valaki eltalálta a generált feladványt, vagy feladta, akkor vége az aktuális játékmenetnek
      if(strcmp(guess1, sequence)==0 || strcmp(guess2, sequence)==0 || strcmp(guess1, "feladom")==0 || strcmp(guess2, "feladom")==0){
        break;
      }

    }

    // bejelentjük, hogy vége az aktuális játékmenetnek, és elküldjük a játékosoknak
    sprintf(buffer, "MATCH_END\n");
    bytes = strlen(buffer);
    // 1. játékos
    trnmsize = send(playerSocket1, buffer, bytes, flags);
    if(trnmsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk megmondani az 1. jatekosnak, hogy vege az aktualis jatekmenetnek!\n");
      exit(22);
    }
    // 2. játékos
    trnmsize = send(playerSocket2, buffer, bytes, flags);
    if(trnmsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk megmondani az 1. jatekosnak, hogy vege az aktualis jatekmenetnek!\n");
      exit(23);
    }

    // eredmény előállítása
    sprintf(buffer, " ++ JATEKMENET VEGE ++\n ++ A helyes megoldas: %s ++\n", sequence);

    // mindkét játékos feladta
    if(strcmp(guess1, "feladom")==0 && strcmp(guess2, "feladom")==0){
      strcat(buffer, " ++ Dontetlen! Mindket jatekos feladta. ++\n");
    }
    // 1. játékos feladta
    else if(strcmp(guess1, "feladom")==0){
      strcat(buffer, " ++ A 2. jatekos nyert! Az 1. jatekos feladta. ++\n");
      score2++;
    }
    // 2. játékos feladta
    else if(strcmp(guess2, "feladom")==0){
      strcat(buffer, " ++ Az 1. jatekos nyert! A 2. jatekos feladta. ++\n");
      score1++;
    }
    // döntetlen
    else if(strcmp(guess1, guess2)==0){
      strcat(buffer, " ++ Dontetlen! Mindket jatekos kitalalta. ++\n");
      score1++;
      score2++;
    }
    // 1. játékos nyert
    else if(strcmp(guess1, sequence)==0){
      strcat(buffer, " ++ Az 1. jatekos nyert! ++\n");
      score1++;
    }
    // 2. játékos nyert
    else if(strcmp(guess2, sequence)==0){
      strcat(buffer, " ++ A 2. jatekos nyert! ++\n");
      score2++;
    }

    bytes = strlen(buffer);

    // eredmény kiküldése a játékosoknak
    // 1. játékos
    trnmsize = send(playerSocket1, buffer, bytes, flags);
    if(trnmsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk elkuldeni az eredmenyt az 1. jatekosnak!\n");
      exit(24);
    }
    // 2. játékos
    trnmsize = send(playerSocket2, buffer, bytes, flags);
    if(trnmsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk elkuldeni az eredmenyt a 2. jatekosnak!\n");
      exit(25);
    }

    // szavazás visszavágóról
    // a két játékos döntése
    char choice1[5];
    char choice2[5];

    // megkérdezzük a két játékost, hogy akarnak-e visszavágót
    sprintf(buffer, "PLAY_AGAIN\n");
    bytes = strlen(buffer);
    // megkérdezzük az 1. játékost
    trnmsize = send(playerSocket1, buffer, bytes, flags);
    if(trnmsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk megkerdezni az 1. jatekost, hogy akar-e visszavagot!\n");
      exit(12);
    }
    rcvsize = recv(playerSocket1, choice1, 5, flags);
    if(rcvsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk fogadni, hogy az 1. jatekos akar-e visszavagot!\n");
      exit(13);
    }
    choice1[rcvsize-1] = '\0';
    // megkérdezzük a 2. játékost
    trnmsize = send(playerSocket2, buffer, bytes, flags);
    if(trnmsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk megkerdezni a 2. jatekost, hogy akar-e visszavagot!\n");
      exit(14);
    }
    rcvsize = recv(playerSocket2, choice2, 5, flags);
    if(rcvsize<0){
      fprintf(stderr, "Hiba! Nem tudtuk fogadni, hogy a 2. jatekos akar-e visszavagot!\n");
      exit(15);
    }
    choice2[rcvsize-1] = '\0';

    // ha mind a ketten igennel szavaznak, akkor jöhet a visszavágó
    if(!(strcmp(choice1, "igen")==0 && strcmp(choice2, "igen")==0)){
      break;
    }

  }

  // tudatjuk a játékosokkal, hogy vége a játéknak
  sprintf(buffer, "GAME_OVER\n");
  bytes = strlen(buffer);
  // 1. játékos
  trnmsize = send(playerSocket1, buffer, bytes, flags);
  if(trnmsize<0){
    fprintf(stderr, "Hiba! Nem tudtuk megmondani az 1. jatekosnak, hogy vege a jateknak!\n");
    exit(16);
  }
  // 2. játékos
  trnmsize = send(playerSocket2, buffer, bytes, flags);
  if(trnmsize<0){
    fprintf(stderr, "Hiba! Nem tudtuk megmondani a 2. jatekosnak, hogy vege a jateknak!\n");
    exit(17);
  }

  // végeredmény elküldése
  sprintf(buffer, " :: VEGEREDMENY ::\n :: 1. jatekos: %d pont ::\n :: 2. jatekos: %d pont ::\n", score1, score2);

  // döntetlen
  if(score1==score2){
    strcat(buffer, " :: DONTETLEN! ::\n");
  }
  // 1. játékos nyert
  else if(score1>score2){
    strcat(buffer, " :: AZ 1. JATEKOS NYERT! ::\n");
  }
  // 2. játékos nyert
  else if(score2>score1){
    strcat(buffer, " :: A 2. JATEKOS NYERT! ::\n");
  }

  bytes = strlen(buffer);

  // 1. játékos
  trnmsize = send(playerSocket1, buffer, bytes, flags);
  if(trnmsize<0){
    fprintf(stderr, "Hiba! Nem tudtuk elkuldeni a vegeredmenyt az 1. jatekosnak!\n");
    exit(26);
  }
  // 2. játékos
  trnmsize = send(playerSocket2, buffer, bytes, flags);
  if(trnmsize<0){
    fprintf(stderr, "Hiba! Nem tudtuk elkuldeni a vegeredmenyt a 2. jatekosnak!\n");
    exit(27);
  }

  // kapcsolatok lezárása
  close(playerSocket2);
  close(playerSocket1);
  close(serverSocket);

  exit(0);
}
