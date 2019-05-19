#include "DMA.hpp"
#include "Debug.hpp"
#include <deque>

#if STM32F030xC
#include "stm32f0xx_hal.h"
#elif STM32F7xx
#include "stm32f7xx_hal.h"
#endif

void resetDMAConfig(DMAConfig *config, size_t dma_buffer_len) {
	config->reset = false;
	config->timeout = 50;
	config->timer = 0;
	config->dma_buffer_len = dma_buffer_len;
	config->prev_pos = config->dma_buffer_len;
	config->idle_line = false;
}

void processDMARXCompleteEvent(DMAConfig *config) {
	if (config->dma_buffer == nullptr || config->queue == nullptr || config->dma_handle == nullptr) {
		abort("processDMARXCompleteEvent fatal error", __FILE__, __LINE__);
		return;
	}

	auto dma_handle = static_cast<DMA_HandleTypeDef *>(config->dma_handle);
	auto queue = static_cast<std::deque<uint8_t> *>(config->queue);

	size_t i, pos, start, length;

	//number of remaining data bytes in the DMA buffer (before complete event is triggered)
	uint32_t currCNDTR = __HAL_DMA_GET_COUNTER(dma_handle);

	/* Ignore IDLE Timeout when the received characters exactly filled up the DMA buffer and DMA Rx Complete IT is generated, but there is no new character during timeout */
	if (config->idle_line && currCNDTR == config->dma_buffer_len) {
		config->idle_line = false;
		return;
	}

	/* Determine start position in DMA buffer based on previous CNDTR value */
	start =
		(config->prev_pos < config->dma_buffer_len) ? (config->dma_buffer_len - config->prev_pos)
													: 0;

	if (config->idle_line)    /* Timeout event */
	{
		/* Determine new data length based on previous DMA_CNDTR value:
		 *  If previous CNDTR is less than DMA buffer size: there is old data in DMA buffer (from previous timeout) that has to be ignored.
		 *  If CNDTR == DMA buffer size: entire buffer content is new and has to be processed.
		*/
		length = (config->prev_pos < config->dma_buffer_len) ? (config->prev_pos - currCNDTR) : (
			config->dma_buffer_len - currCNDTR);
		config->prev_pos = currCNDTR;
		config->idle_line = false;
	} else                /* DMA Rx Complete event */
	{
		length = config->dma_buffer_len - start;
		config->prev_pos = config->dma_buffer_len;
	}

	/* Copy and Process new data */
	for (i = 0, pos = start; i < length; ++i, ++pos) {
		queue->push_back(config->dma_buffer[pos]);
	}
}