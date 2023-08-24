// Declaração das variáveis para a base do transistor e inclusão da biblioteca do NTC
#define base_transistor 3
#include <Thermistor.h>

Thermistor temp(5);     // Variável do tipo Thermistor, indicando o pino no qual está conectado o sensor

// Declaração das variáveis para controle da malha
float y = 0.0;          // Temperatura atual
float setpoint = 0.0;   // Valor do setpoint
float u = 0.0;          // Sinal de controle em PWM
float e = 0.0;          // Sinal de erro
float e0 = 0.0;         // Erro calculado no momento anterior
float Sume = 0.0;       // Integral do erro
float Vare = 0.0;       // Derivada do erro
float C = 15.0;         // Constante de agressividade
float raw = 0.0;  

// Ganhos calculados a partir do método adotado no projeto
float K[3] = {-15.0/C, -1.0/C, -0.1/C};         // Respectivamente, ganhos Kp, Ki e Kd
 
void setup()
{
  Serial.begin(9600);                 // Configuração de transmissão de bits do Arduino UNO

  // Os dados são enviados diretamente para uma planilha do Excel, logo esta linha é para limpar as colunas no software
  // A linha seguinte é para criar o cabeçalho da planilha
  Serial.println("CLEARSHEET");       
  Serial.println("LABEL,Time,Timer,SetPoint [C], Temperatura [C], Sinal de Controle");
  
  pinMode(base_transistor,OUTPUT);    //Define pino como saida para chavear o transistor
}
 
void loop()
{
  // Printa na planilha a data atual, o horário e o instante de amostragem
  Serial.print((String)"DATA,TIME,TIMER,");

  setpoint = 40.0;         
  /*if (millis()>100000 && millis()<=200000){
    setpoint = 35.0;           // Setpoint definido para 40ºC
  }else if(millis()>200000){
    setpoint = 40.0;           // Setpoint definido para 70ºC
  }*/

  y = temp.getTemp();          // Temperatura atual resgatada e armazenada em y 
  e = setpoint-y;              // Sinal de erro
  Sume = e+Sume;               // Resultado da integral do sinal de erro
  Vare = e-e0;                 // Resultado da derivada do sinal de erro

  e0 = e;                                     // Registra o valor do erro para se usado na medição posterior
  u = ((K[0])*e + (K[1])*Sume + (K[2])*Vare);   // Sinal de controle é gerado a partir da lei de controle
  raw = u;

  // A ideia do IF é justamente não saturar o atuador, sendo que os limites são 0 e 255 para gerar o ciclo de trabalho do sinal que chaveia o transistor em série com o cooler (motor DC)
  if(u>255.0){
    u = 255.0; 
  }else if(u<0.0){
    u = 0.0;
  }
  
  analogWrite(base_transistor,u);   // o pino PWM do Arduíno gera o sinal para chavear o transistor

  // Print dos valores do setpoint, temperatura e sinal de controle na planilha do excel
  Serial.println((String)"" + setpoint + "," + y + ","+ u +",AUTOSCROLL_20");
  delay(10);
}
