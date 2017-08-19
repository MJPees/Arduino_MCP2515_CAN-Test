/*******************************************************************
simple CAN-Test for Sparkfun CAN-Shield

Written by Marc Pees. 
*******************************************************************/

#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>

tCAN tx_message;
tCAN rx_message;
unsigned long wait_time = 0;

void setup() {
  Serial.begin(115200);
  
  if(Canbus.init(CANSPEED_125))  //Initialise MCP2515 CAN controller
  {
    Serial.println("CAN Init at 125k baud");
    //set normal operation mode
    delay(500);
    mcp2515_bit_modify(CANCTRL, (1<<REQOP2)|(1<<REQOP1)|(1<<REQOP0), 0);
  }
  else Serial.println("Can't init CAN");

  //set tx message
  tx_message.id = 0x631; //formatted in HEX
  tx_message.header.rtr = 0;
  tx_message.header.length = 8; //formatted in DEC
  tx_message.data[0] = 0x40;
  tx_message.data[1] = 0x05;
  tx_message.data[2] = 0x30;
  tx_message.data[3] = 0xFF; //formatted in HEX
  tx_message.data[4] = 0x00;
  tx_message.data[5] = 0x40;
  tx_message.data[6] = 0x00;
  tx_message.data[7] = 0x00;
}

//********************************Main Loop*********************************//

void loop() 
{
  //send every second
  if(millis() > wait_time)
  {
    mcp2515_send_message(&tx_message);
    wait_time = millis() + 1000;
  }

  if (mcp2515_check_message()) 
  {
    if (mcp2515_get_message(&rx_message)) 
    {
      Serial.print("ID: ");
      Serial.print(rx_message.id,HEX);
      int len = rx_message.header.length;
      Serial.print(" LEN: ");
      Serial.print(len,DEC);
      Serial.print(" Data: ");
      for(int i=0;i < len;i++) 
      { 
        Serial.print(rx_message.data[i],HEX);
        if(i < len -1) Serial.print(" ");
        else Serial.println("");
      }
      //if message id=0x620 return data
      if(rx_message.id == 0x620)  //filter
      {
        rx_message.id = 0x621;
        mcp2515_send_message(&rx_message);
      }
    }
  }
}
