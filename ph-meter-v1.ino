#include <Arduino.h>
#include <U8g2lib.h> // Biblioteca do display OLED
#include <SPI.h>
#include <Wire.h>
#include <FloatToString.h> // Biblioteca para converter float para string
#include <dht11.h> //Biblioteca do sensor DHT11

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); //Inicializa variável u8g2 para manipular display OLED.

float calibration = 0.25; //alterar o valor para calibrar quando não conseguir no potenciômetro do sensor pH.
const int analogInPin = A0; //pino para leitura do pH.
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[100],temp; //matriz buf com 100 elementos para armazenar os valores de tensão lidos.
char buffer[25]; // buffer utilizado para converter float para string
dht11 sensor; //objeto do tipo dht11

void setup() {
    Serial.begin(9600);
    Serial.println("CLEARDATA");  //CLEARDATA para utilização no PLX-DAQ
    Serial.println("LABEL,Hora,Temp,pH"); //Cabeçalho dos dados para utilizar no PLX-DAQ
    u8g2.begin();  //inicializa display OLED

}

void loop() {
    
    int chk = sensor.read(2); // sensor faz a leitura no pino D2
       
    //Loop para fazer a leitura de 100 valores de tensão no pino A0
    for(int i=0;i<100;i++)
    {
        buf[i]=analogRead(analogInPin);
        delay(10);
    }
    
    //Ordena os valores de tensão
    for(int i=0;i<100;i++)
    {
        for(int j=i+1;j<100;j++)
        {
            if(buf[i]>buf[j])
            {
                temp=buf[i];
                buf[i]=buf[j];
                buf[j]=temp;
            }
        }
    }
   
    avgValue=0;

    //Considera 96 valores e calcula a média da amostra
    for(int i=2;i<97;i++)
          avgValue+=buf[i];


    //Calcula o valor médio de tensão do sensor pH  
    float pHVol=(float)avgValue*4.94/1024/96;

    //Converte para string o valor float do phVol e deixa com somente dois dígitos
    String spHVol = floatToString(buffer, pHVol, 2);
    
    //Converte a string spHVol para float novamente mas agora somente com dois dígitos
    float fpHVol = spHVol.toFloat();
   
    //De posse do valor médio de tensão calcula-se o valor de pH acrescentando a calibração
    float phValue = -5.70 * fpHVol + 21.34 + calibration;

    // Se o valor de pH for menor que zero o display mostrará zero
    if (phValue < 0){
        phValue = 0; 
    }



    // A série de prints abaixo vai escrever na Serial os valores separados por vírgula do pH e da temperatura para utilização no PLX-DAQ
    Serial.print("DATA,TIME,");
    Serial.print((float)sensor.temperature,0);
    Serial.print(",");
    Serial.println(phValue);
   
    // Mostra os valores no display OLED
    u8g2.clearBuffer();         // apaga o buffer do display (apaga a tela)
    u8g2.setFont(u8g2_font_logisoso24_tr);  // fonte exibida no display em https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.setCursor(6,30); // posição 6,30 do display
    u8g2.print("pH:"); // escreve pH: no display
    u8g2.setCursor(60,30); // posição 60,30 do display
    u8g2.print(phValue,2); // escreve o valor de pH obtido com dois dígitos
    u8g2.sendBuffer(); // envia o buffer para o display
    delay(2000); //aguarda dois segundos com o valor de pH
    u8g2.clearBuffer();         // apaga o buffer do display (apaga a tela)
    u8g2.setFont(u8g2_font_logisoso18_tr);  // fonte exibida no display em https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.setCursor(6,30); // posição 6,30 do display
    u8g2.print("T:"); // escreve T: no display
    u8g2.setCursor(50,30); // posição 50,30 do display
    u8g2.print((float)sensor.temperature,0); // escreve o valor da temperatura obtida pelo DHT11
    u8g2.setCursor(75,30); // posição 75,30 do display
    u8g2.print("*C"); //  escreve * C
    u8g2.sendBuffer(); //  envia o buffer para o display
   
     delay(1000); //aguarda 1 segundo com o valor de temperatura no display
}
