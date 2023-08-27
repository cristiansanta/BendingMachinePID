#include <avr/wdt.h>
#include <util/atomic.h>
#include "Servo.h"//Libreria
#include <LiquidCrystal.h>;
LiquidCrystal lcd(13, 12, 8, 54, 10, 11);
#include <Keypad.h>

Servo santa;
Servo Pro;
const byte r=55;
const byte g=56;
const byte b=57;

int n=6,n1=0,var=11,Gancho=0,c=0,var1=0,var2=0,k=0,m=0;
int Cantidad_estribos=0,Long_Ing=0,Long_Ancho=0,Long_Largo=0;
bool FlagR=false,FlagC=false,FlagR1=true,FlagC1=true;
const byte DT = 2;
const byte CLK = 4;

const byte EN1 = 5;//Inyectar PWM para controlar velocidad motor
byte engine_1 = HIGH, engine_2 = LOW;
//const byte PWM=99;//VELOCIDAD MOTOR

// globals
long prevT = 0;
int posPrev = 0;

float v1Filt = 0;
float v1Prev = 0;

float eintegral = 0;
float pwr=0;float posi=0;

volatile int Posicion_Encoder = 0;
int State;
int LastState;

const float pi = 3.14;
const float R = 3.25;
const int Npasos = 40;//20 pasos por vuelta osea 2 vueltas para este caso

float distance = 0;

const byte ROWS=4;
const byte COLS=4;

bool una=true;
bool estado_men=false;
bool estado_Cant_Cuadrado = false;
bool estado1_Ancho=true;///////////----------------------
bool estado2_Largo = false;
bool estado3_Cantidad = true;

char keys[ROWS][COLS]={{'1','2','3','A'},
                       {'4','5','6','B'},
                       {'7','8','9','C'},
                       {'*','0','#','D'}};
byte rowPins[ROWS]={17,16,15,14};
byte colPins[COLS]={21,20,19,18};
Keypad teclado1 = Keypad( makeKeymap(keys),rowPins,colPins,ROWS,COLS);


void setup() {
  wdt_disable();
  Serial.begin(9600);
  lcd.begin(20,4);
  pinMode (DT , INPUT);
  pinMode (CLK , INPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  santa.attach(3);
  Pro.attach(9);
  attachInterrupt(digitalPinToInterrupt(DT),Data,RISING);
  n1=n;
}
void Data(){
  static unsigned long LastState=0;
  unsigned long timeInterrupt = millis();

  if(timeInterrupt - LastState > 5){
    if(digitalRead(CLK)==HIGH){
      Posicion_Encoder++;
    }
    else{
      Posicion_Encoder--;
    }
    LastState = timeInterrupt;
  }
}
void FinProceso(){
  digitalWrite(g, LOW);
  digitalWrite(r, HIGH);
  Pro.write(0);
  delay(190);
  Pro.write(90);
  delay(190);   
}
void MiSeervo(){
  digitalWrite(g, LOW);
  digitalWrite(r, LOW);
  digitalWrite(b, HIGH);
  santa.write(0);
  delay(235);
  santa.write(90);
  delay(235);
}
void Stop1() {
    digitalWrite(7,!engine_1);
    digitalWrite(6, engine_2);
    delayMicroseconds(10);
}
void TurnChange(){
    engine_1 = !engine_1;
    engine_2 = !engine_2;
    digitalWrite(g, HIGH); 
}
void HorsePower(){ 
    digitalWrite(7, engine_1);
    digitalWrite(6, engine_2);
    analogWrite(EN1, pwr);
    digitalWrite(r, LOW);
    digitalWrite(b, LOW); 
    digitalWrite(g, HIGH);
}
void LCDVisor(){
    if(FlagR==true){
      FlagR1=false;
      lcd.home();
      lcd.print("HACIENDO RECTANGULOS");    
    }
    if(FlagC==true){
      FlagC1=false;
      lcd.home();
      lcd.print("HACIENDO CUADRADOS  ");    
    }
    lcd.setCursor(3, 1);
    lcd.print(distance);
    lcd.setCursor(13, 1);
    lcd.print("CM");
    lcd.setCursor(0,2);lcd.print("Flejes Por Hacer:");lcd.print(Cantidad_estribos);
    lcd.setCursor(0,3);lcd.print("Flejes Hechos:");
}
void DesplazamientoRectangulo(){
    while(distance>=n && k==c){
        Stop1();MiSeervo();k++;
        c++;
        if(c==1 || c==3){      //Ancho
        n+=Long_Ancho;}
        else if(c==2 || c==4){//Largo
        n+=Long_Largo;}
      else if(c<=5){//Ultimo recorrido para el gancho
        n+=var;
        }
      else if(c<=6){
        k++;
      }
  }
}
void Desplazamiento(){
  while(distance>=n && k==c)
       {
          Stop1();MiSeervo();k++;
          c++;//Serial.print("c:");Serial.println(c);Serial.print("DISTANCIA=");Serial.println(distance);
          if(c<=4)
          {
            n+=Long_Ing;
          }
          else if(c<=5){//Ultimo recorrido para el gancho
            n+=var;
         }
         else if(c<=6){
        k++;
      }
  }
}
void Retrocede(){
     while(distance>=n && k==7)
     {
        TurnChange();k++;
     }
}
void CortarFleje(){
     while(distance<=var2 && k==8)
     {
          Stop1();FinProceso();TurnChange();m++;
          n=var1+n1;c=0;k=0;
          lcd.setCursor(16,3);lcd.print(m);
          Serial.print("Estribo: ");Serial.println(m);
          if(m>1)
          {
            n=var2+n1;c=0;k=0;
          }
      var2+=var1;//good Actualiza Variable new F
      while(m==Cantidad_estribos)
      { if(FlagR1==false){
            FlagR1=true;
            lcd.home();
            lcd.print("RECTANGULOS ACABADOS");    
            }
        if(FlagC1==false){
            FlagC1=true;
            lcd.home();
            lcd.print("CUADRADOS ACABADOS  ");    
            }    
        digitalWrite(g, LOW);
        digitalWrite(r, LOW);
        digitalWrite(b, LOW);
        Serial.println();Serial.print("Cantidad de Flejes Hechos:");
        Serial.println(m); //while(Serial.available() == 0){};//Detener letreros con Serial.available() en el virtual terminal
        Stop1();c=0;k=0;
while(true){
           char pulsacion=tecla_time();
            if(pulsacion!='*' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D' && pulsacion!='#' && pulsacion!='1' && pulsacion!='2' && pulsacion!='0' && pulsacion!='9' && pulsacion!='8' && pulsacion!='7' && pulsacion!='6' && pulsacion!='5' && pulsacion!='4' && pulsacion!='3')
              {//Pulsación 'A' PARADA DE EMERGENCIA -- Regresar Menú Principal
                if(pulsacion=='A'){
                      Cantidad_estribos=0;
                      estado_Cant_Cuadrado=false;estado_men=true;una=true;MENU();                         
                      reset();
                  }
              }
          }
      }
   }
}
void reset(){//1er metodo reset Recomendado X que reiniciar la placa es la función del watchdog en cualquier placa
  wdt_enable(WDTO_500MS);
  while(1){};
}
//-------------------------------------------------------------------------------------------
/*===========================================================================*/
/*=========================    FUNCION TECLA CON TIMER    ===================*/
/*===========================================================================*/
/*PREGUNTA POR UNA TECLA POR U TIEMPO DETERMINADO Y SI NO HAY ACTIVIDAD, DEJA DE
PREGUNTAR Y DEJA QUE EL PIC CONTINUE CON SU TRABAJO*/
char tecla_time(void){
  char c='\0'; unsigned long timeout;
  timeout=0;
  c=teclado1.getKey();
  while(c=='\0' && (++timeout < 20000))
  {
    delayMicroseconds(10);
    c=teclado1.waitForKey();
  }
  return c;
}
///////////////////////////////////////////////////////MENU PRINCIPAL
void MENU()
{
  if(una==true)
  {
    lcd.home();
    lcd.print("       MENU:        ");
    lcd.setCursor(0,1);
    lcd.print("1)CUADRADO          ");
    lcd.setCursor(0,2);
    lcd.print("2)RECTANGULO        ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    una=false;
    estado_men=true;
  }
}
////////////////////////////////////////CUADRADO
void V_longi()
{
  lcd.setCursor(1,0);
  lcd.print("LONGITUD CUADRADO:");
  lcd.setCursor(0,1);
  lcd.print("MEDIDA EN CM:   ");
  lcd.setCursor(0,3);
  lcd.print("*)Cancelar #)Aceptar");
  estado_men=false;
}
void objetos()
{
  lcd.home();
  lcd.print("CANTIDAD CUADRADOS:");
  lcd.setCursor(0,1);
  lcd.print("CANTIDAD:          ");
  lcd.setCursor(0,3);
  lcd.print("*)Cancelar #)Aceptar");
  estado_Cant_Cuadrado=true;
}
////////////////////////////////////////Rectangulo
void LadoAncho()
{
  lcd.home();
  lcd.print("MEDIDAS RECTANGULO: ");
  lcd.setCursor(0,1);
  lcd.print("ANCHO:   ");
  lcd.setCursor(0,2);
  lcd.print("LARGO:");
  lcd.setCursor(0,3);
  lcd.print("*)Cancelar #)Aceptar");
  estado1_Ancho=false;
}
void LadoLargo()
{
  lcd.setCursor(0,2);
  lcd.print("LARGO:   ");
  lcd.setCursor(0,3);
  lcd.print("*)Cancelar #)Aceptar");
  estado2_Largo=true;
}
void Cantidad()
{
  lcd.home();
  lcd.print("CANTIDAD RECTANGULOS");
  lcd.setCursor(0,1);
  lcd.print("CANTIDAD:          ");
  lcd.setCursor(0,2);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("*)Cancelar #)Aceptar");
  estado3_Cantidad=false;
}
//----------------------------------------------------------
void PID(){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
  posi=Posicion_Encoder;
  }
  // Compute velocity with method 1-->ESTE
  long currT = micros();
  float deltaT = ((float) (currT-prevT))/1.0e6;
  float velocity1 = (posi - posPrev)/deltaT;  
  posPrev = posi;
  prevT = currT;

  // Convert count/s to RPM
  float v1 = velocity1/600.0*60.0;
  
/*  // Low-pass filter (25 Hz cutoff)
 *     FORMULA:
 *  v_filt[n]=0.854*v_filt[n-1]*0.0728*v[n]+0.0728*v[n-1];  ECUACION DE DIFERENCIA
 */
  v1Filt = 0.854*v1Filt + 0.0728*v1 + 0.0728*v1Prev;//Lo que me arroja el sensor
  v1Prev = v1;
  
  // Set a target O SETPOINT Lo que quiero
  float vt = 100*(sin(currT/1e6)>0);

  // Compute the control signal u
  float kp = 5;
  float ki = 10;
  float e = vt-v1Filt;
  eintegral = eintegral + e*deltaT;
  
  // control signal==SEÑAL DE CONTROL
  float u = kp*e + ki*eintegral;
  
  //Motor Power
  pwr = (float) fabs(u);
  if(pwr > 255){
    pwr = 255;
  }
}
//----------------------------------------------------------
void loop() {
  MENU();
  String words,words_1;///Variables Cuadrado
  char Cantidad_OB, L_varilla, pulsacion;//Locales
  String words_A,words_L,words_C;///Variables///Variables Rectangulo
  char L_varilla_A,L_varilla_L,Cantidad_OB_R;
  
  pulsacion = tecla_time();//FUNCION THAT recoge el valor de la funcion tecla_time INTERRUPCION(interna)==Timer
 if(pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D' && pulsacion!='*' && pulsacion!='#' && pulsacion!='0' && pulsacion!='9' && pulsacion!='8' && pulsacion!='7' && pulsacion!='6' && pulsacion!='5' && pulsacion!='4' && pulsacion!='3')//Pregunta cual fue la eleccion en el menu pricipal
 {
      if(pulsacion !=0)//Pregunta si hay cualquier pulsacion entonces Limpia Pantalla
      {
        lcd.clear();delay(72);
      }
      if(estado_men==true) //Bandera que cambio de estado en menu principal y redirigio a este IF
      {         
           if(pulsacion=='1')//Pregunta cual fue la eleccion en el menu pricipal
           {
              V_longi();///Llama la funcion para entrar datos del Cuadraddo
              PID();//CONTROL PID MOTOR
           }
           if(pulsacion=='2')//Pregunta cual fue la eleccion en el menu pricipal
           {
              LadoAncho();///Llama la funcion para entrar datos del Rectangulo
              PID();//CONTROL PID MOTOR
           }
     }
}
  if(estado_men==false)//Capturar datos Longitud en CM
  {        int k=0;
           lcd.setCursor(14,1);
   while(k<=2){
           pulsacion=tecla_time();//FUNCION THAT recoge el valor de la funcion tecla_time INTERRUPCION(interna)==Timer
            if(k==0){
                    if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D' && pulsacion!='#'))
                     {
                      if(k==0 || k==1)
                        {
                           L_varilla=pulsacion;//Guarda Caracteres LONGITUD VARILLA
                           if(L_varilla=='#')
                           break;
                           else if(L_varilla!='A' && L_varilla!='B' && L_varilla!='C' && L_varilla!='D' && L_varilla!='*')
                           {lcd.print(L_varilla);}
                           words+=L_varilla;//Concatenar Caracteres
                           k++; 
                        }
                       else if(k==2 && pulsacion=='#'){break;}
                   }
            }
          else if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D'))
                 {
                   if(k==0 || k==1)
                     {
                       L_varilla=pulsacion;//Guarda Caracteres LONGITUD VARILLA
                       if(L_varilla=='#')
                       break;
                       else if(L_varilla!='A' && L_varilla!='B' && L_varilla!='C' && L_varilla!='D' && L_varilla!='*')
                       {lcd.print(L_varilla);}
                       words+=L_varilla;//Concatenar Caracteres
                       k++; 
                   }
                  else if(k==2 && pulsacion=='#'){break;}
            }
         if(pulsacion=='*'){k=3;L_varilla='\0';words="";estado_men=true;una=true;MENU();}
      }
           if(words.length() >0)//DETECTA SI SE INGRESO ALGUN VALOR DE LONGITUD PARA CONTINUAR CON CATIDAD OBJETOS
           {
            objetos();//LLAMA FUNCION PARA GUARDAR CARACTERES
           }
  }
  if(estado_Cant_Cuadrado==true)//Captura datos Cantidad de cuadrados a hacer
  {      int m=0;
         lcd.setCursor(10,1);
   while(m<=2){
         pulsacion=tecla_time();/////////////////
     if(m==0){
           if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D' && pulsacion!='#'))
             {
               if(m==0 || m==1)
                 {
                   Cantidad_OB=pulsacion;
                   if(Cantidad_OB=='#')
                   break;
                   else if(Cantidad_OB!='A' && Cantidad_OB!='B' && Cantidad_OB!='C' && Cantidad_OB!='D' && Cantidad_OB!='*'){
                   lcd.print(Cantidad_OB);}
                   words_1+=Cantidad_OB;//Concatenar Caracteres
                   m++;
                 }
                else if(m==2 && pulsacion=='#'){break;}
           }
     }
    else if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D'))
           {
         if(m==0 || m==1)
           {
              Cantidad_OB=pulsacion;
              if(Cantidad_OB=='#')
              break;
              else if(Cantidad_OB!='A' && Cantidad_OB!='B' && Cantidad_OB!='C' && Cantidad_OB!='D' && Cantidad_OB!='*'){
              lcd.print(Cantidad_OB);}
              words_1+=Cantidad_OB;//Concatenar Caracteres
              m++;
          }
         else if(m==2 && pulsacion=='#'){break;}
        }
  if(pulsacion=='*'){m=3;Cantidad_OB='\0';L_varilla='\0';words_1="";words="";estado_Cant_Cuadrado=false;una=true;MENU();}
   }
         if((words_1.length()>0) && (words.length()>0))
         {
           Cantidad_estribos=words_1.toInt();
           Long_Ing=words.toInt();
           var1=n*2+(Long_Ing*4);//MEDIDA CONSTANTE NO CAMBIA
           var2=var1;
           lcd.home();
           lcd.print("     CUADRADO:      ");
           lcd.setCursor(0,1);
           lcd.print("MEDIDA:     CM      ");
           lcd.setCursor(9,1);
           lcd.print(Long_Ing);
           lcd.setCursor(0,2);
           lcd.print("CANTIDAD:   UNIDADES");
           lcd.setCursor(9,2);
           lcd.print(Cantidad_estribos);
           lcd.setCursor(0,3);
           lcd.print("*)ANULAR   #)INICIAR");
 while(Cantidad_estribos>0 && Long_Ing>0) {
           FlagC=true;FlagR=false;
           pulsacion=tecla_time();
   if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D' && pulsacion!='1' && pulsacion!='2' && pulsacion!='0' && pulsacion!='9' && pulsacion!='8' && pulsacion!='7' && pulsacion!='6' && pulsacion!='5' && pulsacion!='4' && pulsacion!='3'))//Pregunta cual fue la eleccion en el menu pricipal
     {
           if(pulsacion !=0){lcd.clear();delay(72);}
           if(pulsacion=='*')//Regresa Main Menú
           {
              Cantidad_estribos=0;Long_Ing=0;
              L_varilla='\0';Cantidad_OB='\0';words_1="";words="";
              estado_Cant_Cuadrado=false;estado_men=true;una=true;MENU();break;
            }
           else if(pulsacion=='#')
           {
                while(Cantidad_estribos>0){
                            PID();//CONTROL PID MOTOR
                            HorsePower();                            
                            distance = round(((2 * pi * R) / Npasos) * posi);
                            LCDVisor();
                      //----------------------------------1)
                            Desplazamiento();
                      //----------------------------------2)
                            Retrocede();
                      //----------------------------------3)
                            CortarFleje();
                      //----------------------------------
                }
           }
        }
      } 
    }
  }
  if(estado1_Ancho==false)//Bandera que cambio de estado en funcion objetos y redirigio a este IF
  {        int kR=0;
           lcd.setCursor(9,1);
           lcd.print("CM");
           lcd.setCursor(7,1);
   while(kR<=2){
           pulsacion=tecla_time();//Captura el valor de ancho maximo 2 digitos
      if(kR==0)    
        {  if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D' && pulsacion!='#'))
             {
               if(kR==0 || kR==1)
                 {
                   L_varilla_A=pulsacion;//Guarda Caracteres LONGITUD VARILLA
                   if(L_varilla_A=='#')
                   break;
                   else if(L_varilla_A!='A' && L_varilla_A!='B' && L_varilla_A!='C' && L_varilla_A!='D' && L_varilla_A!='*')
                   {lcd.print(L_varilla_A);}
                   words_A+=L_varilla_A;//Concatenar Caracteres
                   kR++; 
               }
               else if(kR==2 && pulsacion=='#'){break;} 
            }
        }    
      else if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D'))
             {
           if(kR==0 || kR==1)
             {
                 L_varilla_A=pulsacion;//Guarda Caracteres LONGITUD VARILLA
                 if(L_varilla_A=='#')
                 break;
                 else if(L_varilla_A!='A' && L_varilla_A!='B' && L_varilla_A!='C' && L_varilla_A!='D' && L_varilla_A!='*')
                 {lcd.print(L_varilla_A);}
                 words_A+=L_varilla_A;//Concatenar Caracteres
                 kR++; 
               }
            else if(kR==2 && pulsacion=='#'){break;} 
            }        
      if(pulsacion=='*'){kR=3;L_varilla_A='\0';words_A="";estado1_Ancho=true;una=true;MENU();}
  }
           if(words_A.length() >0)//Si ya esta ingresado el ancho>0 ahora vamos por el largo
           {
            LadoLargo();//LLAMA FUNCION PARA GUARDAR CARACTERES
           }
  }
  if(estado2_Largo==true)
  {      int mR=0;
         lcd.setCursor(9,2);
         lcd.print("CM");
         lcd.setCursor(7,2);
   while(mR<=2){
         pulsacion=tecla_time();/////////////////Captura el Valor de Largo
     if(mR==0)
       {
         if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D' && pulsacion!='#'))
           {
              if(mR==0 || mR==1)
                {
                   L_varilla_L=pulsacion;
                   if(L_varilla_L=='#')
                   break;
                   else if(L_varilla_L!='A' && L_varilla_L!='B' && L_varilla_L!='C' && L_varilla_L!='D' && L_varilla_L!='*'){
                   lcd.print(L_varilla_L);}
                   words_L+=L_varilla_L;//Concatenar Caracteres
                   mR++;
                 }
              else if(mR==2 && pulsacion=='#'){break;}
            }      
     }
else if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D'))
       {
             if(mR==0 || mR==1)
               {
                 L_varilla_L=pulsacion;
                 if(L_varilla_L=='#')
                 break;
                 else if(L_varilla_L!='A' && L_varilla_L!='B' && L_varilla_L!='C' && L_varilla_L!='D' && L_varilla_L!='*'){
                 lcd.print(L_varilla_L);}
                 words_L+=L_varilla_L;//Concatenar Caracteres
                 mR++;
             }
            else if(mR==2 && pulsacion=='#'){break;}
       }
  if(pulsacion=='*'){mR=3;L_varilla_L='\0';L_varilla_A='\0';words_A="";words_L="";estado2_Largo=false;estado1_Ancho=true;una=true;MENU();}
}
         if((words_A.length()>0) && (words_L.length()>0))
         {
            Cantidad();
         }
 }
  if(estado3_Cantidad==false)
  {        int nR=0;
           lcd.setCursor(11,1);
   while(nR<=2){
           pulsacion=tecla_time();//Captura el valor de Cantidad Rectangulos
 if(nR==0)
   {
          if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D' && pulsacion!='#'))
            {
               if(nR==0 || nR==1)
                 {
                   Cantidad_OB_R=pulsacion;//Guarda Caracteres LONGITUD VARILLA
                   if(Cantidad_OB_R=='#')
                   break;
                   else if(Cantidad_OB_R!='A' && Cantidad_OB_R!='B' && Cantidad_OB_R!='C' && Cantidad_OB_R!='D' && Cantidad_OB_R!='*')
                   {lcd.print(Cantidad_OB_R);}
                   words_C+=Cantidad_OB_R;//Concatenar Caracteres
                   nR++; 
               }
              else if(nR==2 && pulsacion=='#'){break;} 
            }
  }
 else if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D'))
        {
               if(nR==0 || nR==1)
                 {
                   Cantidad_OB_R=pulsacion;//Guarda Caracteres
                   if(Cantidad_OB_R=='#')
                   break;
                   else if(Cantidad_OB_R!='A' && Cantidad_OB_R!='B' && Cantidad_OB_R!='C' && Cantidad_OB_R!='D' && Cantidad_OB_R!='*')
                   {lcd.print(Cantidad_OB_R);}
                   words_C+=Cantidad_OB_R;//Concatenar Caracteres
                   nR++;
               }
               else if(nR==2 && pulsacion=='#'){break;} 
        }             
  if(pulsacion=='*'){nR=3;Cantidad_OB_R='\0';words_C="";L_varilla_L='\0';L_varilla_A='\0';words_A="";words_L="";estado3_Cantidad=true;estado_men=true;una=true;MENU();}
}
           if((words_A.length()>0) && (words_L.length()>0) && (words_C.length()>0))//Si ya estan ingresados los 3 valores del rectangulo muestrelos
           {
              Long_Ancho=words_A.toInt();
              Long_Largo=words_L.toInt();
              Cantidad_estribos=words_C.toInt();
              var1=n*2+(Long_Ancho*2)+(Long_Largo*2);//MEDIDA CONSTANTE NO CAMBIA
              var2=var1;
           lcd.home();
           lcd.print("     RECTANGULO:    ");
           lcd.setCursor(0,1);
           lcd.print("ANC:  cm / LARG:  cm");
           lcd.setCursor(4,1);
           lcd.print(Long_Ancho);
           lcd.setCursor(16,1);
           lcd.print(Long_Largo);           
           lcd.setCursor(0,2);
           lcd.print("CANTIDAD:    Unidade");
           lcd.setCursor(10,2);
           lcd.print(Cantidad_estribos);
           lcd.setCursor(0,3);
           lcd.print("*)ANULAR   #)INICIAR");
    while(Cantidad_estribos>0 && Long_Largo>0 && Long_Ancho>0){
           FlagR=true;FlagC=false;           
           pulsacion=tecla_time();
if((pulsacion!='A' && pulsacion!='B' && pulsacion!='C' && pulsacion!='D' && pulsacion!='1' && pulsacion!='2' && pulsacion!='0' && pulsacion!='9' && pulsacion!='8' && pulsacion!='7' && pulsacion!='6' && pulsacion!='5' && pulsacion!='4' && pulsacion!='3'))//Pregunta cual fue la eleccion en el menu pricipal
    { 
           if(pulsacion !=0){lcd.clear();delay(72);}
           if(pulsacion=='*')
           {
                Long_Ancho=0;Long_Largo=0;Cantidad_estribos=0;
                L_varilla_A='\0';L_varilla_L='\0';Cantidad_OB_R='\0';words_A="";words_L="";words_C="";
                estado3_Cantidad=true;estado2_Largo=false;estado1_Ancho=true;una=true;MENU();break;
           }
            else if(pulsacion=='#')
            {
                while(Cantidad_estribos>0){
                            PID();//CONTROL PID MOTOR
                            HorsePower();                            
                            distance = round(((2 * pi * R) / Npasos) * posi);
                            LCDVisor();
                      //----------------------------------1)
                            DesplazamientoRectangulo();
                      //----------------------------------2)
                            Retrocede();
                      //----------------------------------3)
                            CortarFleje();
                      //----------------------------------
                }
            }
          }
       }
    }
  }
}
