/*
 * Copyright 2019 TU Braunschweig, Institute of Computer and Network Engineering
 * Contributors: Kai-Björn Gemlau (gemlau@ida.ing.tu-bs.de)
 *               Phil Hertha
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its 
 * contributors may be used to endorse or promote products derived 
 * from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
*/

#include "sllet.h"

/**
 * TX Configuration for P1: 5ms TX part of interconnect task
 */
#define SLLET_PORT_5ms	1000
#define SLLET_PORT_2ms	1100

#define SLLET_IP_5ms	IPADDR4_INIT_BYTES(192,168,178,10)
#define SLLET_IP_2ms	IPADDR4_INIT_BYTES(192,168,178,10)

#define SLLET_LABEL_COUNT_5ms	1
#define SLLET_LABEL_COUNT_2ms	1

/* Network: T1: 5ms, BCRT: 0ms, WCRT: 6ms, Copy-In Phase 0,2ms, Sync-Err=0,2ms -> N= 3 buffer */
#define SLLET_BUFFER_SIZE_5ms	3

/* Network: T2: 2ms, BCRT: 0ms, WCRT: 7ms, Copy-In Phase 0,1ms, Sync-Err=0,1ms -> N= 5 buffer */
#define SLLET_BUFFER_SIZE_2ms	5

const SLLET_INTERCONNTECT_INPUT_LABEL sllet_5ms_copy_list[] = {
		/* Task ID, Label ID */
		{.task_id= TASK_5MS_ID,	.label_id = 0}
};

const SLLET_INTERCONNTECT_INPUT_LABEL sllet_2ms_copy_list[] = {
		/* Task ID, Label ID */
		{.task_id= TASK_2MS_ID,	.label_id = 0}
};

#define TASK_1ms_READ_MAP_SIZE	10
const uint16_t task_1ms_readMap[] = {1,1,2,2,3,3,4,4,0,0};

#define TASK_10ms_READ_MAP_SIZE 3
//const uint16_t task_10ms_readMap[] = {1,0,2};
const uint16_t task_10ms_readMap[] = {0,2,1};

SLLET_DECLARE_TX(5ms)

SLLET_DECLARE_TX(2ms)



/**
 * RX Configuration for P1: sporadic RX part of 5ms interconnect task
 */
SLLET_DECLARE_RX_BUFFER(5ms, SLLET_HEADER_SIZE + SLLET_LABEL_COUNT_5ms);
SLLET_DECLARE_SLLET_BUFFER(5ms,SLLET_BUFFER_SIZE_5ms*SLLET_LABEL_COUNT_5ms);

SLLET_BUFFER_CONFIG sllet_5ms_rx_buffer_cfg = {
	SLLET_BUFFER_SIZE_5ms,			/* Number of entries */
	SLLET_LABEL_COUNT_5ms,			/* Number of labels */
	SLLET_SLLET_BUFFER(5ms)			/* Buffer base address */
};

SLLET_INTERCONNECT_RX_CONFIG sllet_5ms_rx_cfg = {
	SLLET_PORT_5ms,				/* UDP Port */
	SLLET_RX_BUFFER(5ms),
	&sllet_5ms_rx_buffer_cfg,		/* Pointer to buffer config */
};

/**
 * RX Configuration for P2: sporadic RX part of 2ms interconnect task
 */
SLLET_DECLARE_RX_BUFFER(2ms, SLLET_HEADER_SIZE + SLLET_LABEL_COUNT_2ms);
SLLET_DECLARE_SLLET_BUFFER(2ms, SLLET_BUFFER_SIZE_2ms*SLLET_LABEL_COUNT_2ms);


SLLET_BUFFER_CONFIG sllet_2ms_rx_buffer_cfg = {
	SLLET_BUFFER_SIZE_2ms,			/* Number of entries */
	SLLET_LABEL_COUNT_2ms,			/* Number of labels */
	SLLET_SLLET_BUFFER(2ms)			/* Buffer base address */
};

SLLET_INTERCONNECT_RX_CONFIG sllet_2ms_rx_cfg = {
		SLLET_PORT_2ms,				/* UDP Port */
		SLLET_RX_BUFFER(2ms),
		&sllet_2ms_rx_buffer_cfg,		/* Pointer to buffer config */
};


/* Task 1ms read config (Reads from 2ms SL-LET Task) */
SLLET_CONSUMER_CONFIG task_1ms_cfg = {
	&sllet_2ms_rx_buffer_cfg,					/* Pointer to buffer config */
	TASK_1ms_READ_MAP_SIZE,						/* Read map size			*/
	task_1ms_readMap							/* Read Map for 1ms task    */
};

/* Task 10ms read config (Reads from 5ms SL-LET Task) */
SLLET_CONSUMER_CONFIG task_10ms_cfg = {
	&sllet_5ms_rx_buffer_cfg,					/* Pointer to buffer config */
	TASK_10ms_READ_MAP_SIZE,					/* Read map size			*/
	task_10ms_readMap							/* Read Map for 1ms task    */
};


