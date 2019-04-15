#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <FloatToString.h>  //set to whatever is the location of floatToStrig
#include <dht11.h> 

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

float calibration = 0.14; //change this value to calibrate
const int analogInPin = A0; 
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[100],temp;
char buffer[25]; // just give it plenty to write out any values you want to test
dht11 sensor; //objeto do tipo dht11
 
void setup() {
 Serial.begin(9600);
 Serial.println("CLEARDATA");
 Serial.println("LABEL,Hora,Temp,pH");
 u8g2.begin();

}
 
void loop() {
    // Serial.print("Lendo sensor: ");
    int chk = sensor.read(2); // sensor faz a leitura no pino 2
    // Testa se a leitura foi bem sucedida 
    /*
    switch(chk) {
             case DHTLIB_OK:
                    Serial.println("OK");
                    break;
            case DHTLIB_ERROR_CHECKSUM:
                   Serial.println("Erro no checksum");
                   break;
            case DHTLIB_ERROR_TIMEOUT:
                   Serial.println("Tempo esgotado");
                   break;
            default:
                   Serial.println("Erro desconhecido");
      } 
    */ 
    // Se a leitura foi bem sucedida, mostra a umidade e temperatura na Serial
    // Serial.print("Umidade (%): ");
    // Serial.println((float)sensor.humidity, 2);
    //Serial.print("Temperatura (graus Celsius): ");
    // Serial.println((float)sensor.temperature, 2);
    
    
    for(int i=0;i<100;i++) 
    { 
        buf[i]=analogRead(analogInPin);
        delay(10);
//        Serial.println(buf[i]);
    }
    
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
    for(int i=2;i<97;i++)
          avgValue+=buf[i];
    float pHVol=(float)avgValue*4.90/1024/96;
    String spHVol = floatToString(buffer, pHVol, 2);
    float fpHVol = spHVol.toFloat();
    //Serial.print("Voltage = ");
    //Serial.println(fpHVol);
 
    float phValue = -5.70 * fpHVol + 21.34 + calibration;
    // Serial.print("pH = ");
    if (phValue < 0){
        phValue = 0;  
    }

    Serial.print("DATA,TIME,");
    Serial.print((float)sensor.temperature,0);
    Serial.print(",");
    Serial.println(phValue);
    
    //Serial.println(phValue);
    // Serial.println(" ");

    u8g2.clearBuffer();         // clear the internal memory
    u8g2.setFont(u8g2_font_logisoso24_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.setCursor(6,30); // write something to the internal memory
    u8g2.print("pH:"); // write something to the internal memory
    u8g2.setCursor(60,30); // write something to the internal memory
    u8g2.print(phValue,2); // write something to the internal memory
    u8g2.sendBuffer();
    delay(2000);
    u8g2.clearBuffer();         // clear the internal memory
    u8g2.setFont(u8g2_font_logisoso18_tr);  // choose a suitable font at https://github.com/olikraus/u8g2/wiki/fntlistall
    u8g2.setCursor(6,30); // write something to the internal memory
    u8g2.print("T:"); // write something to the internal memory
    u8g2.setCursor(50,30); // write something to the internal memory
    u8g2.print((float)sensor.temperature,0); // write something to the internal memory
    u8g2.setCursor(75,30); // write something to the internal memory
    u8g2.print("*C"); // write something to the internal memory
    u8g2.sendBuffer();
    
     delay(1000);

     
}
