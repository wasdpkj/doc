#include <Wire.h>
#include <EtherCard.h>
#define NUM 4

#define PIN_enc_CS 8

/*-------net------*/
static byte mymac[] =
{
  0x2C, 0x81, 0x58, 0x3C, 0xC5, 0x60
};									//MAC
static byte myip[] =
{
  192, 168, 169, 121
};									//IP
static byte gwip[] =
{
  192, 168, 169, 1
};									//Õ¯πÿ
static byte dnsip[] =
{
  192, 168, 169, 1
};

byte Ethernet::buffer[900];


#define LCD_INTERVAL                500
unsigned long LCD_TIMER=millis();

//===========VOL
boolean C[NUM];
//-------------------

void setup()
{
  Serial.begin(9600);
  
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);

  //NET------------------------------------------------------
  ether.begin(sizeof Ethernet::buffer, mymac, PIN_enc_CS);

  ether.staticSetup(myip, gwip, dnsip);

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
  delay(1000);
}


void loop()
{
  digitalWrite(3,C[0]);
  digitalWrite(4,C[1]);
  digitalWrite(5,C[2]);
  digitalWrite(6,C[3]);

  ether.packetLoop(ether.packetReceive());
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);

  if(pos)
  {
    unsigned int light;      //∑Á…»µµŒª
    if(strstr((char *)Ethernet::buffer + pos, "POST") != 0)
      sscanf((char *)strstr((char *)Ethernet::buffer + pos, "lightstatus"), "lightstatus=%d", &light);

    switch(light)
    {
    case 10:
      C[0] = false;
      break;
    case 11:
      C[0] = true;
      break;
    case 20:
      C[1] = false;
      break;
    case 21:
      C[1] = true;
      break;
    case 30:
      C[2] = false;
      break;
    case 31:
      C[2] = true;
      break;
    case 40:
      C[3] = false;
      break;
    case 41:
      C[3] = true;
      break;
    }

    BufferFiller bfill = ether.tcpOffset();
    bfill.emit_p(PSTR("HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Pragma: no-cache\r\n\r\n"
      "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"/>"
      "<title></title><style type=\"text/css\"> body {background-color:black;} .STYLE0 {background-color:#666666;}.STYLE1 {background-color:#0000FF;} </style></head>"
      "<body><table width=\"500\" border=\"0\"><tr>"
      "<td width=\"102\" height=\"80\" class=\"STYLE$D\"></td><td width=\"102\" height=\"80\" class=\"STYLE$D\"></td>"
      "<td width=\"102\" height=\"80\" class=\"STYLE$D\"></td><td width=\"102\" height=\"80\" class=\"STYLE$D\"></td>"
      "</tr></table>"
      "</body></html>"), C[0], C[1], C[2], C[3]);
    ether.httpServerReply(bfill.position());
  }
}