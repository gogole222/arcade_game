// Accéléromètre CMA3000
// (Ne fonctionne que sur la première version des cartes MSP4-EXP430F5529 )
// EPFL 2020

#include <msp430.h>
#include <stdint.h>

#include <HAL_Cma3000.h>
#include <HAL_Dogs102x6.h>

#include <PYR_CarteBlanche.h>
#include <PYR_5PWM.h>

//initialisation des variables
int16_t offsetX;
uint8_t pierrex = 61;
uint8_t pierrey;
uint8_t vaisseau;
int16_t accX;
uint8_t missile;
uint8_t missx;
uint8_t missy;
int8_t touche;
int8_t menu;
int8_t vie;
int8_t saut;
int8_t valroue;
int8_t diff;
int8_t cpt;
uint16_t cpt2;
int8_t cpt3;
int8_t cpt4;
uint16_t countdown;
int8_t r4;
int8_t r2;
int8_t r3;
uint8_t score;
uint8_t score2;
uint8_t score3;
uint8_t score4;
uint8_t pous=0;
uint8_t pous2=0;
uint8_t jeu;
uint8_t jeutype;
uint8_t jump;
uint8_t att=1;
uint8_t ie;
uint8_t L1;
uint8_t L2;
uint8_t L3;
uint8_t L4;
uint8_t speednom;
uint8_t speednum;
uint8_t duck;
uint8_t r1;
uint8_t down;
char* s1="";
char* s2="";
char* s3="";
char* s4="";
char* digit[] = {"0","1","2","3","4","5","6","7","8","9"}; //liste des chiffres pour l'�criture
uint8_t te;

void Cma3000_calibre() {
  uint16_t i;
  offsetX = 0;
  for (i=0; i<100; i++){ // Attente pour stabiliser le capteur (?)
  }
  for (i=0; i<16; i++){ // moyenne sur 16 valeurs
    Cma3000_readAccel();
    offsetX += Cma3000_xAccel;
  }
  offsetX = offsetX/16;
}

void main(void) {
  WDTCTL = WDTPW | WDTHOLD; //watchdog disable
  setupDCO(); // Fréquence à 25 MHz
  InitCarteBlanche(); //initialisation de la carte blanche
  Wheel_init(); //initialisation du potentiom�tre

  P1IES &=~ (1<<7); //Mode d'interruption : flanc montant
  P1IE |= (1<<7); // Interruption P1 activ�e sur le poussoir
  P1IFG &=~(1<<7); //Fanion d'interruption � z�ro

  P2IES|= (1<<2); //Mode d'interruption : flanc descendant
  P2IE |= (1<<2); // Interruption P1 activ�e sur le poussoir
  P2IFG &=~(1<<2); //Fanion d'interruption � z�ro

  __enable_interrupt(); //Active l'ensemble des interruptions

  Cma3000_init(); // Initialisation acc�l�rom�tre

  Dogs102x6_init(); Dogs102x6_backlightInit(); // Initialisation LCD
  Dogs102x6_setBacklight(11); Dogs102x6_setContrast(11); //param�tres d'affichage

  Cma3000_calibre(); //calibration de l'acc�l�rom�tre
  Dogs102x6_clearScreen(); //efface tout sur l'�cran

  while (1) {

      //menu �crit dans des boucles switch - premi�re boucle switch comprend le menu,le niveau de difficult� de spaceship
      //le r�glage du contraste et le jeu en lui-m�me
      switch (menu) {
      case 0 ://page d'accueil- choix du jeu
          valroue = Wheel_getValue()/1400; //lecture de la valeure du potentiom�tre (trois positions si divis� par 1400)
          char* spaceship = "Spaceship"; //initialisation des char pour l'�cran
          char* nointernet = "No Internet";
          Dogs102x6_stringDrawXY(25, 40,spaceship, 0); //�criture des noms des jeux sur l'�cran
          Dogs102x6_stringDrawXY(18, 15,nointernet, 0);
          //�criture du symbol du contraste sur l'�cran pour menu contraste
          Dogs102x6_circleDraw(95,5,2,0);
          Dogs102x6_pixelDraw(95, 2,0);
          Dogs102x6_pixelDraw(95, 8,0);
          Dogs102x6_pixelDraw(98, 3,0);
          Dogs102x6_pixelDraw(92, 3,0);
          Dogs102x6_pixelDraw(98, 7,0);
          Dogs102x6_pixelDraw(92, 7,0);

          //choix entre les jeux et le contraste
          switch (valroue){
          case 0 :   //spaceship
              Dogs102x6_circleDraw(13,19,2,1); //efface ancien curseur
              Dogs102x6_circleDraw(87,5,2,1); //efface ancien curseur
              Dogs102x6_circleDraw(20,44,2,0); //�crit la position du curseur sur LCD
              jeutype=1; //spaceship
              if (pous) {menu=1; pous=0; pous2=0; Dogs102x6_clearScreen();} // renvoi au choix de la difficult� pour spaceship
              break;

          case 1 : //no internet
              Dogs102x6_circleDraw(20,44,2,1); //efface ancien curseur
              Dogs102x6_circleDraw(87,5,2,1);  //efface ancien curseur
              Dogs102x6_circleDraw(13,19,2,0); //�criture position du curseur
              jeutype=0; //no internet
              if (pous) {menu=2; pous=0; Dogs102x6_clearScreen();} //renvoi au menu jeu (pas de choix de difficult�)
              break;

          case 2 : //contraste
              Dogs102x6_circleDraw(20,44,2,1); //efface ancien curseur
              Dogs102x6_circleDraw(13,19,2,1); //efface ancien curseur
              Dogs102x6_circleDraw(87,5,2,0); //�criture position du curseur
              if (pous) {menu=3; pous=0; Dogs102x6_clearScreen();} //renvoi au menu contraste
              break;
          }
          break;

      case 1 : //choix du niveau de difficult� pour spaceship
          valroue = Wheel_getValue()/2100;//valeur du potentiom�tre (deux valeurs)
          char* easy = "easy"; //variable char des choix
          char* hard = "hard";
          Dogs102x6_stringDrawXY(40, 35,easy, 0); //�criture du niveau facile sur LCD
          Dogs102x6_stringDrawXY(40, 10,hard, 0); //�criture du niveau difficle sur LCD

          switch (valroue){
          case 0 : //niveau facile
              Dogs102x6_circleDraw(35,14,2,1); //efface ancien curseur
              Dogs102x6_circleDraw(87,5,2,1); //efface ancien curseur
              Dogs102x6_circleDraw(35,39,2,0); //�crit la position du curseur sur LCD
              diff=0; //variable de diffcult�=0 donc facile
              if (pous) {menu=2; pous=0; Dogs102x6_clearScreen();} //renvoi au menu jeu
              if (pous2) {menu=0; pous2=0; Dogs102x6_clearScreen();} //renvoi au menu du choix du jeu
              break;

          case 1 : //niveau difficile
              Dogs102x6_circleDraw(35,39,2,1); //efface ancien curseur
              Dogs102x6_circleDraw(87,5,2,1); //efface ancien curseur
              Dogs102x6_circleDraw(35,14,2,0); //�criture position du curseur
              diff=1;//niveau difficile=1
              if (pous) {menu=2; pous=0; Dogs102x6_clearScreen();} //renvoi au menu jeu
              if (pous2) {menu=0; pous2=0; Dogs102x6_clearScreen();} //renvoi au menu du choix du jeu
              break;
          }
          break;

      case 2 : //menu du jeu
             pous=1; //bouton poussoir
             score=0; //score remit � z�ro - troisi�me decimale
             score2=0; //seconde d�cimale
             score3=0; //premi�re d�cimale
             L1=L2=L3=L4=0; //variables pour la d�tection du score pour no internet
             te=0; //variable pour l'attente avant les premiers obstacle dans no internet
             jeu=1; //variable pour la boucle while du jeu
             saut=20; //variable pour controller la hauteur du saut dans no internet
             cpt2=0; //compteur
             cpt=-125; //compteur
             cpt3=-125; //compteur
             att=0;
             P1IES |= (1<<7);
             countdown=0; //compteur pour l'attente avant les premiers obstacle dans no internet
             speednom=1; //variable pour la vitesse du jeu dans no internet
             speednum=1; //variable pour la vitesse du jeu dans no internet
             down=0;
             jump=1;
             Dogs102x6_clearScreen(); //efface tout sur l'�cran
             TA2CTL=TASSEL_2 | ID_3 | MC_1 | TAIE; //initialisation du timer avec diviseur par 8, compteur en Up mode et interuption � TA1CCR0
             TA2CCR0 = 65000; //permet de varier la vitesse du jeu
             Led4On;Led5On;Led6On;Led7On;Led8On; //leds bleues allum�es pour le nombre de vie dans spaceship

             while (jeu) {} //boucle du jeu - reste dedans tant que le joueur n'a pas perdu

             TA2CTL=TAIE; //timer disable
             Dogs102x6_clearInverseDisplay(); //remet le display en mode noir sur blanc au cas o� le joueur perd lors d'un changement d'�clairage
             Dogs102x6_clearScreen();
             att=1;
             P1IES &=~ (1<<7);
             menu=0; //retour au menu choix du jeu
             //affichage du score dans le menu
             char* sc = "score:";
             Dogs102x6_stringDrawXY(2,55, sc, 0); //�criture score
             Dogs102x6_stringDrawXY(46, 55,s2, 0);//�criture seconde d�cimale
             Dogs102x6_stringDrawXY(53, 55,s1, 0); //�criture troisi�me d�cimla
             Dogs102x6_stringDrawXY(39, 55,s3, 0); //�criture premi�re d�cimale
             pous=0;
             break;

      case 3 : //menu du contraste
          valroue = 28 - Wheel_getValue()/150; //valeur du potentiom�tre de 1 � 28
          char* contraste = "contraste:"; //variable char
          Dogs102x6_stringDrawXY(25, 10,contraste, 0); //�criture de "contraste"
          Dogs102x6_setContrast(valroue); //changement du contraste en fonction du potentiom�tre
          char* contrlevel = "";
          char* contrlevel2 = "";
          if (valroue<10) {contrlevel = digit[valroue]; contrlevel2 = digit[0];} // �criture sur'�cran du contraste de 0 � 9
          if (valroue>9) {contrlevel=digit[valroue-10]; contrlevel2 = digit[1];} // �criture pour 10 � 19
          if (valroue>19) {contrlevel=digit[valroue-20]; contrlevel2 = digit[2];} // �criture pour 20 � 29
          Dogs102x6_stringDrawXY(53, 30,contrlevel, 1); //�criture du premier chiffre
          Dogs102x6_stringDrawXY(46, 30,contrlevel2, 1); //�criture du second chiffre
          if (pous) {menu=0;pous=0; Dogs102x6_clearScreen();} //renvoi au menu du choix de jeu

           break;
      }
  }
}

//interruption pour personnage qui se baisse dans no internet et poussoir dans menu
#pragma vector=PORT1_VECTOR // syntaxe sp�cifique pour les interruptions
__interrupt void Port_1(void) {

    if (P1IES & (1<<7) && duck==0){duck=1;}//personnage qui se baisse pour front descendant et remonte pour front montant
    else {duck=0;}

    if (pous==0){pous=1;}//pousoir pour le menu au front descendant

    P1IFG &=~ (1<<7); //Fanion d'interruption remit � z�ro
    if (att==0) {P1IES ^= (1<<7);} //toggle pour front montant et descendant
}

//interruption pour lancement de missile dans spaceship, bouton poussoir
//de retour en arri�re dans menu et saut dans no internet
#pragma vector=PORT2_VECTOR // syntaxe sp�cifique pour les interruptions
__interrupt void Port_2(void) {

  if (missile==0){ //v�rifie qu'aucun missile n'est d�j� lanc�
       missile=1; //enclanchement du missile
       missx=vaisseau; //position du vaisseau lors du tire
       missy=0;
   }

  if (pous2==0){pous2=1;}//pousoir de retour en arri�re pour le menu

  if (jump==0 && duck==0) {jump=1;cpt=0;}//saut dans no internet si le personnage n'est pas baiss�

  P2IFG &=~ (1<<2); //Fanion d'interruption remit � z�ro
}

//interuption li� au timer pour l'affichage des jeux
// Timer_A1 Interrupt Vector (TAIV) handler
#pragma vector=TIMER2_A1_VECTOR //interruption du timer � chaque overflow (cadence les images de la vid�o du jeu)
__interrupt void Timer_A2(void) {
  switch(TA2IV) {
  case  2:  // CCR1 : not used
           break;
  case  4: // CCR2 : not used
           break;
  case  6: // CCR3 : not used
             break;
  case  8: // CCR4 : not used
             break;
  case 14: // Overflow (cette valeur �tait de 10 pour le MSP430G)

      //affichage du score
      if(score==10){score2++;score=0;} //incr�mentation de la premi�re d�cimal du score (centaines)
      if(score2==10){score3++;score2=0;score=0;} //incr�mentation de la seconde d�cimal du score (dizaines)
      if(score3==10){score4++;score3=0;score2=0;score=0;}
      s1 = digit[score]; //actualisation de la troisi�me d�cimale
      s2 = digit[score2]; //actualisation de la seconde d�cimale
      s3 = digit[score3]; //actualisation de la premi�re d�cimale
      s4 = digit[score4]; //actualisation de la premi�re d�cimale
      Dogs102x6_clearRow(1); //efface anciennes valeurs du score
      Dogs102x6_stringDrawXY(80, 8,s3, 0); //�criture sur LCD de la premi�re d�cimale
      Dogs102x6_stringDrawXY(87, 8,s2, 0); //�criture sur LCD de la seconde d�cimale
      Dogs102x6_stringDrawXY(94, 8,s1, 0); //�criture sur LCD de la troisi�me d�cimale
      Dogs102x6_stringDrawXY(73, 8,s4, 0); //�criture sur LCD de la troisi�me d�cimale

    if (jeutype){//selction du jeu --> jeutype == 1 alors spaceship

      //�criture du jeu spaceship

          //cr�ation des pierres
          Dogs102x6_circleDraw(pierrex,pierrey,3,1); //efface l'ancienne position de la pierre sur le LCD
          if (pierrey == 65 || touche == 1) { //si la pierre est touch�e ou atteint le sol
              pierrex = rand() % 62 + 20; //cr�ation d'un numbre al�atoire entre 20 et 82 pour la position x de la pierrre
              pierrey = 0; //position y de la pierre remise � z�ro
              touche=0; //variable de si pierre touch�e ou non remise � z�ro
          }

          //level en fonction du choix dans le menu
          if (diff==0 && cpt==2){pierrey++; cpt=0;} //niveau facile la pierre avance tout les deux overflow
          if (diff==1 && cpt==1){pierrey++; cpt=0;} //niveau difficile la pierre avance � chaque overflow
          cpt++; //incr�mentation du comptage pour le niveau

          Dogs102x6_circleDraw(pierrex,pierrey,3,0); //�criture de la nouvelle position de la pierre sur LCD

          //affichage du vaisseau
          Cma3000_readAccel();//lecture de l'acc�l�rom�tre
          if (cpt2 == 6) {accX = Cma3000_xAccel-offsetX; cpt2=0;} //actualisation de la position du vaisseau tout les 6 overflow
          cpt2++; //incr�mentation du comptage pour position vaisseau
          Dogs102x6_horizontalLineDraw(vaisseau-2, vaisseau+2, 60, 1);//efface ancien vaisseau sur LCD
          vaisseau=61-2*accX; //position x du vaisseau sur l'�cran actualis�e � partir de l'acutalisation de la position
          Dogs102x6_horizontalLineDraw(vaisseau-2, vaisseau+2, 60, 0);//�criture du vaisseau sur LCD

          //lancement des missiles
          Dogs102x6_circleDraw(missx, 60-missy,2, 1);  //effa�age de l'ancien missile
          if (missile==1) {
              missy=missy+2;//incr�mentation de la position y du missile
              Dogs102x6_circleDraw(missx, 60-missy,2, 0); //�criture du missile sur �cran LCD si missile lanc�
          }
          if (missy==60) { //si le missile � ateind le haut de l'�cran
                     missile=0;  //missile disable
                     Dogs102x6_circleDraw(missx, 60-missy,2, 1); //effa�age de la derni�re position du missile
                     missy=0; //position y du missile remit � z�ro
          }

          //d�tection de collision entre missile et pierre
           if(missy!=0){ //si missile est sur l'�cran
               if (missx<(pierrex+7) && (pierrex-7)<missx && (pierrey-5)>(60-missy)) {//d�tection d'une collision entre le missile et la pierre
                   touche=1; //touch�
                   score++; //inc�mentation du score
               }
           }

           //d�tection d'une pierre qui touche le sol
           if (pierrey == 65) {vie++;} //si la pierre touche le sol vie perdue
           if (vie==5){
               jeu=0; //si cinq vie perdues retour au menu
               vie=0; //reinisitalisation du nombre de vies
               Led8Off; //Led de derni�re vie �teinte
           }

           //�teindre led pour chaque vie perdue
           switch(vie) {
           case 1 : Led4Off; break;
           case 2 : Led5Off; break;
           case 3 : Led6Off; break;
           case 4 : Led7Off; break;
           }

    }
    else{//�criture de no internet

      //�criture du d�cors fixe sur l'�cran et leds �teintes
      Led4Off;Led5Off;Led6Off;Led7Off;Led8Off;
      Dogs102x6_verticalLineDraw(0, 64, 0, 0); //cadre
      Dogs102x6_verticalLineDraw(0, 64, 102, 0); //cadre
      Dogs102x6_horizontalLineDraw(0, 102, 0, 0); //cadre
      Dogs102x6_horizontalLineDraw(0, 102, 64, 0); //cadre

      //saut du personnage
      if (jump){//variable modifi�e par l'interuption du poussoir

          //effa�age du personnage sur l'�cran
          Dogs102x6_verticalLineDraw(53, 57, 10, 1); //efface le personnage au sol (jambe)
          Dogs102x6_pixelDraw(11, 50, 1); // efface le pixel du bras
          Dogs102x6_lineDraw(10,53, 13, 57, 1); //efface la deuxi�me jambe
          Dogs102x6_circleDraw(16, 50,2, 1); //efface la t�te si personnage baiss�
          Dogs102x6_horizontalLineDraw(12, 14, 50, 1); //efface le corps si personnage baiss�
          Dogs102x6_circleDraw(10, 45-cpt,2, 1); //efface ancienne position de la t�te
          Dogs102x6_verticalLineDraw(53-cpt, 48-cpt, 10, 1); //efface ancienne position du corps
          Dogs102x6_lineDraw(10,53-cpt, 13, 57-cpt, 1); //efface ancienne position d'une jambe
          Dogs102x6_lineDraw(10,53-cpt, 7, 57-cpt, 1); //efface ancienne position de la deuxi�me jambe

          //compteur pour la position du personnage dans l'air
          if (down==1) {cpt++;} //mont�e
          else {cpt--;} //descente
          if (cpt==saut){down=0;} //sommet donc down=0 pour redescendre
          if (cpt>saut+2){down=0;}
          if (cpt==0){jump=0; down=1;} //retour au sol

          //�criture du personnage sur l'�cran durant le saut
          Dogs102x6_circleDraw(10, 45-cpt,2, 0);
          Dogs102x6_verticalLineDraw(53-cpt, 48-cpt, 10, 0);
          Dogs102x6_lineDraw(10,53-cpt, 13, 57-cpt, 0);
          Dogs102x6_lineDraw(10,53-cpt, 7, 57-cpt, 0);
      }

      //personnage au sol pendant la course
      else{
          //�criture des parties fixes du personnage
          Dogs102x6_lineDraw(10,53, 7, 57, 1); //efface la seconde jambe du personnage lors du saut
          Dogs102x6_verticalLineDraw(53, 48, 10, 0); //�criture du corps du personnage
          if (duck==0) {Dogs102x6_circleDraw(10, 45,2, 0);} //�criture de la t�te si personnage pas baiss�
          Dogs102x6_circleDraw(16, 50,2, 1); //efface la t�te du personnage baiss�
          Dogs102x6_horizontalLineDraw(12, 14, 50, 1); //efface le corps du personnage baiss�

          //�criture du bras et premi�re jambe du personnage lorsque cpt va de 0 � 10
          if (cpt<10) {
              Dogs102x6_pixelDraw(11, 50, 0);
              Dogs102x6_verticalLineDraw(53, 57, 10, 0);
              Dogs102x6_lineDraw(10,53, 13, 57, 1);
          }
          else {//efface le bras et �crit la seconde jambe lorsque cpt va de 10 � 20
              Dogs102x6_pixelDraw(11, 50, 1);
              Dogs102x6_lineDraw(10,53, 13, 57, 0);
              Dogs102x6_verticalLineDraw(54, 57, 10, 1);
          }

          if (cpt==20){cpt=0;} //retour du compteur � z�ro
          cpt++;

          //�criture du personnage lorsqu'il se baisse
          if (duck){
              Dogs102x6_circleDraw(16, 50,2, 0); //�crit position de la t�te
              Dogs102x6_horizontalLineDraw(12, 14, 50, 0); //�crit position du corps
              Dogs102x6_circleDraw(10, 45,2, 1); //efface la t�te du personnage debout
              Dogs102x6_verticalLineDraw(48, 49, 10, 1); //efface la corps du personnage debout
          }
      }

      //modification de la vitesse du jeu
      //si le score est �gal � 2 ou � 80 l'interuption int�rvient tout les 60000 du timer (acc�l�ration du jeu)
      if ((score==2 && score2==0 && score3==0) || (score2==8 && score3==0)) {
          TA2CCR0 = 60000;
      }
      //si le score est �gal � 10 ou � 100 l'interuption int�rvient tout les 55000 du timer
      if ((score2==1 && score3==0) || score3==1) {
          TA2CCR0 = 55000;
      }
      //si le score est �gal � 20 ou � 200 l'interuption int�rvient tout les 50000 du timer
      if ((score2==2 && score3==0) || score3==2) {
          TA2CCR0 = 50000;
          Dogs102x6_setInverseDisplay();//inverse les pixels donc blanc sur noir
      }
      //si le score est �gal � 30 ou � 300 l'interuption int�rvient tout les 45000 du timer
      if ((score2==3 && score3==0) || score3==3) {
          TA2CCR0 = 45000;
          Dogs102x6_clearInverseDisplay();//inverse les pixels donc noir sur blanc
      }
      //si le score est �gal � 60 l'interuption int�rvient tout les 65000 du timer
      // et le comptage s'incr�mente de 1.5 aulieu de 1 � chaque interruption pour acc�l�r�er le jeu
      if (score2==6 && score3==0) {
          speednum=3;speednom=2;
          saut=13;//r�duit la hauteur du saut
          TA2CCR0 = 65000;
      }
      //modifie le comptage pour incr�menter � 3/2 aulieu de 1
      if (cpt4==speednom){//diviseur
      cpt3=cpt3+speednum; cpt4=0;}//multiple
      cpt4++;

      //d�tection de collision entre personnage et objet, augmentation du score et �criture des obstacles
      if (countdown>200){//attente avant les premiers obstacles au d�but du jeu

          //quatres obstacles se repette � chaque fois, il peuvent prendre trois formes diff�rentes
          //choisient au hasard: rocher, oiseau qui vole bas et oiseau qui vole haut
          if (cpt3<10 && cpt3>0) { r1=rand() % 3; r2=rand() % 3;}//changement de forme de l'objet 1 et 2 lorsqu'ils ne sont pas sur l'�cran
          if (cpt3<50 && cpt3>40){r3=rand() % 3;}//changement de forme de l'objet 3 lorsqu'il n'est pas sur l'�cran
          if (cpt3<120 && cpt3>110) {r4 = rand() % 3;}//changement de forme de l'objet 4 lorsqu'il n'est pas sur l'�cran

          //d�tection de collision et comptage du score
          //objet 1
          if (cpt3<35 && cpt3>15){//objet 1 lorsqu'il est � la position du personnage
              if ((r3 == 2 || r3==1) && jump==0){jeu=0;} //si le personnage n'a pas saut� pour rocher et oiseau bas --> retour au menu
              else if (r3==0 && duck==0){jeu=0;} //si le personnage ne s'est pas baiss� pour oiseau haut --> jeu=0, retour au menu
              if (cpt3 > 27 && L1==0)  {score++;L1=1;} //comptage score (varibale L1 utile pour compter une seul fois le score)
          }                                            //et pas cpt3==27 car incr�mentation � 1.5
          if (cpt3>34 && cpt3< 45) {L1=0;}//remet la variable � zero lorsque l'objet � quitt� l'�cran
          objet(cpt3-70,cpt,r1); //�criture de l'objet sur l'�cran (voir fonction objet)

          //objet 2
          if (cpt3<-95 && cpt3>-115){//27
               if ((r1 == 2 || r1==1) && jump==0){jeu=0;}
               else if (r1==0 && duck==0){jeu=0;}
               if (cpt3 < -100 && L2==0) {score++;L2=1;}
          }
          if (cpt3>-94 && cpt3<-85){L2=0;}
          objet(cpt3-200,cpt,r3);

          if (te){//enl�ve les deux premiers obstacles du d�but du jeu car sinon ils appara�trait juste devant le personnage

               //objet 3
               if (cpt3<-15 && cpt3>-35){//27
                   if ((r2 == 2 || r2==1) && jump==0){jeu=0;}
                   else if (r2==0 && duck==0){jeu=0;}
                   if (cpt3<-25 && L3==0) {score++;L3=1;}
               }
               if (cpt3>-14 && cpt3<-5) {L3=0;}
               objet(cpt3-150,cpt,r2);

               //objet 4
               if (cpt3<91 && cpt3>71){//27
                   if ((r4 == 2 || r4==1) && jump==0){jeu=0;}
                   else if (r4==0 && duck==0){jeu=0;}
                   if (cpt3>80 && L4==0) {score++;L4=1;}
               }
               if(cpt3>90 && cpt3<100) {L4=0;}
               objet(cpt3,cpt,r4);
          }
          if (cpt3>120){te=1;}
      }
      else {countdown++;}

      //�criture du sol
      Dogs102x6_horizontalLineDraw(0, 102, 55, 0); //sol
      uint8_t i;
      for (i=0;i<15;i++){
          sol (cpt3+40*i,1);//fonction du sol avec deux types diff�rents
          sol (cpt3+60*i,0);
      }
   }
   break;
  }
}

//fonction de l'�criture du sol
void sol (cpt,type){

    int pos;
    int pos2;
    if (type) {pos=60; pos2=57;} //choix de la position du sol
    else {pos=58;  pos2=62;}

    //effa�age du sol
    cpt2=cpt-speednum;
    Dogs102x6_pixelDraw(102-cpt2, pos, 1);
    Dogs102x6_pixelDraw(90-cpt2, pos, 1);
    Dogs102x6_pixelDraw(92-cpt2, pos2, 1);
    Dogs102x6_pixelDraw(93-cpt2, pos2, 1);
    Dogs102x6_pixelDraw(94-cpt2, pos2, 1);

    cpt2=cpt; //avancement du sol

    //�criture du sol
    Dogs102x6_pixelDraw(102-cpt2, pos, 0);
    Dogs102x6_pixelDraw(90-cpt2, pos, 0);
    Dogs102x6_pixelDraw(92-cpt2, pos2, 0);
    Dogs102x6_pixelDraw(93-cpt2, pos2, 0);
    Dogs102x6_pixelDraw(94-cpt2, pos2, 0);
}

//fonction de l'�criture des objets
void objet (cpt,cpt1,random){

    int pos;
    switch (random) {
    case 0 : pos = 44; break; //oiseau haut
    case 1 : pos = 51; break; //oiseau bas
    case 2 : pos = 0; break; //rocher
    }

    cpt2=cpt-speednum;
    if (pos){ //�criture oiseau haut et bas
        //effa�age de l'ancienne position de l'oiseau
        Dogs102x6_pixelDraw(90-cpt2, pos, 1);
        Dogs102x6_pixelDraw(91-cpt2, pos, 1);
        Dogs102x6_pixelDraw(92-cpt2, pos, 1);
        Dogs102x6_pixelDraw(93-cpt2, pos, 1);
        Dogs102x6_pixelDraw(94-cpt2, pos, 1);
        Dogs102x6_pixelDraw(92-cpt2, pos-1, 1);
        Dogs102x6_pixelDraw(92-cpt2, pos+1, 1);
        Dogs102x6_pixelDraw(93-cpt2, pos-2, 1);
        Dogs102x6_pixelDraw(93-cpt2, pos+2, 1);
        Dogs102x6_pixelDraw(93-cpt2, pos-3, 1);
        Dogs102x6_pixelDraw(93-cpt2, pos+3, 1);
        Dogs102x6_pixelDraw(93-cpt2, pos-1, 1);
        Dogs102x6_pixelDraw(93-cpt2, pos+1, 1);

        cpt2=cpt;

        //�criture des parties fixes de l'oiseau
        Dogs102x6_pixelDraw(90-cpt2, pos, 0);
        Dogs102x6_pixelDraw(91-cpt2, pos, 0);
        Dogs102x6_pixelDraw(92-cpt2, pos, 0);
        Dogs102x6_pixelDraw(93-cpt2, pos, 0);
        Dogs102x6_pixelDraw(94-cpt2, pos, 0);

        //�criture des ailes en mouvement de l'oiseau
        if (cpt1<10) {
            Dogs102x6_pixelDraw(92-cpt2, pos-1, 0);
            Dogs102x6_pixelDraw(92-cpt2, pos+1, 1);
            Dogs102x6_pixelDraw(93-cpt2, pos-2, 0);
            Dogs102x6_pixelDraw(93-cpt2, pos+2, 1);
            Dogs102x6_pixelDraw(93-cpt2, pos-3, 0);
            Dogs102x6_pixelDraw(93-cpt2, pos+3, 1);
            Dogs102x6_pixelDraw(93-cpt2, pos-1, 0);
            Dogs102x6_pixelDraw(93-cpt2, pos+1, 1);
        }
        else {
            Dogs102x6_pixelDraw(92-cpt2, pos+1, 0);
            Dogs102x6_pixelDraw(92-cpt2, pos-1, 1);
            Dogs102x6_pixelDraw(93-cpt2, pos-2, 1);
            Dogs102x6_pixelDraw(93-cpt2, pos+2, 0);
            Dogs102x6_pixelDraw(93-cpt2, pos-3, 1);
            Dogs102x6_pixelDraw(93-cpt2, pos+3, 0);
            Dogs102x6_pixelDraw(93-cpt2, pos-1, 1);
            Dogs102x6_pixelDraw(93-cpt2, pos+1, 0);
        }

    Dogs102x6_pixelDraw(102, pos-1, 0);
    Dogs102x6_pixelDraw(102, pos+1, 0);
    Dogs102x6_pixelDraw(102, pos-2, 0);
    Dogs102x6_pixelDraw(102, pos+2, 0);
    Dogs102x6_pixelDraw(102, pos-3, 0);
    Dogs102x6_pixelDraw(102, pos+3, 0);
    }
    else {//�criture du rocher
        //effa�age de l'ancienne position du rocher
        Dogs102x6_verticalLineDraw(58, 51, 88-cpt2, 1);
        Dogs102x6_verticalLineDraw(58, 51, 89-cpt2, 1);
        Dogs102x6_verticalLineDraw(58, 51, 90-cpt2, 1);
        Dogs102x6_verticalLineDraw(58, 51, 91-cpt2, 1);
        Dogs102x6_verticalLineDraw(58, 51, 92-cpt2, 1);
        cpt2=cpt;
        //�criture du rocher
        Dogs102x6_verticalLineDraw(58, 51, 88-cpt2, 0);
        Dogs102x6_verticalLineDraw(58, 51, 89-cpt2, 0);
        Dogs102x6_verticalLineDraw(58, 51, 90-cpt2, 0);
        Dogs102x6_verticalLineDraw(58, 51, 91-cpt2, 0);
        Dogs102x6_verticalLineDraw(58, 51, 92-cpt2, 0);
    }
}
