/*
 * Experiment.h
 *
 * Author: John Wikman
 *
 * Description:
 * Contains experiment definitions and configurations.
 * 
 * TODO:
 * - Change so that polling time is stored in milliseconds
 *   instead of seconds.
 */

#ifndef EXP_CONFIGURATION_H
#define EXP_CONFIGURATION_H

#include <inttypes.h>

#include "MSPSequence.h"

/* Internal ID's for all experiments */
#define EXP_TEST     0
#define EXP_CUBES    1
#define EXP_LEGS     2
#define EXP_MOREBAC  3
#define EXP_CUBEPROP 4
#define EXP_RATEX    5
#define EXP_SEUD     6
#define EXP_SIC      7
#define EXP_CAMERA   8

/* Always check that given experiment ID is less than this */
#define EXP_AMOUNT 9

/* Accessors for experiments */
#define X_TEST      EXP_configuration[EXP_TEST]
#define X_CUBES     EXP_configuration[EXP_CUBES]
#define X_LEGS      EXP_configuration[EXP_LEGS]
#define X_MOREBAC   EXP_configuration[EXP_MOREBAC]
#define X_CUBEPROP  EXP_configuration[EXP_CUBEPROP]
#define X_RATEX     EXP_configuration[EXP_RATEX]
#define X_SEUD      EXP_configuration[EXP_SEUD]
#define X_SIC       EXP_configuration[EXP_SIC]
#define X_CAMERA    EXP_configuration[EXP_CAMERA]
// Generalized one
#define X_CONF(exp_id) (EXP_configuration[exp_id])

typedef struct msp_experiment_poll_configuration {
  uint16_t freq; /* poll frequency in mHz */
  uint32_t last; /* time when last polled in milliseconds  */
} exp_poll_t;

typedef struct msp_experiment_configuration {
  const uint8_t addr;
  struct msp_sequencing tx_seq;
  struct msp_sequencing rx_seq;
  exp_poll_t payload;
  exp_poll_t hk;
  exp_poll_t pus;
} exp_conf_t;

extern exp_conf_t EXP_configuration[];

/*
 * Returns 1 if OBC should poll, else 0.
 * 
 * currentTime is time in milliseconds.
 * 
 * If 1 is returned, then it is assumed that the experiment will be
 * polled and it will not return 1 until the next time for the
 * experiment to be polled again.
 */
int EXP_shouldPoll(exp_poll_t *p, uint32_t currentTime);

/* Resets an experiment */
void EXP_reset(uint8_t exp_id);

#endif


