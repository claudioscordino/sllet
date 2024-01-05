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

#ifndef SRC_MODULES_SLLET_INC_SLLET_H_
#define SRC_MODULES_SLLET_INC_SLLET_H_

#include "sllet_types.h"
#include "sllet_cfg.h"

/** Optionally include PTP Timestamps in SL-LET Messsages */
#if SLLET_INCLUDE_TS > 0
#include "ptpd.h"
#endif

/**
 * Calculate size of SLLET Header
 */
#if SLLET_INCLUDE_TS > 0
#define SLLET_TS_SIZE 	2
#else
#define SLLET_TS_SIZE 	0
#endif

#if SLLET_INCLUDE_SEQ_CHECK > 0
#define SLLET_SEQ_CHECK_SIZE 	1
#else
#define SLLET_SEQ_CHECK_SIZE 	0
#endif

#define SLLET_HEADER_SIZE  (1 + SLLET_TS_SIZE + SLLET_SEQ_CHECK_SIZE)

/**
 * Definitions for auto-generated variables
 */

#define SLLET_COPY_LIST(TASK)		(sllet_##TASK##_copy_list)

#define SLLET_DECLARE_TX_BUFFER(TASK, SIZE)		uint32_t sllet_##TASK##_txBuffer[SIZE];
#define SLLET_TX_BUFFER(TASK)		(sllet_##TASK##_txBuffer)

#define SLLET_DECLARE_RX_BUFFER(TASK, SIZE)		uint32_t sllet_##TASK##_rxBuffer[(SIZE)];
#define SLLET_RX_BUFFER(TASK)		(sllet_##TASK##_rxBuffer)

#define SLLET_DECLARE_SLLET_BUFFER(TASK, SIZE)		uint32_t sllet_##TASK##_slletBuffer[SIZE];
#define SLLET_SLLET_BUFFER(TASK)		(sllet_##TASK##_slletBuffer)

#define SLLET_DECLARE_TX(TASK)	\
	uint32_t sllet_##TASK##_txBuffer[SLLET_HEADER_SIZE + SLLET_LABEL_COUNT_##TASK];	\
																					\
	SLLET_INTERCONNECT_TX_CONFIG sllet_##TASK##_tx_cfg = {							\
		sllet_##TASK##_copy_list, 			/* Pointer to copy list */				\
		SLLET_LABEL_COUNT_##TASK,	 		/* Number of transmitted labels */		\
		-1,								/* socket */								\
		SLLET_PORT_##TASK, 			 	/* UDP Port */								\
		SLLET_IP_##TASK,					/* Target IP Address */					\
		SLLET_TX_BUFFER(TASK)														\
	};


/**
 * Function prototypes
 */
void sllet_read(SLLET_CONSUMER_CONFIG *config, uint32_t jobId, void *data);
void sllet_write(SLLET_BUFFER_CONFIG *bufferCfg, uint32_t seqNr, void *data);
uint32_t *sllet_getWritePointer(SLLET_BUFFER_CONFIG *bufferCfg, uint32_t seqNr);

int sllet_interconn_tx_init(void *p_arg);
void sllet_interconn_tx(void *p_arg);
int sllet_interconn_rx_task(void *p_arg);

/**
 * Hook definitions
 */
#if SLLET_INCLUDE_SEQ_CHECK > 0
void sllet_seqCheckFailedHook(uint32_t seqNr, uint32_t _seqNr);
#endif

#if SLLET_INCLUDE_TS > 0
void sllet_rxTsHook(uint32_t seqNr, TimeInternal txTime);
#endif



#endif /* SRC_MODULES_SLLET_INC_SLLET_H_ */
