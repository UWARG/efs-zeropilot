/**
 * The following file contains useful dma abstractions that help with
 * re-usability in the DMA implementations
 *
 * This header was made as a C header and generic purposefully so we can use it in
 * stm32fxxx_it.h
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct DMAConfig {
	void *dma_handle;
	volatile bool reset; //toggle wether we need to reset DMA lines
	volatile bool idle_line;
	volatile size_t prev_pos; //prev position in dma buffer
	uint8_t *dma_buffer; //must be dynamically allocated
	size_t dma_buffer_len;
	volatile uint32_t timer;
	uint32_t timeout;
	void *queue; //queue to offload the dma buffer onto. Should be pointer to std::deque<uint8_t>
	void (*complete_callback)();
} DMAConfig;

/**
 * Resets dma configuration.
 * @param config
 * @param dma_buffer_len
 * 	of the queue
 */
void resetDMAConfig(DMAConfig *config, size_t dma_buffer_len);

/**
 * Dumps the contents of the dma buffer into the FIFO queue, taking into account
 * if the complete event triggered early due to an idle line or stop bit
 * @param config
 */
void processDMARXCompleteEvent(DMAConfig *config);

#ifdef __cplusplus
}
#endif
