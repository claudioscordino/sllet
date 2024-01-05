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
#include <string.h>
#include "gt_core.h"        /* Uses for generic taskset and IDA LET Machine */

/**
 * \brief: Read a label according to SL-LET by using the job id of the reader
 * \param: SLLET_CONSUMER_CONFIG *config: SL-LET Config for this conumer
 * including read-map and number of labels
 * \param: uint32_t jobId: Sequence Number of the Consumer Job
 * \param: void *data: target pointer to store the data
 * \result: void
 */
void sllet_read(SLLET_CONSUMER_CONFIG *config, uint32_t jobId, void *data){
    uint32_t offset = 0;
#ifndef SLLET_DISABLE
    /* Calculate offset with readMap and jobID */
	offset = config->slletBuffer->number_of_labels * config->readMap[jobId % config->READ_MAP_SIZE];
#endif
    /* Get address of SL-LET Buffer */
	void* addr = &config->slletBuffer->buffer[offset];
    /* Copy data */
	memcpy(data,addr,config->slletBuffer->number_of_labels*sizeof(uint32_t));
}

/**
 * \brief: Write a label according to SL-LET by using the sequence number
 * Used by the sl-let interconnect task
 * \param: SLLET_BUFFER_CONFIG *bufferCfg: Config for SL-LET Buffer
 * \param: uint32_t seqNr: Sequence Number of the transmitted label
 * \param: void *data: source buffer to copy data from
 * \result: void
 */
void sllet_write(SLLET_BUFFER_CONFIG *bufferCfg, uint32_t seqNr, void *data){
    /* Get the buffer address for writing */
	void * buffer = sllet_getWritePointer(bufferCfg,seqNr);
    /* Copy the data */
	memcpy(buffer,data,bufferCfg->number_of_labels*sizeof(uint32_t));
}

/**
 * \brief: Get the correct buffer for sllet_write()
 * \param: SLLET_BUFFER_CONFIG *bufferCfg: Config for SL-LET Buffer
 * \param: uint32_t seqNr: Sequence Number of the transmitted label
 * \result: uint32_t *: Address of the target buffer
 */
uint32_t *sllet_getWritePointer(SLLET_BUFFER_CONFIG *bufferCfg, uint32_t seqNr){
	uint32_t index = 0; 
#ifndef SLLET_DISABLE
    /* Calculate Buffer index with sequence number */
	index = bufferCfg->number_of_labels * (seqNr % bufferCfg->number_of_entries);
#endif
	return &bufferCfg->buffer[index];
}


/******* TX task implementation. Consists of init part (called once) and 
 * run part (called periodically e.g. by synchornized LET scheudler like
 * ILM (IDA LET Machine) *******/

/**
 * \brief: Initialize an instance of the interconnect task (sender part)
 * \param: void *p_arg: Used as a generic form to pass the tx config
 * to the init function
 * \result: int: 0 if socket is created successfully, -1 otherwise
 */
int sllet_interconn_tx_init(void *p_arg){
	SLLET_INTERCONNECT_TX_CONFIG *config = (SLLET_INTERCONNECT_TX_CONFIG*)p_arg;

	config->socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(config->socket < 0){
		printf("Could not create socket");
		return -1;
	}
	return 0;
}

/**
 * \brief: periodically called send function of interconnect task
 * This should be called by a globally synchronized scheduler like
 * the IDA LET Machine with PTP synchronization
 * \param: void *p_arg: Used as a generic form to pass the tx config
 * to the function
 * \result: void
 */
void sllet_interconn_tx(void *p_arg){
	SLLET_INTERCONNECT_TX_CONFIG *config = (SLLET_INTERCONNECT_TX_CONFIG*)p_arg;
	uint32_t *txBuffer = (uint32_t*)config->txBuffer;
	uint16_t txBufferIndex = 0, i = 0;
	struct sockaddr_in     servaddr;
    
    /* set target address and port according to config*/
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(config->target_udp_port);
	servaddr.sin_addr.s_addr = config->target_ip_addr;

    /* Aquire current activation number 
     * (in this case from the generic taskset) to be used
     * as a sequence number */
	uint32_t seqNr = GT_GetActivationNumber();
	txBuffer[txBufferIndex++] = seqNr;

#if SLLET_INCLUDE_SEQ_CHECK > 0
    /* Optionally send sequence number twice in header */
	txBuffer[txBufferIndex++] = seqNr;
#endif

#if SLLET_INCLUDE_TS > 0
    /* Optionally include timestamp in header
     * (in this case aquired from ptp implementation) */
	TimeInternal time;
	getTime(&time);
	txBuffer[txBufferIndex++] = time.seconds;
	txBuffer[txBufferIndex++] = time.nanoseconds;
#endif

    /* Read labels for transmission with LET */
	for(i = 0; i < config->number_of_labels; i++){
		const SLLET_INTERCONNTECT_INPUT_LABEL *label = &config->copy_list[i];
		txBuffer[txBufferIndex++] = LET_readLabel(label->task_id,label->label_id);
	}

    /* Send data to foreign timezone */
	sendto(config->socket, txBuffer, txBufferIndex * sizeof(uint32_t), 0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
}


/******* RX task implementation. This is started once and then pending 
 * on the receive socket *******/

/**
 * \brief: reception task that waits for new messages and writes them 
 * in the SL-LET buffers
 * \param: void *p_arg: Used as a generic form to pass the rx config
 * to the function
 * \result: int: returns -1 in case of error, otherwise this task does not
 * return
 */
int sllet_interconn_rx_task(void *p_arg){
	SLLET_INTERCONNECT_RX_CONFIG *config = (SLLET_INTERCONNECT_RX_CONFIG*)p_arg;
	struct sockaddr_in ra;
	int recv_data = 0;
	uint32_t recv_size = 0;
	uint32_t *rxBuffer = config->rxBuff;
	uint16_t rxBufferIndex = 0;
	uint32_t seqNr = 0;

#if SLLET_INCLUDE_SEQ_CHECK > 0
	uint32_t _seqNr = 0;
#endif

#if SLLET_INCLUDE_TS > 0
	TimeInternal txTime;
#endif

	recv_size = (config->slletBuffer->number_of_labels + SLLET_HEADER_SIZE) * sizeof(uint32_t);

    /* Create receive socket */
	int sockFd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockFd < 0){
		printf("Could not create socket\n");
		return -1;
	}

    /* Set rx address */
	memset(&ra, 0, sizeof(struct sockaddr_in));
	ra.sin_family = AF_INET;
	ra.sin_addr.s_addr = htonl(INADDR_ANY);
	ra.sin_port = htons(config->local_udp_port);

    /* Bind socket to rx port according to rx config */
	if (bind(sockFd, (struct sockaddr *)&ra, sizeof(struct sockaddr_in)) == -1)
	{
		printf("SLLET Error, Bind to Port failed\n");
		close(sockFd);
		return -1;
	}

	while(1){
        /* Receive data (should always receive recv_size bytes */
		recv_data = recv(sockFd,rxBuffer,recv_size,0);
		if(recv_data == recv_size)
		{
			rxBufferIndex = 0;
            /* Get sequence number from packet */
			seqNr = rxBuffer[rxBufferIndex++];
#if SLLET_INCLUDE_SEQ_CHECK > 0
            /* optionally do sequence number check */
			_seqNr = rxBuffer[rxBufferIndex++];
			if (seqNr != _seqNr){
				sllet_seqCheckFailedHook(seqNr, _seqNr);
			} else {
#endif

#if SLLET_INCLUDE_TS > 0
                /* Optionally call rx hook with tx timestamp */
				txTime.seconds = rxBuffer[rxBufferIndex++];
				txTime.nanoseconds = rxBuffer[rxBufferIndex++];
				sllet_rxTsHook(seqNr, txTime);
#endif
                /* write data in SL-LET buffer */
				sllet_write(config->slletBuffer,seqNr,&rxBuffer[rxBufferIndex++]);

#if SLLET_INCLUDE_SEQ_CHECK > 0
			}
#endif
		} else {
			printf("SLLET Error, Packet size does not match\n");
		}
	}
}

/**
 * Week definitions for dummy hooks
 * Can be overwritten by user defined hooks
 */
#if SLLET_INCLUDE_TS > 0
__attribute__((weak)) void sllet_rxTsHook(uint32_t seqNr, TimeInternal txTime){ (void) seqNr; (void) txTime; return; }
#endif
#if SLLET_INCLUDE_SEQ_CHECK > 0
__attribute__((weak)) void sllet_seqCheckFailedHook(uint32_t seqNr, uint32_t _seqNr){ (void) seqNr; (void) _seqNr; return; }
#endif

