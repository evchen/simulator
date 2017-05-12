/*
 * OBCSimulator.c
 *
 * Author: John Wikman
 *
 * Description:
 * Provides convinient functions for setting up OBC simulation.
 */

#include <Time.h>

extern "C" {
  #include "Experiment.h"
  #include "I2C_configuration.h"
  #include "MSPCommunication.h"
  #include "Endian.h"
  #include "MSPSequence.h"
}

#include "Arduino.h"

#define ENABLE_DEBUG
#include "debug.h"

/*
 * Sets up the OBC Simulator (Arduino <-> Arduino)
 *
 * Params
 * - address: Specify which experiment to talk to.
 * - configID: The id of their current configuration (for HK purposes).
 * - payloadSize: The number of bytes in data to be sent when requesting payload.
 * - payloadData: The initial byte-value for the payload send. Will be incremented for each transmission.
 * - bitflipChance: (chance of bitflip happening) / 1000
 * - bitflipRegion: When in the transmission bitflips will occur. (0 anywhere, 1 ADDR, 2 OP CODE, 3 DATA)
 */
void setupSimulation(uint8_t exp_id, uint8_t configID, uint32_t payloadSize, uint8_t payloadData, uint32_t bitflipChance, uint8_t bitflipRegion)
{
  while (1) {
  	uint8_t data[11];
  	data[0] = configID;
  	toBigEndian32(data + 1, payloadSize);
  	data[5] = payloadData; // payload data value
  	toBigEndian32(data + 6, bitflipChance); // bitflip change
  	data[10] = bitflipRegion; // Bitflip region: 0 anywhere, 1 ADDR, 2 msgID, 3 DATA
  
  	MSP_COM com = MSP_generateGeneric(MSP_OBC_SEND_CONFIG, exp_id, data, sizeof(data), NULL, 0);
  
  	MSP_executeAll(&com);
  
  	if (com.code == 0) {
      debugln("Success setting up");
      return;
  	} else {
      debugdecln("Failed setting up", com.code);
      MSP_COM reset = MSP_generateGeneric(MSP_NULL, exp_id, NULL, 0, NULL, 0);
      MSP_executeAll(&reset);
      if (reset.code != 0)
        debugln("Failed Reset");
  	}
    delay(2000);
  }
  return;
}

