/*
 * ethernet_interface.h
 *
 *  Created on: Sep 12, 2025
 *      Author: TK
 */

#ifndef APP_INC_ETHERNET_INTERFACE_H_
#define APP_INC_ETHERNET_INTERFACE_H_

#include "gpio.h"
#include "spi_task.h"

#include "W5500/w5500.h"
#include "socket.h"

/**
 * @brief Function pointer type for queuing SPI requests
 * @param requestFunction Function to be executed in SPI task context
 * @param data Parameter to pass to the request function
 */
typedef void (*spi_queue_request) (Fp_SPI_Queue_Request requestFunction, void* data);

/**
 * @brief Function pointer type for queuing SPI requests from ISR context
 * @param requestFunction Function to be executed in SPI task context
 * @param data Parameter to pass to the request function
 */
typedef void (*spi_queue_request_fromISR)(Fp_SPI_Queue_Request requestFunction, void* data);

/**
 * @brief Handle for managing data buffers
 */
typedef struct {
	uint8_t* buffer;  /**< Pointer to buffer memory */
	uint16_t size;    /**< Size of buffer in bytes */
} BufferHandle_t;

/**
 * @brief Internal ethernet interface handle containing SPI queue functions
 */
typedef struct ethernet_interface_handle {
  spi_queue_request         spiQueueRequest;         /**< Queue request from normal context */
  spi_queue_request_fromISR spiQueueRequest_fromISR; /**< Queue request from ISR context */
} EthernetInterface_t;

/**
 * @brief Supported ethernet protocols
 */
typedef enum EthernetProtocol {
  TCP    = Sn_MR_TCP,    /**< TCP protocol */
  UDP    = Sn_MR_UDP,    /**< UDP protocol */
  MACRAW = Sn_MR_MACRAW  /**< Raw MAC protocol */
} EthernetProtocol_t;

/**
 * @brief Socket handle containing all socket-related information
 */
typedef struct {
  EthernetProtocol_t  protocol;         /**< Protocol used by this socket */
  uint16_t            port;             /**< Port number */
  uint8_t             flag;             /**< Socket flags */
  uint8_t             socket_id;        /**< Socket number (0-7) */
  BufferHandle_t      RX_BUFFER;        /**< Receive buffer */
  BufferHandle_t      TX_BUFFER;        /**< Transmit buffer (currently unused) */
  void*               SocketCallbackFP; /**< Callback function pointer */
  bool                inUse;            /**< Socket in use flag */
} SocketHandle_t;

/**
 * @brief Socket events passed to callback functions
 */
typedef enum SocketEvent {
  SE_CONNECTED,    /**< Client connected */
  SE_DISCONNECTED, /**< Client disconnected */
  SE_RX,           /**< Data received */
  SE_TX_COMPLETE,  /**< Data transmission complete */
  SE_TIMEOUT,      /**< Operation timeout */
  SE_ERROR,        /**< Error occurred */
  NUM_EVENTS       /**< Total number of events */
} SocketEvent_t;

/**
 * @brief Callback function type for socket events
 * @param socket Socket that triggered the event
 * @param event Type of event that occurred
 * @param details Additional event details (currently unused)
 */
typedef void (*SocketCallbackFunction) (SocketHandle_t socket, SocketEvent_t event, void* details);

/* ========== Initialization ========== */

/**
 * @brief Initialize the ethernet interface
 *
 * Sets up SPI queue functions, registers interrupt callback, loads default
 * configuration, and partitions RX/TX buffers for all sockets.
 */
void Ethernet_Init();

/**
 * @brief Initialize and open a port with specified protocol
 * @param port Port number to listen on
 * @param protocol Protocol type (TCP/UDP/MACRAW)
 * @param callback Function to call on socket events
 *
 * Finds a free socket and opens it with the given parameters.
 */
void Ethernet_initPort(uint16_t port, EthernetProtocol_t protocol, SocketCallbackFunction callback);

/* ========== Callbacks ========== */

/**
 * @brief GPIO interrupt callback for W5500 interrupt line
 * @param ID Source ID
 * @param DI Digital input that triggered
 * @param edge Trigger edge (rising/falling)
 *
 * Queues the actual interrupt handler in SPI task context.
 */
void Ethernet_IRQ_Callback(uint8_t ID, GPIO_DI_TypeDef DI, PinTriggerEdge_TypeDef edge);

/**
 * @brief Main interrupt handler executed in SPI task context
 * @param pData Unused parameter
 *
 * Processes all socket interrupts and calls registered callbacks.
 */
void __IRQ_Callback_CB(void* pData);

/* ========== Socket Management ========== */

/**
 * @brief Open a socket with specified parameters
 * @param sockNum Socket number (0-7)
 * @param protocol Protocol type (TCP/UDP/MACRAW)
 * @param port Port number
 * @param flag Socket flags
 *
 * Queues socket opening in SPI task. For TCP sockets, also starts listening.
 */
void Ethernet_openSocket(uint8_t sockNum, uint8_t protocol, uint16_t port, uint8_t flag);

/**
 * @brief SPI task callback for opening socket
 * @param pData Socket number as void pointer
 */
void __openSocket_CB(void* pData);

/**
 * @brief Close a socket
 * @param sockNum Socket number to close
 *
 * Marks socket as not in use and queues close operation in SPI task.
 */
void Ethernet_closeSocket(uint8_t sockNum);

/**
 * @brief SPI task callback for closing socket
 * @param pData Socket number as void pointer
 */
void __closeSocket_CB(void* pData);

/* ========== Data Transfer ========== */

/**
 * @brief Send data over a socket
 * @param sockNum Socket number
 * @param tx_buffer Data to send
 * @param len Length of data in bytes
 *
 * For TCP: Uses send()
 * For UDP: Uses sendto() with configured target IP/port or broadcast for debug
 */
void Ethernet_send(uint8_t sockNum, uint8_t* tx_buffer, uint16_t len);

/**
 * @brief Receive data from a socket
 * @param sockNum Socket number
 * @param rx_buffer Buffer to store received data
 * @param len Maximum length to receive
 * @return Number of bytes received, 0 on error
 */
int32_t Ethernet_receive(uint8_t sockNum, uint8_t* rx_buffer, uint16_t len);

/* ========== Buffer Management ========== */

/**
 * @brief Reset buffer contents to zero
 * @param buffer Buffer handle to reset
 */
void Ethernet_resetBuffer(BufferHandle_t* rx_buffer);

/**
 * @brief Set buffer size (currently not implemented)
 * @param buffer Buffer handle
 * @param size New size in bytes
 */
void Ethernet_setBufferSize(BufferHandle_t* buffer, uint16_t size);

/* ========== Network Configuration ========== */

/**
 * @brief Set network information (IP, subnet, gateway, etc.)
 * @param netInfo Network configuration structure
 */
void Ethernet_setNetworkInfo(wiz_NetInfo* netInfo);

/**
 * @brief Get current network information
 * @return Pointer to network configuration structure
 */
wiz_NetInfo* Ethernet_getNetworkInfo();

/* ========== Helper Functions ========== */

/**
 * @brief Find first available free socket
 * @return Socket number (0-7), or -1 if all sockets in use
 */
uint8_t __Ethernet_getFreeSocket();

#endif /* APP_INC_ETHERNET_INTERFACE_H_ */
