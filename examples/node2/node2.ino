#include <NativeEthernet.h>
#include <TeensyDMX.h>
namespace teensydmx = ::qindesign::teensydmx;
#include <TeensyArtNet.h>

byte MAC_ADDRESS[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


IPAddress SELF_IP(192, 168, 1, 90);
IPAddress BROADCAST_IP(192, 168, 1, 255);

elapsedMillis timeSincePacket = 0;

teensydmx::Sender dmxTx1{Serial2};
teensydmx::Sender dmxTx2{Serial1};

teensyartnet::Receiver artnetRx;

void setup() {
  Serial.begin(9600);
  
  Ethernet.begin(MAC_ADDRESS, SELF_IP);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWriteFast(2, HIGH);
  digitalWriteFast(6, HIGH);

  
  dmxTx1.begin();
  dmxTx2.begin();

  artnetRx.begin();
  artnetRx.addUniverse(5);
  artnetRx.addUniverse(6);
}



void loop() {

  artnetRx.update();
  
  if(artnetRx.universeHasData(5)){
    resetIndicatorTimer();
    for(int i=1; i<=512; i++){
      dmxTx1.set(i, artnetRx.getSlot(5, i));
    }
  }

  if(artnetRx.universeHasData(6)){
    resetIndicatorTimer();
    for(int i=1; i<=512; i++){
      dmxTx2.set(i, artnetRx.getSlot(6, i));
    }
  }


  if (timeSincePacket < 20) {
    digitalWriteFast(LED_BUILTIN, HIGH);
  } else {
    digitalWriteFast(LED_BUILTIN, LOW);
  }

}

void resetIndicatorTimer() {
  if (timeSincePacket > 50) {
    timeSincePacket = 0;
  }
}