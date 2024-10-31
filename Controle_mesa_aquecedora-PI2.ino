#include <OneWire.h>
#include <DallasTemperature.h>

#define rele 24

//Instances
OneWire oneWire(26);
DallasTemperature sensors(&oneWire);

//Var
DeviceAddress ds18; // Sensor
byte n; //Quantidade de sensores
float setpoint = 0.0;
float sensor1a = 0.0;
float sensor1b = 0.0;
float sensor2 = 0.0;
float trset = 0.0;


void setup(void) {

  Serial.begin(9600);
  pinMode(rele, OUTPUT);

  //Inicialização dos sensores
  sensors.begin();
  sensors.setResolution(12);
  n = sensors.getDeviceCount();
  Serial.print("DS18B20 identificados: ");
  Serial.println(n);

  //Entrada para o setpoint
  Serial.println(" Digite a temperatura de setpoint: ");

  setpoint = Serial.parseFloat();
  trset = (setpoint - 4.00);
  
  if (Serial.available() != 0)
  {
    Serial.print("Setpoint: ");
    Serial.print(setpoint);
  }
}


void loop() 
{
  sensors.requestTemperatures();
  if (Serial.available() != 0)
  {
  
    //Identificação dos sensores
    for (byte i = 0; i < n; i++) 
    {
      if (!sensors.getAddress(ds18, i)) 
      {
        Serial.print("Falha lendo sensor "); 
        Serial.println(i);
      }
      else
        {//Prints
          Serial.println("----------------------------");
          sensor1a = sensors.getTempCByIndex(0);
          sensor2 = sensors.getTempCByIndex(1);
          Serial.print("Sensor 1: ");
          Serial.println(sensor1a);
          
          Serial.print("Sensor 2: ");
          Serial.println(sensor2);
        } 
        
        //Ligar rele - transient
        if (sensor1a < trset)
        {
          digitalWrite(rele, HIGH);
        }

        delay(1000);

        sensor1b = sensors.getTempCByIndex(0);

        //Ligar rele na descida da curva de temperatura
        if ((sensor1a > setpoint) && (sensor1b - sensor1a) < 0.00  )
        {
          digitalWrite(rele, HIGH);
        }

        //Desligar rele na subida da temperatura
        if ((sensor1a > setpoint) && (sensor1b - sensor1a) > 0.00 )
        {
          digitalWrite(rele, LOW);
        }
      }
  }
}
