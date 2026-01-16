#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>

#define DIR_CLIENT 0x7E8 // Dirección del Cliente (Respuesta del servidor/ECU)
#define DIR_SERVER_LISTEN 0x7DF // Dirección de escucha del servidor (Petición del cliente)

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);

unsigned char valor_fuel[] = {90, 111, 96, 103, 100, 118, 127, 120, 108, 99};
unsigned char valor_oxigen1[] = {20, 40, 60, 80, 100, 110, 120, 30, 50, 70 };
unsigned char valor_oxigen2[] = {116,118,120,122,124,126,127,121,123,117};

// Buffer de respuesta CAN (8 bytes max)
unsigned char stmp[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 

char indice_fuel = 0;
char indice_oxigen1 = 0;
char indice_oxigen2 = 0;

unsigned char Flag_Recv = 0; // Flag de interrupciones
unsigned char len_Recv = 0; 
unsigned char buf_Recv[8]; // Búfer para la petición entrante
char canLen;

unsigned char obdLen = 0; 
unsigned char obdService = 0;
unsigned char obdPID = 0;


void setup()
{
    Serial.begin(115200);

START_INIT:
    if(CAN_OK == CAN.begin(CAN_500KBPS))
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail. Reintentando...");
        delay(100);
        goto START_INIT;
    }
     // Habilitar la interrupción por recepción de datos en el Pin D2
     attachInterrupt(0, MCP2515_ISR, FALLING); 
}

void MCP2515_ISR()
{
    Flag_Recv = 1;
}

void loop()
{
    if(Flag_Recv) // Comprobamos si hay una petición pendiente
    {
        Flag_Recv = 0;
        
        // Leer el mensaje CAN entrante
        // Pasamos &len_Recv y buf_Recv como punteros a la funcion
        CAN.readMsgBuf(&len_Recv, buf_Recv); 
        
        // Siempre que recibamos algo, lo procesamos. No necesitamos 'if (len_Recv > 0)' aquí dentro.

        // Procesado de datos OBD de la PETICIÓN ENTRANTE
        obdLen = buf_Recv[0]; 
        obdService = buf_Recv[1];
        obdPID = buf_Recv[2];

        // Usamos una variable local para decidir si responder o no a ESTA petición específica
        bool doResponse = true; 

        // Limpiamos el buffer de respuesta para esta trama específica
        for (int i=0;i<sizeof(stmp);i++){
          stmp[i] = 0; 
        }

        if (obdPID == 10){ 
          stmp[3] = valor_fuel[indice_fuel]; // Dato A (Byte 3 de la trama CAN)
          obdLen = 3; // La longitud de datos utiles que declaramos en el byte 0 de la respuesta
          indice_fuel++;
          if (indice_fuel > 9){ indice_fuel = 0; }
        } else if (obdPID == 20){ 
          stmp[3]= valor_oxigen1[indice_oxigen1]; // Dato A
          stmp[4] = valor_oxigen2[indice_oxigen2]; // Dato B
          obdLen = 4; // Longitud de datos utiles
          indice_oxigen1++;
          indice_oxigen2++;
          if (indice_oxigen1>9){
              indice_oxigen1=0;
              indice_oxigen2=0;
          }
        } else {
          // PID no reconocido, no respondemos
          Serial.print("Parámetro ");
          Serial.print(obdPID, DEC);
          Serial.println(" no contemplado en el servidor. No se envia respuesta.");
          doResponse = false; 
        }
        
        // Si decidimos responder, preparamos los bytes de cabecera estándar
        if (doResponse){
            stmp[0] = obdLen; // Primer byte: Longitud de datos útiles (3 o 4)
            stmp[1] = 0x41;   // Segundo byte: Servicio de respuesta (0x40 + 0x01)
            stmp[2] = obdPID; // Tercer byte: El PID que nos pidieron
            
            // Enviamos el mensaje completo de 8 bytes (rellenamos con 0s si es necesario)
            // Usamos DIR_CLIENT (0x7E8) para responder
            CAN.sendMsgBuf(DIR_CLIENT, 0, 8, stmp); 

            Serial.print("Respuesta enviada para PID ");
            Serial.println(obdPID, DEC);
        }
    }
  
  // ELIMINAMOS el delay(1000) de aquí para que el microcontrolador esté siempre listo para la próxima interrupción.
}
