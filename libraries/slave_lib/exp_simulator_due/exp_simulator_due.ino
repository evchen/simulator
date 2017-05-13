/*
 * exp_simulator for the MIST project at KTH.
 * 
 * Written by Johan Sjöblom and John Wikman
 */

extern "C" {
  #include "I2Cslave.h"
  #include "SlaveDefinitions.h"
  #include "debug2.h"
  #include "CRC.h"
  #include "PUSGenerator.h"
  #include "Endian.h"
}

typedef enum {
  NO_DATA, //only to be used if we didn't send any data yet
  PAYLOAD_DATA,
  HK_DATA,
  PUS_DATA
} Data_Type;

typedef enum {
    S_READY,
    S_REC_TIME,
    S_REC_PUS,
    S_SEND_PAYLOAD_HEADER,
    S_SEND_PAYLOAD_DATA,
    S_SEND_HK_HEADER,
    S_SEND_HK_DATA,
    S_SEND_PUS_HEADER,
    S_SEND_PUS_DATA,
  } Slave_State;
static Slave_State slaveState;

typedef struct __attribute__((__packed__)) {
  uint8_t configID;
  uint32_t currentTime;
  uint8_t hasPayloadToSend;
} EXP_HK;
static EXP_HK HK;

//keep track of sequence numbers
static uint8_t latestReceivedSeq; //the last seq received
static uint8_t currentlyReceivingSeq; //the seq of packet currently being received
static uint8_t latestSentSeq; //the last seq sent
static uint8_t currentlySendingSeq; //the seq of packet currently being sent

static Data_Type lastSentDataType; //used for debug purposes
static uint32_t packetsLeft;
static uint32_t payloadSize = 260;    //some init, should get overwritten by config
static uint32_t bitflipChance;     //chance of bitflip = bitflipChange / 1000
static uint8_t bitflipRegion;       //0 == anywhere, 1 == ADDR, 2 == msgID, 3 == DATA

static PUS_Packet PUSPacket;

void setup() {
  Serial.begin(9600);
  reset(); //resets local state-parameters
  Serial.println("Lazers online");
  HK.configID = 0;
  HK.currentTime = 0;
  HK.hasPayloadToSend = payloadSize > 0;
  lastSentDataType = NO_DATA;
  latestSentSeq = 250;
  bitflipChance = 0;
  bitflipRegion = 0;
  debugSetup();
  I2C_setup(receiveCallback, transmitBeginCallback, transmitCompleteCallback);
}

uint32_t x;
void loop() {
  printDebugs();
  
  if (x%10000000 == 0) {
    writeDebug("alive");
  }
  x++;
}

void transmitCompleteCallback() {
  switch(slaveState) {
    case S_READY:
      //sent null, should not update seq nr
      break;
    case S_SEND_PAYLOAD_HEADER:
      writeDebug("PayLoad header sent");
      packetsLeft--;
      slaveState = S_SEND_PAYLOAD_DATA;
      break;
    case S_SEND_PAYLOAD_DATA:
      writeDebug("sent payload packet");
      if (--packetsLeft == 0) {
        writeDebug("Sent entire payload");
        //transaction complete
        latestSentSeq = currentlySendingSeq;
        lastSentDataType = PAYLOAD_DATA;
        reset();
      }
      break;
    case S_SEND_HK_HEADER:
      writeDebug("HK header sent");
      packetsLeft--;
      slaveState = S_SEND_HK_DATA;
      break;
    case S_SEND_HK_DATA:
      if (--packetsLeft == 0) {
        writeDebug("Sent entire HK");
        //transaction complete
        latestSentSeq = currentlySendingSeq;
        lastSentDataType = HK_DATA;
        reset();
      } else {
        writeDebug("Vafan, Johan?");
      }
      break;
    case S_SEND_PUS_HEADER:
      writeDebug("PUS header sent");
      packetsLeft--;
      slaveState = S_SEND_PUS_HEADER;
      break;
    case S_SEND_PUS_DATA:
      if (--packetsLeft == 0) {
        writeDebug("Sent entire PUS");
        //transaction complete
        latestSentSeq = currentlySendingSeq;
        lastSentDataType = PUS_DATA;
        reset();
      }
    default:
      writeDebug("E10");
  }
}

void transmitBeginCallback() { 
  switch(slaveState) {
    case S_READY:
      writeDebug("TR while S_READY");
      prepareNullHeader();
      break;
    case S_SEND_PAYLOAD_HEADER:
      preparePayloadHeader();
      break;
    case S_SEND_PAYLOAD_DATA:
      preparePayloadData();
      break;
    case S_SEND_HK_HEADER:
      prepareHKHeader();
      break;
    case S_SEND_HK_DATA:
      prepareHKData();
      break;
    case S_SEND_PUS_HEADER:
      preparePUSHeader();
      break;
    case S_SEND_PUS_DATA:
      preparePUSData();
      break;
    default:
      writeDebug("E2");
  }
  //simulation based on bitflipRegion and bitflipChance
  simulateSEU(); 
}

void receiveCallback(uint8_t* data, uint32_t dataSize) {
  if (data[0] != I2C_SLAVE_RECV_ADDR) {
    writeDebug("ADDR field mismatch");
    reset();
    return;
  }
  if (!CRCIsValid(data, dataSize)) {
    writeDebug("CRC-auth failed");
    reset();
    return;
  }
  if (data[1] == MSP_NULL) {
    char debug[32]; debug[0] = '\0';
    strcat(debug, "rec 0x00, seq = "); itoa(data[2], debug + strlen(debug), 10);
    writeDebug(debug);
    reset();
    return;
  }
  
  switch(slaveState) {
    case S_READY:
      handleFreshRead(data, dataSize);
      break;
    case S_REC_TIME:
      handleTimeRead(data, dataSize);
      break;
    case S_REC_PUS:
      handlePUSRead(data, dataSize);
      break;
    default:
      writeDebug("E20, reached default");
  }
}

/**/
void handleFreshRead(uint8_t* data, int dataSize) {
  if (dataSize != 9) {
    writeDebug("Received 'fresh' packet with wrong length.");
    reset();
    return;
  }

  uint8_t msgID = data[1];
  uint8_t seqNr = data[2];
  uint16_t DL = fromBigEndian16(data + 3);
  switch(msgID) {
    case MSP_OBC_REQ_HK:
      writeDebug("HK req");
      currentlySendingSeq = getNextSeqNr(seqNr, HK_DATA);
      slaveState = S_SEND_HK_HEADER;
      break;
    case MSP_OBC_REQ_PAYLOAD:
      writeDebug("PLoad req");
      currentlySendingSeq = getNextSeqNr(seqNr, PAYLOAD_DATA);
      slaveState = S_SEND_PAYLOAD_HEADER;
      break;
    case MSP_OBC_REQ_PUS:
      writeDebug("PUS req");
      currentlySendingSeq = getNextSeqNr(seqNr, PUS_DATA);
      if (currentlySendingSeq == latestSentSeq) {
        //this is a re-send, dont update PUSPacket
      } else {
        PUS_Packet *newPUS = getPUSPacket();
        if (newPUS == NULL) {
          PUSPacket.dataSize = 0; 
        } else {
          int i; 
          for (i = 0; i < newPUS->dataSize; i++) {
            PUSPacket.data[i] = (newPUS->data)[i];
          }
          PUSPacket.dataSize = newPUS->dataSize;
        }
      }  
      slaveState = S_SEND_PUS_HEADER;
    case MSP_OBC_SEND_TIME:
      writeDebug("rec time");
      currentlyReceivingSeq = seqNr;
      packetsLeft = 1;
      slaveState = S_REC_TIME;
      break;
    case MSP_OBC_SEND_PUS:
      writeDebug("rec PUS-header");
      currentlyReceivingSeq = seqNr;
      packetsLeft = DL / (I2C_BUFFER_SIZE - 6);
      if (DL % (I2C_BUFFER_SIZE - 6) != 0) packetsLeft++;
      if (packetsLeft > 0) {
        slaveState = S_REC_PUS;
      } else {
        writeDebug("PUS-header where DL=0, resetting state");
        reset();
      }
      break;
    default:
      writeDebug("E4");
      reset();
  }
}

uint8_t getNextSeqNr(uint8_t requestedSeq, Data_Type dataType) {
  char debug1[32]; debug1[0] = '\0';
  strcat(debug1, "OBC req SeqNr: "); itoa(requestedSeq, debug1 + strlen(debug1), 10);
  writeDebug(debug1);
  if (requestedSeq == (uint8_t) (latestSentSeq + 1)) {
    //all is in order, proceeed (also handles overflow)
    return requestedSeq; 
  } else if (requestedSeq == latestSentSeq) {
    if (dataType = lastSentDataType) {
      //should resend latest sent packet
      writeDebug("Preparing to resent last sent packet");
      return requestedSeq;
    } else {
      writeDebug("ERROR, OBC request wrong dataType!");
    }
  } else {
    //OBC requests something else than next or previous packet..
    writeDebug("Can't service OBC:s requested seqNr");    
  }
  return latestSentSeq; //for some reason couldn't handle request, resend latest Sent
}

/* struct configLayout {
  uint8_t addr;
  uint8_t msgID;
  uint8_t configID;
  uint32_t payloadSize;
  uint8_t payloadDataValue;   //REMOVED!!
  
  uint32_t bitflipChance;     //chance of bitflip = bitflipChange / 1000
  uint8_t bitflipRegion;       //0 == anywhere, 1 == ADDR, 2 == msgID, 3 == DATA
}; */
void handleConfigRead(uint8_t* data, int dataSize) {
  if (packetsLeft < 1) {
    writeDebug("E11");
    reset();
    return;
  }
  if(data[1] != MSP_DATA || dataSize != 17) {
    writeDebug("E12, handling as if ready");
    handleFreshRead(data, dataSize);
    return;
  }
  writeDebug("rec config-data");
  if (data[2] == HK.configID) {
    writeDebug("config alrdy up to date");
    //return;
    writeDebug("Applying config anyway.");
  }
  HK.configID = data[2];
  payloadSize = fromBigEndian32(data + 3);
  bitflipChance = fromBigEndian32(data + 8);
  bitflipRegion = data[12];
  writeDebug("config updated");
  char debug1[32]; debug1[0] = '\0';
  strcat(debug1, "new plSize: "); itoa(payloadSize, debug1 + strlen(debug1), 10);
  writeDebug(debug1);
  char debug2[32]; debug2[0] = '\0';
  strcat(debug2, "new bitflipChance: "); itoa(bitflipChance, debug2 + strlen(debug2), 10);
  writeDebug(debug2);
  char debug3[32]; debug3[0] = '\0';
  strcat(debug3, "new bitflipRegion: "); itoa(bitflipRegion, debug3 + strlen(debug3), 10);
  writeDebug(debug3);
  reset();
}

void handleTimeRead(uint8_t *packet, uint32_t packetSize) {
  char debug[32]; debug[0] = '\0';
  strcat(debug, "Rec time-header, NT: ");
  itoa(HK.currentTime, debug + strlen(debug), 10);
  strcat(debug, "\0");
  HK.currentTime = fromBigEndian32(packet + 2);
  latestReceivedSeq = currentlyReceivingSeq;
  reset();
}

void handlePUSRead(uint8_t *packet, uint32_t packetSize) {
  writeDebug("PUS packet received");
  if (packetsLeft != 1) {
    //assuming here PUS-packet fits in one MSP-packet (might not be true, 
    //depending on size of future PUS-services).
    writeDebug("Error in PUS-packet size");
    generatePUSAcknowledges(packet + 2, packetSize - 6);
    reset();
    return;
  }
  latestReceivedSeq = currentlyReceivingSeq;
  reset();
}

void preparePayloadHeader() {
    currentPacket.MSP_packet[0] = I2C_SLAVE_SEND_ADDR;
    currentPacket.MSP_packet[1] = MSP_EXP_SEND_PAYLOAD;
    currentPacket.MSP_packet[2] = currentlySendingSeq;
    toBigEndian16(currentPacket.MSP_packet + 3, payloadSize);
    appendCRC(currentPacket.MSP_packet, 5);
    //set num packets to send
    packetsLeft = 1; //for header
    if (payloadSize > 0) {
      packetsLeft += payloadSize / (I2C_BUFFER_SIZE - 6); //6 is overhead
      if ((payloadSize % (I2C_BUFFER_SIZE - 6) ) != 0) {
        packetsLeft++;
      }
    }
    
    currentPacket.packetSize = 9;
    //debug
    char debug[32]; debug[0] = '\0';
    strcat(debug, "prep'd PL header. Seq = ");
    itoa(currentlySendingSeq, debug + strlen(debug), 10);
    writeDebug(debug);
    //end debug
}

void preparePayloadData() {
  if (packetsLeft <= 0) {
    writeDebug("E5");
    prepareNullHeader();
    return;
  }
  //debug
  char debug[32]; debug[0] = '\0';
  strcat(debug, "prep PL data, packetsL: ");
  itoa(packetsLeft, debug + strlen(debug), 10);
  writeDebug(debug);
  //end debug
  currentPacket.MSP_packet[0] = I2C_SLAVE_SEND_ADDR;
  currentPacket.MSP_packet[1] = MSP_DATA;
  uint32_t dataToSend;
  if (packetsLeft > 1) {
    dataToSend = I2C_BUFFER_SIZE - 6;
  } else {
    dataToSend = payloadSize % (I2C_BUFFER_SIZE - 6);
  }
  int i;
  for (i = 2; i < (dataToSend + 2); i++) {
    currentPacket.MSP_packet[i] = currentlySendingSeq;
  }
  appendCRC(currentPacket.MSP_packet, dataToSend + 2); //+2 to include ADDR+msgID
  currentPacket.packetSize = 1 + 1 + dataToSend + 4;
}

void prepareHKHeader(){
  writeDebug("Preparing HK header");
  currentPacket.MSP_packet[0] = I2C_SLAVE_SEND_ADDR;
  currentPacket.MSP_packet[1] = MSP_EXP_SEND_HK;
  currentPacket.MSP_packet[2] = currentlySendingSeq;
  toBigEndian16(currentPacket.MSP_packet + 3, sizeof(EXP_HK));
  appendCRC(currentPacket.MSP_packet, 5);
  //update packetsLeft
  packetsLeft = 1; //for header
  if (sizeof(EXP_HK) > 0) {
    packetsLeft += sizeof(EXP_HK) / (I2C_BUFFER_SIZE - 6); //6 is overhead
    if ((sizeof(EXP_HK) % (I2C_BUFFER_SIZE - 6) ) != 0) {
      packetsLeft++;
    }
  }

  currentPacket.packetSize = 9;
  //debug
  char debug[32]; debug[0] = '\0';
  strcat(debug, "prep'd PL header. Seq = ");
  itoa(currentlySendingSeq, debug + strlen(debug), 10);
  writeDebug(debug);
  //end debug
}

void prepareHKData(){
  writeDebug("Preparing HK data");
  currentPacket.MSP_packet[0] = I2C_SLAVE_SEND_ADDR;
  currentPacket.MSP_packet[1] = MSP_DATA;
  currentPacket.MSP_packet[2] = HK.configID;
  toBigEndian32(currentPacket.MSP_packet + 3, HK.currentTime);
  currentPacket.MSP_packet[7] = payloadSize > 0;
  appendCRC(currentPacket.MSP_packet, 8);
  currentPacket.packetSize = 8 + 4;
}

void preparePUSHeader() {
  currentPacket.MSP_packet[0] = I2C_SLAVE_SEND_ADDR;
  currentPacket.MSP_packet[1] = MSP_EXP_SEND_PUS;
  currentPacket.MSP_packet[2] = currentlySendingSeq;
  toBigEndian16(currentPacket.MSP_packet + 3, PUSPacket.dataSize);

  //set num packets to send
  packetsLeft = 1; //for header
  if (PUSPacket.dataSize > 0) {
    packetsLeft += PUSPacket.dataSize / (I2C_BUFFER_SIZE - 6); //6 is overhead
    if ((PUSPacket.dataSize % (I2C_BUFFER_SIZE - 6) ) != 0) {
      packetsLeft++;
    }
  }

  currentPacket.packetSize = 9;
  //debug
  char debug[32]; debug[0] = '\0';
  strcat(debug, "prep'd PUS header. Seq = ");
  itoa(currentlySendingSeq, debug + strlen(debug), 10);
  writeDebug(debug);
  //end debug
}

void preparePUSData() {
   if (packetsLeft <= 0) {
    writeDebug("E24");
    prepareNullHeader();
    return;
  }
  currentPacket.MSP_packet[0] = I2C_SLAVE_SEND_ADDR;
  currentPacket.MSP_packet[1] = MSP_DATA;
  int totalBytesLeftToSend = (packetsLeft - 1) * (I2C_BUFFER_SIZE - 6) + PUSPacket.dataSize % (I2C_BUFFER_SIZE - 6);
  int startIndex = PUSPacket.dataSize - totalBytesLeftToSend;
  int dataToSend = min(totalBytesLeftToSend, I2C_BUFFER_SIZE - 6);
  int i;
  for (i = 0; i < dataToSend; i++) {
    currentPacket.MSP_packet[i] = PUSPacket.data[startIndex + i];
  }
  appendCRC(currentPacket.MSP_packet, dataToSend + 2); //+2 to include ADDR+msgID
  currentPacket.packetSize = 1 + 1 + dataToSend + 4;
  
  if (currentPacket.packetSize > I2C_BUFFER_SIZE) {
    writeDebug("Bad code, investigate!");
  }
}

void prepareNullHeader() {
  //debug
  char debug[32]; debug[0] = '\0';
  strcat(debug, "prep'd NULL header. Seq = ");
  itoa(latestReceivedSeq, debug + strlen(debug), 10);
  writeDebug(debug);
  //end debug
  currentPacket.MSP_packet[0] = I2C_SLAVE_SEND_ADDR;
  currentPacket.MSP_packet[1] = MSP_NULL;
  currentPacket.MSP_packet[2] = latestReceivedSeq;
  toBigEndian16(currentPacket.MSP_packet + 3, 0);
  appendCRC(currentPacket.MSP_packet, 5);
  currentPacket.packetSize = 9;
}
/**/

void simulateSEU() {
  if (bitflipChance == 0)
    return;
  uint16_t r = rand() % 1000;
  if (r >= bitflipChance) {
    return;
  }
  switch(bitflipRegion) {
    //bitflipRegion indicates in which byte should be occur.
    //if bitflipRegion > 10, 
    case 0: 
      bitflip(currentPacket.MSP_packet, currentPacket.packetSize - 4);
      break;
    case 1: 
      bitflip(currentPacket.MSP_packet, 1);
      break;
    case 2: 
      bitflip(currentPacket.MSP_packet + 1, 1);
      break;
    case 3: 
      bitflip(currentPacket.MSP_packet + 2, currentPacket.packetSize - 6);
      break;
    default:
      writeDebug("E12");
  }
}

void bitflip(uint8_t *bitflipTarget, uint32_t len) {
  uint32_t byteIndex = rand() % len;
  uint8_t bitIndex = rand() % 8;
  uint8_t mask = (uint8_t)(1 << bitIndex);
  if ( bitflipTarget[byteIndex] & mask ) {
    bitflipTarget[byteIndex] &= ~mask;  
  } else {
    bitflipTarget[byteIndex] |= mask;
  }
  writeDebug("flippin'");
}

/**
 *  Calc CRC on data [src, src+len) and place result in src+len+1
 */
void appendCRC(uint8_t *src, uint32_t len) {
  //uint32_t crc32(const uint8_t *data, uint32_t length);
  toBigEndian32(src + len, crc32(src,len));
}

uint8_t CRCIsValid(uint8_t *packet, uint32_t packetSize) {
  uint32_t givenCRC = fromBigEndian32(packet + packetSize - 4);
  return givenCRC == crc32(packet, packetSize - 4);
}

void reset() {
  packetsLeft = 0;
  currentlyReceivingSeq = 0;
  currentlySendingSeq = 0;
  slaveState = S_READY;
}

