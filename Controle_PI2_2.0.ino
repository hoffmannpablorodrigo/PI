#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc; // Instância do RTC DS3231

//===================================== CONTROLE ===================================== 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
unsigned long previousMillis = 0; // Armazena o tempo de referência
const unsigned long interval = 3000; // Intervalo de 3 segundos

#define rele 24

//Instances
OneWire oneWire(26);
DallasTemperature sensors(&oneWire);

//Var
DeviceAddress ds18; // Sensor
byte n; //Quantidade de sensores
//float fator_cal_sensor_1 = 0.25;
float t_calibrado;

//float fator_cal_sensor_1 = 0.00;
float fator_cal_sensor_2 = 0.94;

float setpoint = 0.0;
float sensor_1a = 0.0;
float sensor_1b = 0.0;
float get_temp_0 = 0.00;
float get_temp_1 = 0.00;
float vetor_media_movel_a [8]{0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00};
float vetor_media_movel_b [8]{0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00};
float mmovel_a = 0.00;
float mmovel_b = 0.00;
float mediamovel_a = 0.00;
float mediamovel_b = 0.00;
float t_amb = 0.0;
float trset = 0.0;
int flag1 = 0;
int flag2 = 0;
int flag3 = 0;
int flag4 = 0;
int flag5 = 0;
int flag_parar = 0;
int flagprint = 0;
float temp_plot = 0.00;
unsigned long tempoInicial;
int l = 0; 
int m = 0; 
float fator_tr = 0.00;
int j;
int k;
float media_movel_a(float sensor_1ab);
float media_movel_b(float sensor_1ab);
float flag_verde = 0;
float fator_tamb = 0.00;
int fator_tamb2 = 0;
//===================================== CONTROLE ===================================== 

//===================================== TELA ===================================== 

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <UTFTGLUE.h> 

#define TAMANHOMAXSENHA 9
//#define SENHAVALIDA "01234"
#define MINPRESSURE 300
#define MAXPRESSURE 2000
#define PRETO   0x0000
#define VERMELHO     0xF800
#define VERDE   0x07E0
#define BRANCO   0xFFFF

#define XP 8
#define XM A2
#define YP A3
#define YM 9

#define TS_LEFT 72
#define TS_RT 899
#define TS_TOP 916
#define TS_BOT  106
// Dimensões da tela no modo landscape
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// Configuração do Matrix
#define NUM_COLUMNS 20  // Número de colunas de caracteres
#define CHAR_HEIGHT 15  // Altura dos caracteres
#define CHAR_WIDTH  16  // Largura dos caracteres
#define NUM_ROWS (SCREEN_HEIGHT / CHAR_HEIGHT)

// Caracteres aleatórios e cor
char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$%&*"; // Caracteres do Matrix
int columnY[NUM_COLUMNS];      // Posição vertical da cascata de cada coluna
int cascadeLength[NUM_COLUMNS]; // Comprimento da cascata de cada coluna


float setpointx = 0.0;
int flag_tela = 0;
int flag_parado = 0;
int flag_gravando = 0;
float t_amb_preto = 0.00;
float temp_plot_preto = 0.00;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button botoes[13]; // Adicionado mais um botão
Adafruit_GFX_Button botoes_2[3];
MCUFRIEND_kbv tft;

bool pressionado = false;
int posX;
int posY;
String senha = "";


bool obterToque(void) {
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE);
    if (pressed) {
        posX = map(p.y, TS_LEFT, TS_RT, 0, 320); 
        posY = map(p.x, TS_TOP, TS_BOT , 0, 240);
        return true;
    }
    return false; 
}
//===================================== TELA =========================================

//================================= CARTÃO MICRO SD =================================
#include <SPI.h>
#include <SD.h>

const int chipSelect = 53; // Pino CS configurado
//================================= CARTÃO MICRO SD =================================


void setup(void) 
{

  //===================================== RTC =========================================

  Serial.begin(9600);
  
  Wire.begin();

  // Inicializa o RTC
  if (!rtc.begin()) {
    Serial.println("Erro: RTC não detectado!");
    while (1); // Bucle infinito
  }

  // ** Configuração de Data e Hora (descomente para configurar manualmente) **
  
  //rtc.adjust(DateTime(2025, 1, 20, 15, 32, 0)); // DD/MM/AAAA HH:MM:SS
  //Serial.println("Data e hora configuradas manualmente!");

  //===================================== RTC =========================================

//===================================== TELA =========================================
  uint16_t ID = tft.readID();
  tft.begin(ID);
  if (ID == 0x0 || ID == 0xFFFF) {
    Serial.println("Erro: Display não detectado!");
    while (1); // Pausa o programa
  }
  tft.begin(ID);
  tft.setRotation(1); // Modo landscape
  tft.fillScreen(0x0000); // Fundo preto

  // Inicializa as posições e comprimentos das cascatas
  for (int i = 0; i < NUM_COLUMNS; i++) {
    columnY[i] = random(-NUM_ROWS, 0) * CHAR_HEIGHT;      // Começa fora da tela
    cascadeLength[i] = random(5, 15); // Comprimento aleatório da cascata
  }

  for(int i =0; i < 20; i++) { // Loop infinito para a animação
    for (int col = 0; col < NUM_COLUMNS; col++) {
      int x = col * CHAR_WIDTH;

      // Apaga o último caractere fora da cascata
      int eraseY = columnY[col] - cascadeLength[col] * CHAR_HEIGHT;
      if (eraseY >= 0) {
        tft.fillRect(x, eraseY, CHAR_WIDTH, CHAR_HEIGHT, 0x0000); // Apaga com preto
      }

      // Desenha a cascata
      for (int i = 0; i < cascadeLength[col]; i++) {
        int y = columnY[col] - i * CHAR_HEIGHT;

        if (y >= 0 && y < SCREEN_HEIGHT) {
          char c = chars[random(0, sizeof(chars) - 1)];
          if (i == 0) {
            // O último caractere da cascata é mais brilhante
            tft.setTextColor(0xFFFF); // Branco
          } else {
            // Outros caracteres são verdes
            tft.setTextColor(0x07E0); // Verde
          }
          tft.setTextSize(2);
          tft.setCursor(x, y);
          tft.print(c);
        }
      }

      // Atualiza a posição da coluna
      columnY[col] += CHAR_HEIGHT;

      // Reinicia a cascata quando sair da tela
      if (columnY[col] - cascadeLength[col] * CHAR_HEIGHT > SCREEN_HEIGHT) {
        columnY[col] = 0;
        cascadeLength[col] = random(5, 15); // Gera um novo comprimento de cascata
      }
    }

    //delay(5); // Controle da velocidade da animação
  }

  tft.setTextColor(0xFFFF);//branco
  tft.setRotation(1);
  telaInicial();
//===================================== TELA ========================================= 


//===================================== CONTROLE ===================================== 
  tempoInicial = millis();
  pinMode(rele, OUTPUT);

  //Inicialização dos sensores
  sensors.begin();
  sensors.setResolution(12);
  n = sensors.getDeviceCount();
//===================================== CONTROLE ===================================== 


}


void loop() 
{ 

//===================================== TELA =========================================
  bool toque = obterToque();
  TeclaPressionada(toque);
  TeclaPressionada_2(toque);
  //DELAY DE 50 ms
          unsigned long currentMillis_2 = millis(); // Obtém o tempo atual
           // Executa a tarefa durante 3 segundos
          while (millis() - currentMillis_2 < (interval - 2950)) //calcula quanto tempo ja se passou desde currentMillis_1
          {
           bool toque = obterToque();
           TeclaPressionada_2(toque);
          }
//===================================== TELA =========================================

  controle();

}

//===================================== TELA =========================================
void telaInicial() {

  if(flag_tela == 0) {

  senha = "";
  tft.fillScreen(PRETO);

  String texto = "Temperatura de setpoint:";
 
  tft.setTextSize(2);
  tft.setCursor(20, 10);
  tft.println(texto);

  botoes[0].initButton(&tft,  60, 100, 30, 40, BRANCO, VERDE, PRETO, "1", 2);
  botoes[1].initButton(&tft, 110, 100, 30, 40, BRANCO, VERDE, PRETO, "2", 2);
  botoes[2].initButton(&tft, 160, 100, 30, 40, BRANCO, VERDE, PRETO, "3", 2);
  botoes[3].initButton(&tft, 210, 100, 30, 40, BRANCO, VERDE, PRETO, "4", 2);
  botoes[4].initButton(&tft, 260, 100, 30, 40, BRANCO, VERDE, PRETO, "5", 2);
  botoes[5].initButton(&tft, 60,  150, 30, 40, BRANCO, VERDE, PRETO, "6", 2);
  botoes[6].initButton(&tft, 110, 150, 30, 40, BRANCO, VERDE, PRETO, "7", 2);
  botoes[7].initButton(&tft, 160, 150, 30, 40, BRANCO, VERDE, PRETO, "8", 2);
  botoes[8].initButton(&tft, 210, 150, 30, 40, BRANCO, VERDE, PRETO, "9", 2);
  botoes[9].initButton(&tft, 260, 150, 30, 40, BRANCO, VERDE, PRETO, "0", 2);
  botoes[10].initButton(&tft, 60, 200, 30, 40, BRANCO, VERDE, PRETO, ".", 2); 
  botoes[11].initButton(&tft, 160, 200, 80, 40, BRANCO, VERDE, PRETO, "OK", 2);
  botoes[12].initButton(&tft, 260, 200, 80, 40, BRANCO, VERDE, PRETO, "<", 2);

  
  for (int i = 0; i < 13; i++) {
    botoes[i].drawButton(false);
  }
  escreveSenha();
  }
}

void tela_2() {

  flag_tela = 1;

  tft.fillScreen(PRETO);
  desativarBotoes(0, 12);


  setpoint = senha.toFloat(); //substitui por temp_plot
  tft.setTextColor(BRANCO); 
  tft.setCursor(10,45);
  tft.setTextSize(2); 
  tft.print("Setpoint:");
  tft.setCursor(125,45);
  tft.print(setpoint);
 

  botoes_2[0].initButton(&tft, 260,  100, 110, 40, BRANCO, VERDE, PRETO, "Parar/Inic", 2);
  botoes_2[1].initButton(&tft, 260, 150, 110, 40, BRANCO, VERDE, PRETO, "Gravar", 2);
  botoes_2[2].initButton(&tft, 260, 200, 110, 40, BRANCO, VERDE, PRETO, "Reiniciar", 2);

         
  

  for (int i = 0; i < 3; i++) {
    botoes_2[i].drawButton(false);
  }

}


void TeclaPressionada(bool teclaTocada) {
  if(flag_tela == 0){

  for (int i = 0; i < 13; i++) {
    botoes[i].press(teclaTocada && botoes[i].contains(posX, posY));
  }
  checkPressedButton();
  }
}


void TeclaPressionada_2(bool teclaTocada) {
  for (int i = 0; i < 3; i++) {
    botoes_2[i].press(teclaTocada && botoes_2[i].contains(posX, posY));
  }
  checkPressedButton_2();
}


void checkPressedButton() {
  if(flag_tela == 0){

  for (int i = 0; i < 13; i++) {
    if (botoes[i].justPressed()) {
      checkPressed(i);
    }
  }
  }
}

void checkPressedButton_2() {
  for (int i = 0; i < 3; i++) {
    if (botoes_2[i].justPressed()) {
      checkPressed_2(i);
    }
  }
}

void esperaSoltar() {
  while (obterToque()) {
    //DELAY DE 50 ms
          unsigned long currentMillis_2 = millis(); // Obtém o tempo atual
           // Executa a tarefa durante 3 segundos
          while (millis() - currentMillis_2 < (interval - 2950)) //calcula quanto tempo ja se passou desde currentMillis_1
          {
           bool toque = obterToque();
           TeclaPressionada_2(toque);
          }
  }
}


void escreveSenha() {
  
  if(flag_tela == 0){

  tft.fillRect(70, 30, 190, 30, BRANCO);
  tft.setCursor(74, 35); 
  tft.setTextColor(PRETO); 
  tft.setTextSize(3);
  tft.print(senha + "_"); 
  }
}


void apagarUltimocaractere() {
  if(flag_tela == 0){
  if (senha.length() > 0) {
    senha.remove(senha.length() - 1);
  }
  }
}


void checkPressed(int button) {
  
  if(flag_tela == 0)
  {

  botoes[button].drawButton(true);
  esperaSoltar();
  botoes[button].drawButton();

  if (senha.length() < TAMANHOMAXSENHA || button == 11 || button == 12) {
    switch (button) {
      case 0: senha += "1"; break;
      case 1: senha += "2"; break;
      case 2: senha += "3"; break;
      case 3: senha += "4"; break;
      case 4: senha += "5"; break;
      case 5: senha += "6"; break;
      case 6: senha += "7"; break;
      case 7: senha += "8"; break;
      case 8: senha += "9"; break;
      case 9: senha += "0"; break;
      case 10: senha += "."; break; // Adicionado ponto
      case 11: tela_2(); break;
      case 12: apagarUltimocaractere(); break;
    }
    escreveSenha();
  }
}
}

void checkPressed_2(int button) {
  botoes_2[button].drawButton(true);
  esperaSoltar();
  botoes_2[button].drawButton();

  switch (button) {
    case 0: 

      if(flag_parado == 0)
      {
      tft.setTextColor(BRANCO); 
      tft.setCursor(15, 10); 
      tft.setTextSize(2); 
      tft.print("|Parado|");
      flag_parado = 1;
      flag_parar = 1;
      digitalWrite(rele, LOW);
      }
      else
      {
      tft.setTextColor(PRETO); 
      tft.setCursor(15, 10); 
      tft.setTextSize(2); 
      tft.print("|Parado|");
      flag_parado = 0;
      flag_parar = 0;
      digitalWrite(rele, HIGH);
      }
      break; // Parar

    case 1: 
      
      if(flag_gravando == 0)
      {
      tft.setTextColor(BRANCO); 
      tft.setCursor(120, 10); 
      tft.setTextSize(2); 
      tft.print("|Gravando|"); 
      flag_gravando = 1;
      criar_arquivo_csv();
      salvar_leituras();
      }
      else
      {
      tft.setTextColor(PRETO); 
      tft.setCursor(120, 10); 
      tft.setTextSize(2); 
      tft.print("|Gravando|"); 
      flag_gravando = 0;
      }
      break; // Gravar


    case 2: 

      tft.setTextColor(BRANCO); 
      tft.setCursor(40, 100); 
      tft.setTextSize(3); 
      tft.fillScreen(PRETO);
      tft.print("Reiniciando..."); 
      delay(2000);
      //Reiniciar Arduino
      asm volatile ("jmp 0");
      break; // Reiniciar
  }
}


void desativarBotoes(int inicio, int fim) {

    for (int i = inicio; i <= fim; i++) 
    {
        botoes[i].initButton(&tft, -1, -1, 0, 0, BRANCO, PRETO, PRETO, "", 1); // Desativa o botão
    }
}

//===================================== TELA =========================================


//===================================== CONTROLE ===================================== 
void controle()
{
  unsigned long tempoAtual = millis();
  
unsigned long tempoDecorrido = tempoAtual - tempoInicial;
unsigned long minutosDecorridos = tempoDecorrido / 1000;

  
  //flag3 = 1; //Abilitar caso queira avaliar apenas o regime permanente
  
  //Solicita a entrada para o setpoint
  if(flag1 == 0)
  {
    // Lê a entrada como string
    //String input = Serial.readString();
    //setpoint = input.toFloat();  // Converte para float

    if(setpoint > 0.00)
    {
      flag1 ++;
    }
  }

    if (Serial.available() != 1 && flag1 == 1)
    {
      //Identificação dos sensores
      for (byte i = 0; i < n; i++) 
      {
        if (!sensors.getAddress(ds18, i)) 
        {
          //Serial.print("Falha lendo sensor "); 
          //Serial.println(i);
        }
      }   

          
          //ADQUIRIR TEMPERATURAS A
          sensors.requestTemperatures();
          t_amb = (sensors.getTempCByIndex(1) + fator_cal_sensor_2);
          //Serial.println(t_amb);

          if(t_amb < 27.00)
          {
              //CALCULAR FATOR TRANSIENTE DE ACORDO COM O SETPOINT ESCOLHIDO
              if(setpoint > 37.00)
              {
                fator_tr = (((setpoint*5.00)/40.00));
              }
              if(setpoint < 37.00)
              {
                fator_tr = (((setpoint*5.00)/40.00) + 2.00);
              }

              trset = (setpoint - fator_tr); //Limiar inicial
          }

          if(t_amb > 27.00)
          {
              //CALCULAR FATOR TRANSIENTE DE ACORDO COM O SETPOINT ESCOLHIDO
              if((setpoint > 37.00) && (setpoint <= 50))
              {
                fator_tr = (((setpoint*5.00)/40.00));  //=2.25 para 40 
                fator_tamb = 0.25;
                fator_tamb2 = 2000;
              }
              else
              {
                fator_tr = (((setpoint*3.00)/40.00));  //= 4,275 para 57  //fator era 3,2 e multiplicava por 2.25
                fator_tamb = 0.05;
                fator_tamb2 = 0;
              }
              
              if(setpoint < 37.00)
              {
                fator_tr = (((setpoint*2.25)/40.00) + 2.00);
              }
              
              trset = (setpoint - fator_tr); //Limiar para desligar relé
              //Serial.println(fator_tamb);
          }


          //MÉDIA MÓVEL A
          for(int i = 0; i < 8; i++)
          {
            sensors.requestTemperatures();
            sensor_1a = ((1.0118*sensors.getTempCByIndex(0)) + 0.0055); //CALIBRAÇÃO
            mediamovel_a = media_movel_a(sensor_1a); //REALIZA A MÉDIA MÓVEL 

            bool toque = obterToque();
            TeclaPressionada_2(toque);
          }

          temp_plot = mediamovel_a; //ADQUIRE A MÉDIA MÓVEL COM OS 4 CARACTERES 

          unsigned long segundos2 = millis()/1000;
          Serial.print(setpoint); Serial.print(", "); Serial.print(segundos2); Serial.print(", "); Serial.print(temp_plot); Serial.print(", "); Serial.println(t_amb);
          segundos2 = millis()/1000;

          //SALVA LEITURAS NO CARTÃO MICROSD CASO TOQUE NO BOTÃO [SALVAR]
          if(flag_parado == 0)
          {
            salvar_leituras();
          }
          

          ///////////////////PRINT//////////////////
          tft.setTextColor(PRETO); tft.setTextColor(BRANCO); tft.setCursor(10,90); tft.setTextSize(2); tft.print("Amb.:"); tft.setTextColor(PRETO); tft.setCursor(80,90);
          tft.print(t_amb_preto); tft.setTextColor(BRANCO); tft.setCursor(80,90); tft.print(t_amb);
          
          //CASO A TEMPERATURA ESTEJA CONTROLADA, PRINTAR DE COR VERDE
          if(flag_verde < 1)
          {
          tft.setTextSize(6); tft.setTextColor(PRETO); tft.setCursor(10,150); tft.print(temp_plot_preto); tft.setTextColor(BRANCO); tft.setCursor(10,150); tft.print(temp_plot);
          }
          else
          {
          tft.setTextSize(6); tft.setTextColor(PRETO); tft.setCursor(10,150); tft.print(temp_plot_preto); tft.setTextColor(VERDE); tft.setCursor(10,150); tft.print(temp_plot);
          }

          t_amb_preto = t_amb;
          temp_plot_preto = temp_plot;
    

          
          //DELAY DE 3s
          unsigned long currentMillis_1 = millis(); // Obtém o tempo atual
           // Executa a tarefa durante 3 segundos
          while (millis() - currentMillis_1 < (interval - fator_tamb2)) //calcula quanto tempo ja se passou desde currentMillis_1
          {
           bool toque = obterToque();
           TeclaPressionada_2(toque);
          }


          //ADQUIRIR TEMPERATURAS B
         //MÉDIA MÓVEL B
          for(int i = 0; i < 8; i++)
          {
            sensors.requestTemperatures();
            sensor_1b = ((1.0118*sensors.getTempCByIndex(0)) + 0.0055);
            mediamovel_b = media_movel_b(sensor_1b);

            bool toque = obterToque();
            TeclaPressionada_2(toque);

          }

          temp_plot = mediamovel_b;

          segundos2 = millis()/1000;
          Serial.print(setpoint); Serial.print(", "); Serial.print(segundos2); Serial.print(", "); Serial.print(temp_plot); Serial.print(", "); Serial.println(t_amb);


          if(flag_parado == 0)
          {
            salvar_leituras();
          }
              


          ///////////////////PRINT//////////////////
          tft.setTextColor(PRETO); tft.setTextColor(BRANCO); tft.setCursor(10,90); tft.setTextSize(2);  tft.print("Amb.:"); tft.setTextColor(PRETO); tft.setCursor(80,90); 
          tft.print(t_amb_preto); tft.setTextColor(BRANCO);  tft.setCursor(80,90); tft.print(t_amb);
          
          
          if(flag_verde < 1)
          {
          tft.setTextSize(6); tft.setTextColor(PRETO); tft.setCursor(10,150); tft.print(temp_plot_preto); tft.setTextColor(BRANCO);  tft.setCursor(10,150); tft.print(temp_plot);
          }
          else
          {
          tft.setTextSize(6); tft.setTextColor(PRETO); tft.setCursor(10,150); tft.print(temp_plot_preto); tft.setTextColor(VERDE); tft.setCursor(10,150); tft.print(temp_plot);
          }
          
          t_amb_preto = t_amb;
          temp_plot_preto = temp_plot;
          ///////////////////PRINT//////////////////


          
        if(flag_parar == 0)
        {
          //RELE [ON] TRANSIENTE
          if (temp_plot < trset)
          {
            digitalWrite(rele, HIGH);
            flag2 = 1;
            //Serial.println("Rele ON Transiente");
          }
        }

          //RELE [OFF] TRANSIENT
          if((temp_plot > trset) && flag2 == 1)
          {
            digitalWrite(rele, LOW);
           // Serial.println("Rele OFF Transiente");
            
            flag2 = 0;
          }

          //Condição para estabilidade do controle
          if(temp_plot >= setpoint)
          {
            flag3 = 1;
          }

          if((temp_plot < setpoint) && (flag3 > 0))
          {
            flag4 = 1;
          }

          if((temp_plot > setpoint) && (flag4 > 0))
          {
            flag5 ++;
          }

          if((temp_plot < setpoint) && (flag5 > 0))
          {
            flag_verde ++;
          }

          

  
        if(flag_parar == 0)
        {
          //RELE [ON] PERMANENTE
          if((temp_plot > trset) && (temp_plot < (setpoint - fator_tamb)) && (flag3 > 0) && ((mediamovel_b - mediamovel_a) < 0.00)) 
          {
            digitalWrite(rele, HIGH);
            //Serial.println("Rele ON Permanente");
          }
        }

          //RELE [OFF] PERMANENTE
          if((temp_plot > trset) && (flag3 > 0) && ((mediamovel_b - mediamovel_a) >= 0.00))
          {
            digitalWrite(rele, LOW);
            //Serial.println("Rele OFF Permanente");
          }


        //DELAY DE 3s
          unsigned long currentMillis_2 = millis(); // Obtém o tempo atual
           // Executa a tarefa durante 3 segundos
          while (millis() - currentMillis_2 < (interval - fator_tamb2)) //calcula quanto tempo ja se passou desde currentMillis_1
          {
           bool toque = obterToque();
           TeclaPressionada_2(toque);
          }
    }
}






float media_movel_a(float sensor_1ab) 
{
  vetor_media_movel_a[k] = sensor_1ab; // Preenche vetor na posição atual
  mmovel_a = 0;

  bool toque = obterToque();
  TeclaPressionada_2(toque);

  // Soma todos os elementos do vetor
  for (int y = 0; y < (j + 1); y++) 
  { // Usa j+1 para incluir todos os valores válidos
    mmovel_a += vetor_media_movel_a[y];
    bool toque = obterToque();
    TeclaPressionada_2(toque);
  }
  
  // Calcula a média móvel
  mmovel_a = mmovel_a / (j + 1); 
  
  // Atualiza j até que o vetor esteja completo (máx 4 elementos)
  if (j < 7) 
  {
    j++;
  }

  // Índice circular para sobrescrever o vetor
  k++;
  if (k > 7) 
  {
    k = 0;
  }

  return mmovel_a;
}

float media_movel_b(float sensor_1ab) 
{
  vetor_media_movel_b[m] = sensor_1ab; // Preenche vetor na posição atual
  mmovel_b = 0;

  bool toque = obterToque();
  TeclaPressionada_2(toque);

  // Soma todos os elementos do vetor válidos até o momento
  for (int y = 0; y < (l + 1); y++) 
  {
    mmovel_b += vetor_media_movel_b[y];
    bool toque = obterToque();
    TeclaPressionada_2(toque);
  }

  // Calcula a média móvel
  mmovel_b = mmovel_b / (l + 1);
  
  // Atualiza l até que o vetor esteja completo (máx 8 elementos)
  if (l < 7) 
  {
    l++;
  }

  // Índice circular para sobrescrever o vetor
  m++;
  if (m > 7) 
  {
    m = 0;
  }

  return mmovel_b;
}
//===================================== CONTROLE ===================================== 





//=========================================== CARTAO ==================================
void criar_arquivo_csv()
{
    //Serial.println("Inicializando o cartão SD...");

  if (!SD.begin(chipSelect)) {
    //Serial.println("Falha na inicialização do cartão SD!");
    return;
  }
  //Serial.println("Cartão SD inicializado com sucesso!");

  // Cria ou abre o arquivo CSV para adicionar cabeçalhos
  File dataFile = SD.open("dados.csv", FILE_WRITE);

  if (dataFile) {
    dataFile.println("Data, Hora, Segundos Decorridos,Temperatura do Bequer,Temperatura Ambiente, Temperatura de Setpoint"); // Cabeçalhos do CSV
    dataFile.close();
    //Serial.println("Arquivo CSV configurado.");
  } else {
    //Serial.println("Erro ao criar o arquivo CSV.");
  }
}

void salvar_leituras()
{
  DateTime now = rtc.now();
  // Simulação de leitura de dados (substitua pelos seus sensores)
  unsigned long segundos = millis() / 1000; // Simulando tempo em segundos
 

  // Formata os dados como CSV
String dataString = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()) + ", " +
                    String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + ", " +
                    String(segundos) + ", " + String(temp_plot) + ", " + String(t_amb) + ", " + String(setpoint);

  // Abre o arquivo para adicionar os dados
  File dataFile = SD.open("dados.csv", FILE_WRITE);

  if (dataFile) {
    dataFile.println(dataString); // Grava a linha no arquivo
    dataFile.close();
    //Serial.println("Dados gravados: " + dataString);
  } else {
    //Serial.println("Erro ao gravar no arquivo.");
  }
}
//=========================================== CARTAO ==================================



//=========================================== RTC ==================================

void real_time_clock()
{
    // Obtém a data e hora atual
  DateTime now = rtc.now();

  // Exibe a data no formato DD/MM/AAAA
  /*
  if (now.day() < 10) Serial.print("0"); // Adiciona zero se o dia for menor que 10
  Serial.print(now.day());
  Serial.print('/');

  if (now.month() < 10) Serial.print("0"); // Adiciona zero se o mês for menor que 10
  Serial.print(now.month());
  Serial.print('/');

  Serial.print(now.year());

  // Exibe a hora no formato HH:MM:SS
 
  if (now.hour() < 10) Serial.print("0"); // Adiciona zero se a hora for menor que 10
  Serial.print(now.hour());
  Serial.print(':');

  if (now.minute() < 10) Serial.print("0"); // Adiciona zero se os minutos forem menores que 10
  Serial.print(now.minute());
  Serial.print(':');

  if (now.second() < 10) Serial.print("0"); // Adiciona zero se os segundos forem menores que 10
  Serial.println(now.second());
  */
}
//=========================================== RTC ==================================
