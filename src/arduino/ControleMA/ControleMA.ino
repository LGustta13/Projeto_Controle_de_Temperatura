#define analogpin A0
#define base_transistor 3
#include <Thermistor.h> //INCLUSÃO DA BIBLIOTECA
 
int DutyCycle = 0;
int valor_pot = 0;
float temperature = 0.0;
Thermistor temp(5); //VARIÁVEL DO TIPO THERMISTOR, INDICANDO O PINO ANALÓGICO (A2) EM QUE O TERMISTOR ESTÁ CONECTADO
 
void setup()
{
  Serial.begin(9600);
  Serial.println("CLEARSHEET"); // clears sheet starting at row 1
  Serial.println("LABEL,Time,Timer,Temp[C],DutyCycle");

  // set the names for the 3 checkboxes
  Serial.println("CUSTOMBOX1,LABEL,Parar em 15k amostras");
  Serial.println("CUSTOMBOX2,LABEL,Parar em 10 minutos");
  Serial.println("CUSTOMBOX3,LABEL,Salvar");
  
  pinMode(base_transistor,OUTPUT); //Define pino como saida para chavear o transistor
}
 
void loop()
{
  Serial.print((String)"DATA,TIME,TIMER,");
  
  valor_pot = analogRead(analogpin); 
  temperature = temp.getTemp(); 
  
  DutyCycle = map(valor_pot,0,1023,0,255); 
  analogWrite(base_transistor,DutyCycle); 

  Serial.println((String)"" + temperature + "," + DutyCycle + ",AUTOSCROLL_20");
}
