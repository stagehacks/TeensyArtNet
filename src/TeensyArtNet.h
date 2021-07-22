#include "Arduino.h"
#include <NativeEthernetUdp.h>


namespace teensyartnet{

  const char VALID_HEADER[9] = "Art-Net\0";
  const uint16_t OPCODE_ART_DMX = 0x5000;
  const uint16_t OPCODE_ART_POLL = 0x2000;


  class Receiver{
    public:
      Receiver();
      void begin();
      void update();
      boolean universeHasData(byte universe);
      byte getSlot(byte universe, int slot);
      void setUniverse(byte universe);
      void addUniverse(byte universe);
    private:
      EthernetUDP interface;
      byte _hasData = false;
      byte _universes[512];
      byte _universeList[4];
      unsigned long _universeMillis[4];
      byte _universeIndex = 0;
      byte _slots[4][512];
      int _slotCount;
      unsigned char _packetBuffer[636];
      void _packetReceived(unsigned char * buf);
      void _artpollReceived(unsigned char * buf);
      int _checkHeader(unsigned char * msg, int len);
  };

  struct ArtPollReply {
    uint8_t  id[8];
    uint16_t opCode;
    uint8_t  ipAddress[4];
    uint16_t port;
    uint8_t  versInfoH;
    uint8_t  versInfoL;
    uint8_t  netSwitch;
    uint8_t  subSwitch;
    uint8_t  oem[2];
    uint8_t  ubea;
    uint8_t  status1;
    uint8_t  estaMan[2];
    uint8_t  shortName[18];
    uint8_t  longName[64];
    uint8_t  nodeReport[64];
    uint8_t  numPortsHi;
    uint8_t  numPorts;
    uint8_t  portTypes[4];
    uint8_t  goodInput[4];
    uint8_t  goodOutput[4];
    uint8_t  swIn[4];
    uint8_t  swOut[4];
    uint8_t  swVideo;
    uint8_t  swMacro;
    uint8_t  swRemote;
    uint8_t  spares[3];
    uint8_t  style;
    uint8_t  mac[6];
    uint8_t  bindIp[4];
    uint8_t  bindIndex;
    uint8_t  status2;
    uint8_t  goodOutputB[4];
    uint8_t  status3;
    uint8_t  filler[21];
  };



}
