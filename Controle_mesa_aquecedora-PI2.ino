#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

#define rele 24

//Instances
OneWire oneWire(26);
DallasTemperature sensors(&oneWire);

//Var
DeviceAddress ds18; // Sensor
byte n; //Quantidade de sensores
float fator_cal_sensor_1 = 0.30;
float fator_cal_sensor_2 = 0.93;
float setpoint = 0.0;
float sensor_1a = 0.0;
float sensor_1b = 0.0;
float vetor_media_movel_a [4]{0.00,0.00,0.00,0.00};
float vetor_media_movel_b [4]{0.00,0.00,0.00,0.00};
float mmovel_a = 0.00;
float mmovel_b = 0.00;
float mediamovel_a = 0.00;
float mediamovel_b = 0.00;
float t_amb = 0.0;
float trset = 0.0;
int flag1 = 0;
int flag2 = 0;
int flag3 = 0;
int flagprint = 0;
float temp_plot = 0.00;
unsigned long tempoInicial;
int l = 0; // Contador de elementos preenchidos
int m = 0; // Índice circular para inserir no vetor
float fator_tr = 0.00;

int j;
int k;


float media_movel_a(float sensor_1ab);
float media_movel_b(float sensor_1ab);

void setup(void) 
{
  Serial.begin(9600);
  tempoInicial = millis();
  pinMode(rele, OUTPUT);

  //Inicialização dos sensores
  sensors.begin();
  sensors.setResolution(12);
  n = sensors.getDeviceCount();
  //Serial.print("DS18B20 identificados: ");
  //Serial.println(n);
}


void loop() 
{ 

unsigned long tempoAtual = millis();
unsigned long tempoDecorrido = tempoAtual - tempoInicial;
unsigned long minutosDecorridos = tempoDecorrido / 1000;

  ////////////////////=-=-==-0-909-00973598273895y13y587'2////////////////////////-=-=-==-0-909-00973598273895y13y587'2
  
  //flag3 = 1; //Abilitar caso queira avaliar apenas o regime permanente
  
  //Solicita a entrada para o setpoint
  if(flag1 == 0)
  {
    //Serial.println("Digite o valor de setpoint e tecle ENTER:");
    /*
    // Aguarda até que um dado completo seja inserido
    while (Serial.available() == 0) 
    {
    
    }
    */
    // Lê a entrada como string
    String input = Serial.readString();
    setpoint = input.toFloat();  // Converte para float

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
          t_amb = (sensors.getTempCByIndex(1) + fator_cal_sensor_2);
        
          if(setpoint > 37.00)
          {
            fator_tr = (((setpoint*5.00)/40.00));
          }
          else
          {
            fator_tr = (((setpoint*5.00)/40.00) + 1.00);
          }
          
          
          trset = (setpoint - fator_tr);

          //MÉDIA MÓVEL A
          for(int i = 0; i < 4; i++)
          {
            sensors.requestTemperatures();
            sensor_1a = sensors.getTempCByIndex(0);
            mediamovel_a = media_movel_a(sensor_1a);
            delay(10);
          }
          
          temp_plot = (mediamovel_a + fator_cal_sensor_1);
          Serial.print(temp_plot);
          Serial.print(", ");
          Serial.print(minutosDecorridos);
          Serial.print(", ");
          Serial.println(t_amb);
          
          delay(3000);

          tempoAtual = millis();
          tempoDecorrido = tempoAtual - tempoInicial;
          minutosDecorridos = tempoDecorrido / 1000;

          //MÉDIA MÓVEL B
          for(int i = 0; i < 4; i++)
          {
            sensors.requestTemperatures();
            sensor_1b = sensors.getTempCByIndex(0);
            mediamovel_b = media_movel_b(sensor_1b);
            delay(10);
          }
 
          temp_plot = (mediamovel_b + fator_cal_sensor_1);
          Serial.print(temp_plot);
          Serial.print(", ");
          Serial.print(minutosDecorridos);
          Serial.print(", ");
          Serial.println(t_amb);
        
          
          

          //RELE [ON] TRANSIENT
          if (temp_plot < trset)
          {
            digitalWrite(rele, HIGH);
            flag2 = 1;
          
          }

          //RELE [OFF] TRANSIENT
          if((temp_plot > trset) && flag2 == 1)
          {
            digitalWrite(rele, LOW);
            
            flag2 = 0;
          }

          if(temp_plot >= setpoint)
          {
            flag3 = 1;
          }

          //RELE [ON] PERMANENTE
          if((temp_plot > trset) && (temp_plot <= setpoint) && (flag3 > 0) && ((mediamovel_b - mediamovel_a) < 0.00)) //ou 0.00?
          {
            digitalWrite(rele, HIGH);
          }

          //RELE [OFF] PERMANENTE
          if((temp_plot > trset) && (flag3 > 0) && (mediamovel_b - mediamovel_a) >= 0.00)
          {
            digitalWrite(rele, LOW);
          }

          /*
          if((temp_plot <= setpoint - 0.4) && (flag3 > 0))
          {
            digitalWrite(rele, HIGH);
          }
          */
          delay(3000);
    }
}


float media_movel_a(float sensor_1ab) 
{
  vetor_media_movel_a[k] = sensor_1ab; // Preenche vetor na posição atual
  mmovel_a = 0;

  // Soma todos os elementos do vetor
  for (int y = 0; y < (j + 1); y++) 
  { // Usa j+1 para incluir todos os valores válidos
    mmovel_a += vetor_media_movel_a[y];
  }
  
  // Calcula a média móvel
  mmovel_a = mmovel_a / (j + 1); 
  
  // Atualiza j até que o vetor esteja completo (máx 8 elementos)
  if (j < 3) 
  {
    j++;
  }

  // Índice circular para sobrescrever o vetor
  k++;
  if (k > 3) 
  {
    k = 0;
  }

  return mmovel_a;
}

float media_movel_b(float sensor_1ab) 
{
  vetor_media_movel_b[m] = sensor_1ab; // Preenche vetor na posição atual
  mmovel_b = 0;

  // Soma todos os elementos do vetor válidos até o momento
  for (int y = 0; y < (l + 1); y++) 
  {
    mmovel_b += vetor_media_movel_b[y];
  }

  // Calcula a média móvel
  mmovel_b = mmovel_b / (l + 1);
  
  // Atualiza l até que o vetor esteja completo (máx 8 elementos)
  if (l < 3) 
  {
    l++;
  }

  // Índice circular para sobrescrever o vetor
  m++;
  if (m > 3) 
  {
    m = 0;
  }

  return mmovel_b;
}

