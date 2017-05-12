/*
 * Experiment.c
 *
 * Author: John Wikman
 *
 * Description:
 * TBW
 */

#include <inttypes.h>

#include "Experiment.h"
#include "MSPSequence.h"
                                   /*    tx_seq            rx_seq          poll frequencies */
#define EXP_CONFIG_DEFAULT_VALUES   MSP_NEW_SEQUENCE, MSP_NEW_SEQUENCE, {0, 0}, {0, 0}, {0, 0}

/* Initialize experiments */
exp_conf_t EXP_configuration[9] = {
  {0x08, EXP_CONFIG_DEFAULT_VALUES},
  {0x11, EXP_CONFIG_DEFAULT_VALUES},
  {0x12, EXP_CONFIG_DEFAULT_VALUES},
  {0x13, EXP_CONFIG_DEFAULT_VALUES},
  {0x14, EXP_CONFIG_DEFAULT_VALUES},
  {0x15, EXP_CONFIG_DEFAULT_VALUES},
  {0x16, EXP_CONFIG_DEFAULT_VALUES},
  {0x17, EXP_CONFIG_DEFAULT_VALUES},
  {0x18, EXP_CONFIG_DEFAULT_VALUES}
};

/*
 * Returns 1 if OBC should poll, else 0
 */
int EXP_shouldPoll(exp_poll_t *p, uint32_t currentTime)
{
  if (p->freq == 0)
    return 0;

  if (currentTime - p->last > p->freq) {
    p->last = currentTime;
    return 1;
  }

  return 0;
}

void EXP_reset(uint8_t exp_id)
{
  if (exp_id >= EXP_AMOUNT)
    return;
  
  X_CONF(exp_id).tx_seq = MSP_newSequence();
  X_CONF(exp_id).rx_seq = MSP_newSequence();
  X_CONF(exp_id).payload.freq = 0;
  X_CONF(exp_id).payload.last = 0;
  X_CONF(exp_id).hk.freq = 0;
  X_CONF(exp_id).hk.last = 0;
  X_CONF(exp_id).pus.freq = 0;
  X_CONF(exp_id).pus.last = 0;
}

