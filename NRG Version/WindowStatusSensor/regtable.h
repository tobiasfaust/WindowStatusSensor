/**
 * regtable.h
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
 * Creation date: 01/19/2012
 */

#ifndef _REGTABLE_H
#define _REGTABLE_H

#include "Arduino.h"
#include "register.h"
#include "commonregs.h"

void getVarsFromEeprom();
const void GetConfiguration(byte rId);
const void UpdateGroupState(byte rID) ;
const void SetConfiguration(byte rId, byte *value) ;

#ifdef PANSTAMP_NRG
 // NRG 2
  #define cnt0 10
  #define cnt1 11
  #define cnt2 12
  #define cnt3 13
  #define cnt4 14
  #define cnt5 15
  #define cnt6 9
  #define cnt7 8
  
  #define LED_R 22
  #define LED_Y 21
  #define LED_G 20
#else
  #include <PinChangeInterrupt.h>
  #define INFOMEM_SECTION_B 0

  #define cnt0 3
  #define cnt1 5
  #define cnt2 6
  #define cnt3 7
  #define cnt4 8
  #define cnt5 9
  #define cnt6 A0
  #define cnt7 A1
  
  #define LED_R A2
  #define LED_Y A3
  #define LED_G A4
 
#endif

/**
 * Register indexes
 */
DEFINE_REGINDEX_START()
  REGI_CONFIGURATION,
  REGI_BININPUTS,
  REGI_GROUPSTATES
DEFINE_REGINDEX_END()

#endif
