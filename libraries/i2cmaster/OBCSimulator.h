/*
 * OBCSimulator.h
 *
 * Author: John Wikman
 *
 * Description:
 * Provides convinient functions for setting up OBC simulation.
 */

#ifndef OBCSIMULATOR_H
#define OBCSIMULATOR_H

/*
 * Sets up the arduino at the given address to inherit properties
 * according to the specified parameters.
 * 
 * I2C MUST be started before this is called.
 */
void setupSimulation(uint8_t address, uint8_t configID, uint32_t payloadSize, uint8_t payloadData, uint32_t bitflipChance, uint8_t bitflipRegion);

#endif
