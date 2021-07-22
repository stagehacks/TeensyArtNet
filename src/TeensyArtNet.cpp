#include "Arduino.h"
#include "TeensyArtNet.h"
#include <NativeEthernetUdp.h>

teensyartnet::Receiver::Receiver(){
  
}
void teensyartnet::Receiver::begin(){
  interface.begin(6454);
  //interface.beginMulticast(IPAddress(239, 255, 0, 2), 5568);
  Serial.println(_universes[0]);
}

void teensyartnet::Receiver::update(){
  int packetSize = interface.parsePacket();
  if (packetSize) {
    interface.read(_packetBuffer, 636);
    uint16_t _opcode = _checkHeader(_packetBuffer, packetSize);    
    
    if (_opcode==OPCODE_ART_DMX){
      _packetReceived(_packetBuffer);

    }else if(_opcode==OPCODE_ART_POLL){
      _artpollReceived(_packetBuffer);

    }
  }
}



void teensyartnet::Receiver::addUniverse(byte universe){
  if(_universeIndex < 4){
    _universes[universe] = _universeIndex+1;
    _universeList[_universeIndex] = universe+1;
    _universeIndex++;
  }
}




boolean teensyartnet::Receiver::universeHasData(byte universe){
  return millis() - _universeMillis[_universes[universe]-1] < 2000;
}
byte teensyartnet::Receiver::getSlot(byte universe, int slot){
  return _slots[_universes[universe]-1][slot-1];
}






void teensyartnet::Receiver::_packetReceived(unsigned char * buf){

  byte sequence = _packetBuffer[12];
  byte universe = _packetBuffer[14] | _packetBuffer[15] << 8;
  _slotCount = _packetBuffer[17] | _packetBuffer[16] << 8;

  if(_universes[universe]>0){
    for (int slot = 0; slot < _slotCount; slot++) {
      _universeMillis[_universes[universe]-1] = millis();
      _slots[_universes[universe]-1][slot] = buf[18 + slot];
    }
  }

}

void teensyartnet::Receiver::_artpollReceived(unsigned char * buf){

  Serial.println("poll reply");
  ArtPollReply pollReply;

  
  uint8_t  id[8];
  uint8_t  node_ip_address[4] = {192,168,1,90};

  sprintf((char *)id, "Art-Net");
  memcpy(pollReply.id, id, sizeof(pollReply.id));
  memcpy(pollReply.ipAddress, node_ip_address, sizeof(pollReply.ipAddress));

  pollReply.opCode = 0x2100; // ART_POLL_REPLY
  pollReply.port =  6454;

  memset(pollReply.goodInput,  0x08, 4);
  memset(pollReply.goodOutput,  0x80, 4);
  memset(pollReply.portTypes,  B00000011, _universeIndex);

  uint8_t shortName[18];
  uint8_t longName[64];
  sprintf((char *)shortName, "NODE_2");
  sprintf((char *)longName, "Stage Hacks NODE_2");
  memcpy(pollReply.shortName, shortName, sizeof(shortName));
  memcpy(pollReply.longName, longName, sizeof(longName));

  pollReply.numPortsHi = 0;
  pollReply.numPorts = (uint8_t) _universeIndex;
  pollReply.status2  = 0x08;

  pollReply.swOut[0] = _universeList[0]-1;
  pollReply.swOut[1] = _universeList[1]-1;
  pollReply.swOut[2] = _universeList[2]-1;
  pollReply.swOut[3] = _universeList[3]-1;

  IPAddress BROADCAST_IP(192, 168, 1, 255);

  interface.beginPacket(BROADCAST_IP, 6454);
  interface.write((uint8_t *)&pollReply, sizeof(pollReply));
  interface.endPacket();

}

int teensyartnet::Receiver::_checkHeader(unsigned char * msg, int len) {

  for (byte i = 0; i < 8; i++) {
    if (msg[i] != VALID_HEADER[i]) {
      return 0;
    }
  }

  return _packetBuffer[8] | _packetBuffer[9] << 8;

}
