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

#ifndef SRC_MODULES_SLLET_INC_SLLET_TYPES_H_
#define SRC_MODULES_SLLET_INC_SLLET_TYPES_H_

#include <stdint.h>
#include <lwip/inet.h>          /* Uses UDP Sockets of LwIP Stack */
#include <lwip/sockets.h>

/**
 *  Configuration of SL-LET Buffer 
 */
typedef struct {
	const uint16_t number_of_entries;   /**< Number of buffer entries */
	const uint16_t number_of_labels;    /**< Number of labels per entry */
	uint32_t *buffer;                   /**< Base address of buffer */
} SLLET_BUFFER_CONFIG;

/**
 * Configuration of SL-LET Consumer
 */
typedef struct {
	const SLLET_BUFFER_CONFIG * slletBuffer;    /**< Corresponding SL-LET Buffer */
	const uint16_t	READ_MAP_SIZE;              /**< Size of Readmap */
	const uint16_t *readMap;                    /**< Pointer to readmap */
} SLLET_CONSUMER_CONFIG;

/**
 * (SL-)LET Config for input label of the interconnect task
 */
typedef struct {
	const uint32_t task_id;         /**< ID of producer task */
	const uint32_t label_id;        /**< Label ID for producer task */
} SLLET_INTERCONNTECT_INPUT_LABEL;

/**
 * Configuration for Interconnect task TX Part
 */
typedef struct {
	const SLLET_INTERCONNTECT_INPUT_LABEL *copy_list;   /**< list of labels need to be copied */
	const size_t  			number_of_labels;           /**< size of label list */
	int						socket;                     /**< UDP socket used for transmission */
	const in_port_t			target_udp_port;            /**< UDP Port of target */
	const in_addr_t			target_ip_addr;             /**< IP Address of target */
	void 					*txBuffer;                  /**< Buffer to store the tx packet */
} SLLET_INTERCONNECT_TX_CONFIG;

/**
 * Configuration for Interconnect task RX Part
 */
typedef struct {
	const in_port_t			local_udp_port;             /**< UDP Port to wait for incomming packets */
	void					*rxBuff;                    /**< RX Buffer to store packets */
	SLLET_BUFFER_CONFIG		*slletBuffer;               /**< configuration of SL-LET Buffer to store payload */
} SLLET_INTERCONNECT_RX_CONFIG;

#endif /* SRC_MODULES_SLLET_INC_SLLET_TYPES_H_ */
