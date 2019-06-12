/**
 * DMA implementation uses UART Idle line detection to support variable size reads. More info can be found
 * on: https://github.com/akospasztor/stm32-dma-uart
 * Which is what this code is based off
 *
 * For the Autopilot chip, only UARTs 2, 3, 4 are DMA capable! Only DMA receive is implemented. Note that UART 3 and 4
 * are not capable of CTS and RTS!
 */
#include "UART.hpp"
#include "GPIO.hpp"
#include "DMA.hpp"
#include "Status.hpp"
#include "Debug.hpp"
#include "stm32f7xx_hal.h"
#include <stdlib.h>
#include <queue>

typedef struct UARTPinSettings {
  GPIOPort tx_port;
  GPIOPort rx_port;
  GPIOPort cts_port;
  GPIOPort rts_port;
  GPIOPinNum tx_pin;
  GPIOPinNum rx_pin;
  GPIOPinNum cts_pin;
  GPIOPinNum rts_pin;
} UARTPinSettings;

static UARTPinSettings PIN_SETTINGS[4] = {
    { GPIO_PORT_A, GPIO_PORT_A, GPIO_PORT_A, GPIO_PORT_A, 9, 10, 11, 12},
    { GPIO_PORT_A, GPIO_PORT_A, GPIO_PORT_A, GPIO_PORT_A, 36, 37, 34, 35},
    { GPIO_PORT_D, GPIO_PORT_D, GPIO_PORT_D, GPIO_PORT_D, 8, 9, 0, 0},
    { GPIO_PORT_D, GPIO_PORT_D, GPIO_PORT_D, GPIO_PORT_D,  0, 1, 0, 0}
};

//no dma on uart1 (checked on STM32CUbeMX)
static UART_HandleTypeDef huart1;

//uart 4 defines for enabling DMA
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
static std::deque<uint8_t> uart2_rx_queue;
DMAConfig uart2_dma_config;

//uart 4 defines for enabling DMA
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;
static std::deque<uint8_t> uart3_rx_queue;
DMAConfig uart3_dma_config;

//uart 4 defines for enabling DMA
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_usart4_rx;
static std::deque<uint8_t> uart4_rx_queue;
DMAConfig uart4_dma_config;

extern StatusCode get_status_code(HAL_StatusTypeDef status);
void USART2_DMA_ErrorCallback(DMA_HandleTypeDef *dma);

bool UARTPort::is_valid_port() {
  return port == UART_PORT1 || port == UART_PORT2 || port == UART_PORT3 || port == UART_PORT4;
}

UARTPort::UARTPort(UARTPortNum port, UARTSettings settings) {
  this->port = port;
  this->settings = settings;

  switch (port) {
    case UART_PORT1:
      rx_pin = GPIOPin(PIN_SETTINGS[0].rx_port,
                       PIN_SETTINGS[0].rx_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                       GPIO_FREQ_HIGH,
                       GPIO_AF7_USART1);
      tx_pin = GPIOPin(PIN_SETTINGS[0].tx_port,
                       PIN_SETTINGS[0].tx_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                       GPIO_FREQ_HIGH,
                       GPIO_AF7_USART1);
      cts_pin = GPIOPin(PIN_SETTINGS[0].cts_port,
                       PIN_SETTINGS[0].cts_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                        GPIO_RES_NONE,
                        GPIO_FREQ_HIGH,
                       GPIO_AF7_USART1);
      rts_pin = GPIOPin(PIN_SETTINGS[0].rts_port,
                       PIN_SETTINGS[0].rts_port,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                        GPIO_RES_NONE,
                        GPIO_FREQ_HIGH,
                       GPIO_AF7_USART1);
      huart1.Instance = USART1;
      this->interface_handle = &huart1;
      __HAL_RCC_USART1_CLK_ENABLE();
      break;
    case UART_PORT2:
      rx_pin = GPIOPin(PIN_SETTINGS[1].rx_port,
                       PIN_SETTINGS[1].rx_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                       GPIO_FREQ_HIGH,
                       GPIO_AF7_USART2);
      tx_pin = GPIOPin(PIN_SETTINGS[1].tx_port,
                       PIN_SETTINGS[1].tx_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                       GPIO_FREQ_HIGH,
                       GPIO_AF7_USART2);
      cts_pin = GPIOPin(PIN_SETTINGS[1].cts_port,
                       PIN_SETTINGS[1].cts_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                        GPIO_FREQ_HIGH,
                       GPIO_AF7_USART2);
      rts_pin = GPIOPin(PIN_SETTINGS[1].rts_port,
                       PIN_SETTINGS[1].rts_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                        GPIO_FREQ_HIGH,
                       GPIO_AF7_USART2);
      huart2.Instance = USART2;
      this->interface_handle = &huart2;
      __HAL_RCC_USART2_CLK_ENABLE();
      break;
    case UART_PORT3:
      rx_pin = GPIOPin(PIN_SETTINGS[2].rx_port,
                       PIN_SETTINGS[2].rx_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                       GPIO_FREQ_HIGH,
                       GPIO_AF7_USART3);
      tx_pin = GPIOPin(PIN_SETTINGS[2].tx_port,
                       PIN_SETTINGS[2].tx_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                       GPIO_FREQ_HIGH,
                       GPIO_AF7_USART3);
      huart3.Instance = USART3;
      this->interface_handle = &huart3;
      this->settings.cts_rts = false; //uart 3 not capable of cts/rts
      __HAL_RCC_USART3_CLK_ENABLE();
      break;
    case UART_PORT4:
      rx_pin = GPIOPin(PIN_SETTINGS[3].rx_port,
                       PIN_SETTINGS[3].rx_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                       GPIO_FREQ_HIGH,
                       GPIO_AF8_UART4);
      tx_pin = GPIOPin(PIN_SETTINGS[3].tx_port,
                       PIN_SETTINGS[3].tx_pin,
                       GPIO_ALT_PP,
                       GPIO_STATE_LOW,
                       GPIO_RES_NONE,
                       GPIO_FREQ_HIGH,
                       GPIO_AF8_UART4);
      huart4.Instance = UART4;
      this->interface_handle = &huart4;
      this->settings.cts_rts = false; //uart 4 not capable of cts/rts
      __HAL_RCC_UART4_CLK_ENABLE();
      break;
    default: rx_pin = GPIOPin();
      tx_pin = GPIOPin();
      rts_pin = GPIOPin();
      cts_pin = GPIOPin();
      break;
  }
}

StatusCode UARTPort::reset() {
  if (!is_setup) return STATUS_CODE_INVALID_ARGS;

  auto uart = (UART_HandleTypeDef *) this->interface_handle;

  if (dma_setup_rx || dma_setup_tx) {
    resetDMA();
  }

  switch (port) {
    case UART_PORT1: __HAL_RCC_USART1_CLK_DISABLE();
      break;
    case UART_PORT2: __HAL_RCC_USART2_CLK_DISABLE();
      break;
    case UART_PORT3: __HAL_RCC_USART3_CLK_DISABLE();
      break;
    case UART_PORT4: __HAL_RCC_UART4_CLK_DISABLE();
      break;
    default: return STATUS_CODE_INVALID_ARGS;
  }

  StatusCode status = get_status_code(HAL_UART_DeInit(uart));
  if (status != STATUS_CODE_OK) return status;

  status = rx_pin.reset();
  if (status != STATUS_CODE_OK) return status;

  status = tx_pin.reset();
  if (status != STATUS_CODE_OK) return status;

  if (settings.cts_rts){
    status = cts_pin.reset();
    if (status != STATUS_CODE_OK) return status;

    status = rts_pin.reset();
    if (status != STATUS_CODE_OK) return status;
  }

  is_setup = false;
  return STATUS_CODE_OK;
}

StatusCode UARTPort::setupDMA(size_t tx_buffer_size, size_t rx_buffer_size) {
  if (!is_setup) return STATUS_CODE_UNINITIALIZED;
  if (dma_setup_rx || rx_buffer_size == 0) return STATUS_CODE_INVALID_ARGS;

  if (port == UART_PORT2) {

    dma_config = &uart2_dma_config;
    resetDMAConfig(dma_config, rx_buffer_size);
    dma_config->dma_handle = (void *) &hdma_usart2_rx;
    dma_config->queue = (void *) &uart2_rx_queue;
    dma_config->timeout = 50;
    this->rx_queue = &uart2_rx_queue;

    auto dma_handle = (DMA_HandleTypeDef *) dma_config->dma_handle;
    auto uart_handle = (UART_HandleTypeDef *) this->interface_handle;

    __HAL_RCC_DMA1_CLK_ENABLE();

    dma_handle->Instance = DMA1_Stream5;
    dma_handle->Init.Channel = DMA_CHANNEL_4;
    dma_handle->Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_handle->Init.PeriphInc = DMA_PINC_DISABLE;
    dma_handle->Init.MemInc = DMA_MINC_ENABLE;
    dma_handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    dma_handle->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_handle->Init.Mode = DMA_CIRCULAR;
    dma_handle->Init.Priority = DMA_PRIORITY_HIGH;
    dma_handle->Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    StatusCode status = get_status_code(HAL_DMA_Init(dma_handle));
    if (status != STATUS_CODE_OK) return status;

    __HAL_LINKDMA(uart_handle, hdmarx, (*dma_handle));

    //don't allocate memory if we hadn't free'd it
    if (dma_config->dma_buffer == nullptr) {
      dma_config->dma_buffer = (uint8_t *) malloc(dma_config->dma_buffer_len * sizeof(uint8_t));
    }

    if (dma_config->dma_buffer == nullptr) {
      return STATUS_CODE_RESOURCE_EXHAUSTED;
    }

    //init circular dma transfer
    status =
        get_status_code(HAL_UART_Receive_DMA(uart_handle,
                                             dma_config->dma_buffer,
                                             (uint16_t) dma_config->dma_buffer_len));
    if (status != STATUS_CODE_OK) return status;

    //disable the DMA half interrupt since we're using idle line detection instead
    __HAL_DMA_DISABLE_IT(dma_handle, DMA_IT_HT);
    HAL_DMA_RegisterCallback(dma_handle, HAL_DMA_XFER_ERROR_CB_ID, USART2_DMA_ErrorCallback);

    //enable uart idle line interrupt
    SET_BIT(uart_handle->Instance->CR1, USART_CR1_IDLEIE);

    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    dma_setup_rx = true;

    return STATUS_CODE_OK;
  }

  return STATUS_CODE_UNIMPLEMENTED; //only port 2 supported for stm32f0
}

StatusCode UARTPort::resetDMA() {
  if (port != UART_PORT2) return STATUS_CODE_UNIMPLEMENTED;
  if (!dma_setup_rx) return STATUS_CODE_INVALID_ARGS;

  auto dma_handle = (DMA_HandleTypeDef *) dma_config->dma_handle;
  auto uart_handle = (UART_HandleTypeDef *) this->interface_handle;

  //disable dma interrupts
  CLEAR_BIT(uart_handle->Instance->CR1, USART_CR1_IDLEIE);
  HAL_NVIC_DisableIRQ(USART2_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Stream5_IRQn);

  StatusCode status;
  status = get_status_code(HAL_DMA_DeInit(dma_handle));
  if (status != STATUS_CODE_OK) return status;

  //avoid uneccessary re-allocations in case of dma error
  if (reallocate_dma_buffer) {
    if (dma_config->dma_buffer == nullptr) abort("uart dma buffer null!", __FILE__, __LINE__);
    free(dma_config->dma_buffer);
    dma_config->dma_buffer = nullptr;
    rx_queue->erase(rx_queue->begin(), rx_queue->end());
  }

  dma_setup_rx = false;

  return STATUS_CODE_OK;
}

//Below code based off: https://github.com/akospasztor/stm32-dma-uart/blob/master/Src/main.c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART2) {
    processDMARXCompleteEvent(&uart2_dma_config);
  }
}
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
  error("half called");
  UNUSED(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    error("USART1 Error received", huart->ErrorCode);
  } else if (huart->Instance == USART2) {
    error("USART2 Error received", huart->ErrorCode);
    uart2_dma_config.reset = true;
  } else {
    error("Unknown USART port got an error callback");
  }
}

void USART2_DMA_ErrorCallback(DMA_HandleTypeDef *dma) {
  error("USART 2 DMA Error received!", dma->ErrorCode);
}

void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    error("USART1 Abort received");
  }
  if (huart->Instance == USART2) {
    error("USART2 Abort received");
  }
}

