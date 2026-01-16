#include <mcp_can.h>
#include <mcp_can_dfs.h>

// demo: CAN-BUS Shield, receive data with interrupt mode
// when in interrupt mode, the data coming can't be too fast, must >20ms, or else you can use check mode
// loovee, 2014-6-13

#include <SPI.h>
#include "mcp_can.h"

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

// Definiciones

#define PID_1 10 // En nuestro codigo el PID_1 va a estar asociado al PID 10
#define PID_2 20 // En nuestro codigo el PID_2 va a estar asociado al PID 20
#define DIR_OBDII 0x7df // Dirección del Cliente

// Variables

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];
unsigned char fuel[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
unsigned char oxigen1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
unsigned char oxigen2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

int index_fuel=0, index1=0, index2=0;

unsigned char stmp[] = {0, 1, 0, 0, 0};

bool parametro=false;

unsigned char Flag_Recv = 0; // Es el flag de interrupciones asociado a la librería "mcp_can.h"
unsigned char len_Recv = 0; // Tamaño del paquete enviado
unsigned char buf_Recv[8]; // Almacena el valor que vamos a enviar mediante la comunicación
unsigned char obdLen = buf_Recv[0];
unsigned char obdService = buf_Recv[1];
unsigned char obdPID = buf_Recv[2];

// configuración de la comunicación
void setup()
{
  //se inicia la transmisión can bus a 500 kbps  
    Serial.begin(115200);

START_INIT:

    //se inicia la transmisión por serie (conexión USB): 115200 bits/s
    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
    
/*    // CONFIGURACIÓN DE MÁSCARAS Y FILTROS -------------------------
    // Solo aceptar mensajes con ID = 0xFF
    CAN.init_Mask(0, 0, 0x7FF);   // Máscara 0: todos los bits del ID importan
    CAN.init_Mask(1, 0, 0x7FF);   // Máscara 1

    CAN.init_Filt(0, 0, 0xFF);    // Filtro 0: solo ID 0xFF
    CAN.init_Filt(1, 0, 0xFF);    // Filtro 1
    CAN.init_Filt(2, 0, 0xFF);    // Filtro 2:
    CAN.init_Filt(3, 0, 0xFF);    // Filtro 3
    CAN.init_Filt(4, 0, 0xFF);    // Filtro 4:
    CAN.init_Filt(5, 0, 0xFF);    // Filtro 5
    // --------------------------------------------------------------
*/
    //Se habilita la interrupción por recepción de datos
    attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
}

//rutina de interrupción
void MCP2515_ISR()
{
    // Si llega un dato esta variable se pondrá a 1
    flagRecv = 1;
}

void loop()
{

  char canLen;
  for (int i=0;i<sizeof(stmp);i++){
    stmp[i] = 0;
  }
  if (parametro){ // Esta condición lo que hace es que cada vez que se ejecute el programa del cliente, este lea los datos de uno de los PIDs cada vez
    stmp[0] = 4;
    stmp[1] = 1;
    stmp[2] = PID_2;
    stmp[3]= oxigen1[index1];
    stmp[4]= oxigen2[index2];
    canLen = 8;
    if(index1 > 9)
    {
      index2++;
      index1=0;  
    }
    if(index2 > 9)
    {
      index2=0;  
    }
    index1++;
  } else {
    stmp[0] = 3;
    stmp[1] = 1;
    stmp[2] = PID_1;
    stmp[3]= fuel[index_fuel];
    stmp[4]= 0;
    canLen = 8;
    if(index_fuel > 9)
    {
      index_fuel=0;  
    } 
    index_fuel++;
  }
  parametro = !parametro;
  
  CAN.sendMsgBuf(DIR_OBDII, 0, canLen, stmp); 
  
    if(flagRecv) 
    {                                   // comprobamos si esta variable vale 1. Si vale 1 es que hemos recibido datos

        flagRecv = 0;                   // borramos esta variable o flag

        CAN.readMsgBuf(&len_Recv, buf_Recv); // read data,  len: data length, buf: data buf
    // Procesado de datos OBD
     obdLen = buf_Recv[0];
     obdService = buf_Recv[1];
     obdPID = buf_Recv[2];
     
    Serial.print(obdPID, DEC);
    Serial.print(";");
    if (obdPID==PID_2){
    Serial.print(buf_Recv[3], DEC);
    Serial.print(",");
    Serial.println(buf_Recv[4], DEC);
    }
    if (obdPID==PID_1){
       Serial.println(buf_Recv[3], DEC);
    }
    }
  delay(1000);
    }


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/

