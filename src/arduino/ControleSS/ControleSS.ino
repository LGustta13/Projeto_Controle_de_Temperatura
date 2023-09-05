// Declaração das variáveis para a base do transistor e inclusão da biblioteca do NTC
#define base_transistor 3
#include <Thermistor.h>

Thermistor temp(5);     // Variável do tipo Thermistor, indicando o pino no qual está conectado o sensor

// Declaração das variáveis para controle da malha
float y = 0.0;          // Temperatura atual
float y0 = 0.0;         // Temperatura anterior
float y00 = 0.0;        // Temperatura antes da anterior
float setpoint = 0.0;   // Valor do setpoint
float u = 0.0;          // Sinal de controle em PWM
float e = 0.0;          // Sinal de erro
float xn = 0.0;         // Estado referente a ação integral
float x1 = 0.0;         // Estado 1
float x2 = 0.0;         // Estado 2
float x3 = 0.0;         // Estado 3
float C = 0.009;        // Ganho de ajuste do sinal de controle para ponderamento
float raw = 0.0;

// Ganhos calculados a partir do modelo estimado para os estados (método Acker)
float K[3] = {-3.9706, 11.8414, 0.0090};      // Ganho dos pólos da equação característica
float Ke = {-8.5872};                         // Ganho do integrador

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

  setpoint = 48.0;         
  if (millis()>100000 && millis()<=200000){
    setpoint = 35.0;           // Setpoint definido para 40ºC
  }else if(millis()>200000){
    setpoint = 40.0;           // Setpoint definido para 70ºC
  }

  y = temp.getTemp();          // Temperatura atual resgatada e armazenada em y 
  e = setpoint-y;              // Sinal de erro
  xn = e+xn;                   // Valor do estado relacionado ao integrador, no qual é a integral do erro

  y00 = y0;                    // y00 é a variável que armazena a temperatura da segunda leitura anterior a atual  
  y0 = x1;                     // y0 armazena a leitura de temperatura anterior a atual
  x1 = y;                      // x1 é o estado que representa e recebe a leitura da temperatura atual
  
  x2 = y-y0;                   // x2 é o estado que representa a variação das duas últimas leituras de temperatura realizadas
  x3 = (y-y0)-(y0-y00);        // x3 é o estado que representa a variação entre as duas últimas leituras com as duas leituras anteriores a atual ("variação da variação")

  u = C*(-((K[0])*x1 + (K[1])*x2 + (K[2])*x3) + (Ke)*xn);   // Sinal de controle é gerado a partir da lei de controle
  raw = u;

  // A ideia do IF é justamente não saturar o atuador, sendo que os limites são 0 e 255 para gerar o ciclo de trabalho do sinal que chaveia o transistor em série com o cooler (motor DC)
  if(u>255.0){
    u = 255.0; 
  }else if(u<0.0){
    u = 0.0;
  }
  
  analogWrite(base_transistor,u);   // o pino PWM do Arduíno gera o sinal para chavear o transistor

  // Print dos valores do setpoint, temperatura e sinal de controle na planilha do excel
  Serial.println((String)"" + setpoint + "," + y + ","+ raw +",AUTOSCROLL_20");
  delay(10);
}
