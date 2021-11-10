/**
 * regtable
 *
 * Copyright (c) 2015 Daniel Berenguer <dberenguer@usapiens.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 04/01/2015
 */

#include "product.h"
#include "regtable.h"

STORAGE nvMem;
uint8_t MemoryValue[2];

/**
 * Definition of common registers
 */
DEFINE_COMMON_REGISTERS()

/*
 * Definition of custom registers
 */
// setup register
byte dtConfiguration[14];
/*
 * dtConfiguration[0] => Pin 10 gehört zur Gruppe <GroupNumber>
 * dtConfiguration[1] => Pin 11 gehört zur Gruppe <GroupNumber>
 * dtConfiguration[2] => Pin 12 gehört zur Gruppe <GroupNumber>
 * dtConfiguration[3] => Pin 13 gehört zur Gruppe <GroupNumber>
 * dtConfiguration[4] => Pin 14 gehört zur Gruppe <GroupNumber>
 * dtConfiguration[5] => Pin 15 gehört zur Gruppe <GroupNumber>
 * dtConfiguration[6] => Pin 16 gehört zur Gruppe <GroupNumber>
 * dtConfiguration[7] => Pin 17 gehört zur Gruppe <GroupNumber>
 * dtConfiguration[8,9] => BlinkOnEvtTime
 * dtConfiguration[10,11] => BlinkOnIdleTime
 * dtConfiguration[12,13] => BlinkOffTime
 **/

REGISTER regConfiguration(dtConfiguration,
                            sizeof(dtConfiguration),
                            &GetConfiguration,
                            &SetConfiguration);
// Binary input register
REGISTER regBinInputs(dtBinInputs, sizeof(dtBinInputs), NULL, NULL);

// GroupStates 
REGISTER regGroupStates(group_state, sizeof(group_state), &UpdateGroupState, NULL);

/**
 * Initialize table of registers
 */
DECLARE_REGISTERS_START()
  &regConfiguration,                  // Register ID = 11 (0x0B)
  &regBinInputs,                      // Register ID = 12 (0x0C)
  &regGroupStates                     // Register ID = 13 (0x0D)
DECLARE_REGISTERS_END()

/**
 * Definition of common getter/setter callback functions
 */
DEFINE_COMMON_CALLBACKS()

/**
 * Definition of custom getter/setter callback functions
 */

 /**
 * GetConfiguration
 *
 * Collect ConfigurationParameters in Register
 *
 * 'rId'         Register ID
 * 'value'       New value
 */
const void GetConfiguration(byte rId) {
  // Update register
  for(unsigned int i=0;i<=7;i++) { 
    dtConfiguration[i] = group_assignment[i];
  }

  dtConfiguration[8]  = (LED_BlinkOnEvtTime >> 0x08) & 0xFF;
  dtConfiguration[9]  =  LED_BlinkOnEvtTime & 0xFF;

  dtConfiguration[10] = (LED_BlinkOnIdleTime >> 0x08) & 0xFF;
  dtConfiguration[11] =  LED_BlinkOnIdleTime & 0xFF;

  dtConfiguration[12] = (LED_BlinkOffTime >> 0x08) & 0xFF;
  dtConfiguration[13] =  LED_BlinkOffTime & 0xFF;
} 


void getVarsFromEeprom() {
  uint8_t myVal = 0;

  //initialization
  for(unsigned int i=0;i<8;i++) {
    group_count[i]      = 0;
    group_assignment[i] = 0;
  }
     
  // getting group definitions
  // INFOMEM_SECTION_B: pos 0 bis INFOMEM_SECTION_B: pos 7
  for(unsigned int i=0;i<=7;i++) { 
    nvMem.read(&MemoryValue[0], INFOMEM_SECTION_B, 13-i, sizeof(MemoryValue[0]));
    myVal = (int)MemoryValue[0] & 0x07; // Gruppennummer, nur die ersten 3 Bit -> nur Range 0-7 ist valid
    //myVal = MemoryValue[0];
    group_assignment[i] = myVal;
    group_count[myVal]++;

    debug("Pin 1", false);
    debug_hex(i, false);
    debug("belongs to Group ", false);
    debug_hex(myVal, true);
  } 

  nvMem.read(&MemoryValue[0], INFOMEM_SECTION_B, 4, sizeof(MemoryValue[0]));
  nvMem.read(&MemoryValue[1], INFOMEM_SECTION_B, 5, sizeof(MemoryValue[1]));
  LED_BlinkOnEvtTime = (MemoryValue[0] << 8) | MemoryValue[1];
  
  debug("LED_BlinkOnEvtTime = ", false);
  debug_hex(LED_BlinkOnEvtTime, true);
  
  nvMem.read(&MemoryValue[0], INFOMEM_SECTION_B, 2, sizeof(MemoryValue[0]));
  nvMem.read(&MemoryValue[1], INFOMEM_SECTION_B, 3, sizeof(MemoryValue[1]));
  LED_BlinkOnIdleTime = (MemoryValue[0] << 8) | MemoryValue[1];
  debug("LED_BlinkOnIdleTime = ", false);
  debug_hex(LED_BlinkOnIdleTime, true);
  
  nvMem.read(&MemoryValue[0], INFOMEM_SECTION_B, 0, sizeof(MemoryValue[0]));
  nvMem.read(&MemoryValue[1], INFOMEM_SECTION_B, 1, sizeof(MemoryValue[1]));
  LED_BlinkOffTime = (MemoryValue[0] << 8) | MemoryValue[1];
  debug("LED_BlinkOffTime = ", false);
  debug_hex(LED_BlinkOffTime, true);
}

const void UpdateGroupState(byte rID) {
  // Update GroupStates for TX
  uint8_t group_sum[8]; // group_sum[GroupNumber] = Summe der Stati
  
  //initialization
  for(unsigned int i=0;i<8;i++) {
    group_state[i] = 0;
    group_sum[i] = 0;
  }
  
  // ueber alle Pins
  group_sum[group_assignment[0]] += (dtBinInputs[0] & 0x01);
  group_sum[group_assignment[1]] += (dtBinInputs[0] & 0x02) >> 0x01 ;
  group_sum[group_assignment[2]] += (dtBinInputs[0] & 0x04) >> 0x02 ;
  group_sum[group_assignment[3]] += (dtBinInputs[0] & 0x08) >> 0x03 ;
  group_sum[group_assignment[4]] += (dtBinInputs[0] & 0x10) >> 0x04 ;
  group_sum[group_assignment[5]] += (dtBinInputs[0] & 0x20) >> 0x05 ;
  group_sum[group_assignment[6]] += (dtBinInputs[0] & 0x40) >> 0x06 ;
  group_sum[group_assignment[7]] += (dtBinInputs[0] & 0x80) >> 0x07 ;

  for(unsigned int i=0;i<=7;i++) {
    if(group_sum[i] == 0) group_state[i] = 0; // closed
    else if(group_sum[i] == group_count[i]) group_state[i] = 2; // opened
    else group_state[i] = 1; // tilted
  }

  debug("Binary State: ", false);
  debug_bin(dtBinInputs[0],true);

  debug("Status Group Assignment: ", false);
  debug_hex(group_assignment[7],false);
  debug_hex(group_assignment[6],false);
  debug_hex(group_assignment[5],false);
  debug_hex(group_assignment[4],false);
  debug_hex(group_assignment[3],false);
  debug_hex(group_assignment[2],false);
  debug_hex(group_assignment[1],false);
  debug_hex(group_assignment[0],true);
  
  debug("Status Group_Count: ", false);
  debug_hex(group_count[7],false);
  debug_hex(group_count[6],false);
  debug_hex(group_count[5],false);
  debug_hex(group_count[4],false);
  debug_hex(group_count[3],false);
  debug_hex(group_count[2],false);
  debug_hex(group_count[1],false);
  debug_hex(group_count[0],true);
  
  debug("Status Group_Sum: ", false);
  debug_hex(group_sum[7],false);
  debug_hex(group_sum[6],false);
  debug_hex(group_sum[5],false);
  debug_hex(group_sum[4],false);
  debug_hex(group_sum[3],false);
  debug_hex(group_sum[2],false);
  debug_hex(group_sum[1],false);
  debug_hex(group_sum[0],true);
  
  debug("Status Group_State: ", false);
  debug_hex(group_state[7],false);
  debug_hex(group_state[6],false);
  debug_hex(group_state[5],false);
  debug_hex(group_state[4],false);
  debug_hex(group_state[3],false);
  debug_hex(group_state[2],false);
  debug_hex(group_state[1],false);
  debug_hex(group_state[0],true);
  
}

/**
 * setConfiguration
 *
 * Set Confuguration and store in EEPROM
 *
 * 'rId'         Register ID
 * 'value'       New value
 */
const void SetConfiguration(byte rId, byte *value) {
  // Update register
  debug("Configuration received", true);
  if ((value[0] != regTable[rId]->value[0]) ||            \
      (value[1] != regTable[rId]->value[1]) ||            \
      (value[2] != regTable[rId]->value[2]) ||            \
      (value[3] != regTable[rId]->value[3]) ||            \
      (value[4] != regTable[rId]->value[4]) ||            \
      (value[5] != regTable[rId]->value[5]) ||            \
      (value[6] != regTable[rId]->value[6]) ||            \
      (value[7] != regTable[rId]->value[7]) ||            \
      (value[8] != regTable[rId]->value[8]) ||            \
      (value[9] != regTable[rId]->value[9]) ||            \
      (value[10] != regTable[rId]->value[10]) ||            \
      (value[11] != regTable[rId]->value[11]) ||            \
      (value[12] != regTable[rId]->value[12]) ||            \
      (value[13] != regTable[rId]->value[13]))              \
  {
    debug("Configuration writing ....", true);
 
    // write into info/eeprom memory
    for(unsigned int i=0;i<=13;i++) { 
      MemoryValue[0] = value[i];
      // INFOMEM_SECTION_B: pos 0 bis INFOMEM_SECTION_B: pos 7
      nvMem.write(&MemoryValue[0], INFOMEM_SECTION_B, i, sizeof(MemoryValue[0]));

      debug("Loop ", false);
      debug_dec(i, false);
      debug(": ", false);
      debug_hex(MemoryValue[0], false);
      debug(" written", false);

      nvMem.read(&MemoryValue[1], INFOMEM_SECTION_B, i, sizeof(MemoryValue[1]));
      debug("  -->  ", false);
      debug("read: ", false);
      debug_hex(MemoryValue[1], true);
    }
    // neue Parameter in die Variablen laden
    getVarsFromEeprom();

    // Transmit new Configuration 
    send_config = true;
  }
}
