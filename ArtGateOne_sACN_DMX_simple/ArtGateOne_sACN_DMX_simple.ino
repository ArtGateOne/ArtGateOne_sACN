//ArtGateOne sACN - DMX simple node - ONE UNIVERSE

#include <lib_dmx.h>  // comment/uncomment #define USE_UARTx in lib_dmx.h as needed
#include <Ethernet.h>
#include <EthernetUdp.h>

#define DMX512 (0)  // (250 kbaud - 2 to 512 channels) Standard USITT DMX-512

//Ethernet settings
byte packetBuffer[3];                                 //buffer to hold incoming packets
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xAA, 0xEA, 0x01 };  // MAC
IPAddress ip(2, 0, 0, 101);                           // IP
IPAddress subnet(255, 0, 0, 0);                       // SUBNET

//sACN settings
unsigned int localPortSACN = 5568;  // Port dla danych sACN
unsigned int sACN_Universe = 1;     // Nr Universe dla odbioru sACN 1-63999

unsigned int i = 0;

EthernetUDP UdpSACN;

void setup() {

  Ethernet.init(10);               //Ethernet init
  if (Ethernet.begin(mac) == 0) {  //check DHCP if not - set static ip
    Ethernet.begin(mac, ip);
    Ethernet.setSubnetMask(subnet);
  }

  // Recalculate  sACN_Universe to IP multicast
  unsigned int x = (sACN_Universe / 256);
  unsigned int y = (sACN_Universe - (x * 256));

  // Inicjalize UDP for multicast sACN_Universe
  IPAddress multicastIP(239, 255, x, y);
  UdpSACN.beginMulticast(multicastIP, localPortSACN);

  ArduinoDmx0.set_control_pin(-1);   // Arduino output pin for MAX485 input/output control (connect to MAX485-1 pins 2-3)(-1 not used)
  ArduinoDmx0.set_tx_address(1);     // set rx1 start address
  ArduinoDmx0.set_tx_channels(512);  // 2 to 2048!! channels in DMX1000K (512 in standard mode) See lib_dmx.h  *** new *** EXPERIMENTAL
  ArduinoDmx0.init_tx(DMX512);       // starts universe 1 as tx, standard DMX 512 - See lib_dmx.h, now support for DMX faster modes (DMX 1000K)

}

void loop() {

  receiveSACN(UdpSACN);  //Recive sACN
}

void receiveSACN(EthernetUDP &udp) {
  int packetSizeSACN = udp.parsePacket();
  if (packetSizeSACN > 0) {

    for (i = 0; i < 126; i++) {  //read udp buffer 1-126
      udp.read(packetBuffer, 1);
    }

    for (i = 0; i < 512; i++) {  //read udp buffer data an copy to DMX buffer
      udp.read(packetBuffer, 1);
      ArduinoDmx0.TxBuffer[i] = packetBuffer[0];
    }
  }
}
