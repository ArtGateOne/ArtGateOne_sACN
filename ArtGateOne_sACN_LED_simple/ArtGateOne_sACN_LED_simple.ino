//ArtGateOne sACN - WS_LED simple node - ONE UNIVERSE - 170 WS28xx LED RGB

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 6       //defined led pin
#define LED_COUNT 170  // How many NeoPixels are attached to the Arduino 1-170
#define LED_BRIGHTNESS 50  // 1-255

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

//Ethernet settings
byte packetBuffer[3]; //buffer to hold incoming packets
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xAA, 0xEA, 0x01 };  // MAC
IPAddress ip(2, 0, 0, 101);                           // IP
IPAddress subnet(255, 0, 0, 0);                       // SUBNET

//sACN settings
unsigned int localPortSACN = 5568;  // Port dla danych sACN
unsigned int sACN_Universe = 1;  // Nr Universe dla odbioru sACN 1-63999

EthernetUDP UdpSACN;

void setup() {

  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(LED_BRIGHTNESS);  //set led brighness

  Ethernet.init(10);  //Ethernet init
  if (Ethernet.begin(mac) == 0) {//check DHCP if not - set static ip
    Ethernet.begin(mac, ip);
    Ethernet.setSubnetMask(subnet);
  }

  // Recalculate  sACN_Universe to IP multicast
  unsigned int x = (sACN_Universe / 256);
  unsigned int y = (sACN_Universe - (x * 256));

  // Inicjalize UDP for multicast sACN_Universe
  IPAddress multicastIP(239, 255, x, y);
  UdpSACN.beginMulticast(multicastIP, localPortSACN);

}

void loop() {

  receiveSACN(UdpSACN); //Recive sACN

}

void receiveSACN(EthernetUDP &udp) {
  int packetSizeSACN = udp.parsePacket();
  if (packetSizeSACN > 0) {

    udp.read(packetBuffer, 1);
    int startCode = packetBuffer[0];

    if (startCode == 0x00) {//check start code

      for (unsigned int i = 1; i < 126; i++) {//read udp buffer 1-126
        udp.read(packetBuffer, 1);
      }

      for (unsigned int i = 0; i < LED_COUNT; i++) {//read udp buffer data an copy to WS LED strip buffer
        udp.read(packetBuffer, 3);
        strip.setPixelColor(i, packetBuffer[0], packetBuffer[1], packetBuffer[2]);
      }
      strip.show();//send data to WS LED
    }
  }
}
