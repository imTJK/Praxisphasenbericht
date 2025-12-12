/*
 * ethernet_interface.c
 *
 *  Created on: Sep 12, 2025
 *      Author: TK
 */
#include "ethernet_interface.h"
#include "serial.h"
#include <stdlib.h>

extern _WIZCHIP  WIZCHIP;

static EthernetInterface_t ethernet_h = {0};
static SocketHandle_t sockets[_WIZCHIP_SOCK_NUM_] = {0};

// Static allocation for RX- and TX Buffers
// Allows for Buffer resizing without dynamic memory allocation
static uint8_t rx_buffer_pool[WIZ_MAX_BUFFER_SIZE];
static uint8_t tx_buffer_pool[WIZ_MAX_BUFFER_SIZE];

void Ethernet_Init() {
	// Check if WIZCHIP has been initialized
	if(!WIZCHIP.gen_device_h) return;

	// Select right RTOS-Queue functions depending on device handle
	bus_device_t* device_h = (bus_device_t*) WIZCHIP.gen_device_h;
	if (device_h->spi_device_handle.spi_h->Instance == SPI1) {
		ethernet_h.spiQueueRequest = SPI_QueueRequest;
		ethernet_h.spiQueueRequest_fromISR = SPI_QueueRequest_fromISR;
	}
	else if (device_h->spi_device_handle.spi_h->Instance == SPI2) {
		ethernet_h.spiQueueRequest = SPI2_QueueRequest;
		ethernet_h.spiQueueRequest_fromISR = SPI2_QueueRequest_fromISR;
	}
	else {
		// TODO Error Handling
		return;
	}

	// Register and enable the Interrupt-line
	if(GPIO_DI_Int_Reg(SID_ETHERNET, DI_W5500, falling, &Ethernet_IRQ_Callback) != GPIO_NO_ENTRY){
		GPIO_DI_Int_Enable(SID_ETHERNET, DI_W5500);
	} else {
		// TODO Error Handling
	}

	// Load default configuration
	device_h->re_configure((void*) device_h);

	/// Socket specific initializations
	// String literal to uint8_t*
	uint8_t* rx_sizes = (uint8_t[]) WIZ_RX_BUFFER_SIZES;
	uint8_t* tx_sizes = (uint8_t[]) WIZ_TX_BUFFER_SIZES;

	// Buffer partitioning
	uint16_t rx_offset = 0;
	uint16_t tx_offset = 0;

	for(uint8_t sockNum = 0; sockNum < _WIZCHIP_SOCK_NUM_; sockNum++) {
		sockets[sockNum].RX_BUFFER.buffer = &rx_buffer_pool[rx_offset];
		sockets[sockNum].RX_BUFFER.size = rx_sizes[sockNum] * 1024; // kBytes -> Bytes
		rx_offset += sockets[sockNum].RX_BUFFER.size;

		sockets[sockNum].TX_BUFFER.buffer = &tx_buffer_pool[tx_offset];
		sockets[sockNum].TX_BUFFER.size = tx_sizes[sockNum] * 1024; // kBytes -> Bytes
		tx_offset += sockets[sockNum].TX_BUFFER.size;
	}
}

void Ethernet_IRQ_Callback(uint8_t ID, GPIO_DI_TypeDef DI, PinTriggerEdge_TypeDef edge){
	ethernet_h.spiQueueRequest_fromISR(__IRQ_Callback_CB, (void*) 0);
}

void __IRQ_Callback_CB(void* pData) {
	uint8_t sn_ir;
	uint8_t sir = getSIR();

	// Iterate over the sockets
	for(uint8_t sockNum = 0; sockNum < _WIZCHIP_SOCK_NUM_; sockNum++){
		// Check if the current socket has an active interrupt
		if(!(sir & (1 << sockNum))) continue;

		// Get the interrupt flags for the socket
		sn_ir = getSn_IR(sockNum);

		// void* -> SocketCallbackFunction
		SocketCallbackFunction socket_cb = (SocketCallbackFunction)(sockets[sockNum].SocketCallbackFP);

		/// Client Disconnected
		if (sn_ir & Sn_IR_DISCON) {
			// Acknowledge disconnect
			debugEthPrintWithInfoStr(sockets[sockNum].port, sockNum, (uint8_t*) "Disconnecting...");

			// Close the socket
			Ethernet_closeSocket(sockNum);

			// Call registered callback Function
			socket_cb(sockets[sockNum], SE_DISCONNECTED, (void*) NULL);
		}

		/// Client Connected
		if (sn_ir & Sn_IR_CON) {
			// Acknowledge established connection
			debugEthPrintWithInfoStr(sockets[sockNum].port, sockNum, (uint8_t*) "New Connection established");

			// Open new socket under same port
			Ethernet_initPort(sockets[sockNum].port, sockets[sockNum].protocol, sockets[sockNum].SocketCallbackFP);

			// Call registered callback Function
			socket_cb(sockets[sockNum], SE_CONNECTED, (void*) NULL);
		}

		/// Message Received / RX-Buffer not empty
		if (sn_ir & Sn_IR_RECV) {
			// Reads W5500 Buffer
			int32_t recvLen = Ethernet_receive(sockNum, sockets[sockNum].RX_BUFFER.buffer, sockets[sockNum].RX_BUFFER.size);

			if(recvLen != 0) {
				debugEthPrintWithInfo(sockets[sockNum].port, sockNum, sockets[sockNum].RX_BUFFER.buffer, recvLen);
			}

			// Call registered callback Function
			socket_cb(sockets[sockNum], SE_RX, (void*) NULL);
		}

		/// Timeout after command was set
		if (sn_ir & Sn_IR_TIMEOUT) {
			// Call registered callback Function
			socket_cb(sockets[sockNum], SE_TIMEOUT, (void*) NULL);
		}

		/// Message sent successfully
		if (sn_ir & Sn_IR_SENDOK) {
			sn_ir &= ~(Sn_IR_SENDOK); // Dont't disable the SEND_OK interrupt for the send_command
			// Call registered callback Function
			socket_cb(sockets[sockNum], SE_TX_COMPLETE, (void*) NULL);
		}

		// Reset Interrupt Flags
		setSn_IR(sockNum, sn_ir);
	}
}


void Ethernet_initPort(uint16_t port, EthernetProtocol_t protocol, SocketCallbackFunction callback) {
	uint8_t sockNum = __Ethernet_getFreeSocket();
	sockets[sockNum].SocketCallbackFP = (void*) callback;

	Ethernet_openSocket(sockNum, protocol, port, 0);
}

// Check if invalid sockNum
void Ethernet_openSocket(uint8_t sockNum, uint8_t protocol, uint16_t port, uint8_t flag) {
	// Save socket parameters
	sockets[sockNum].protocol = protocol;
	sockets[sockNum].port = port;
	sockets[sockNum].flag = flag;
	sockets[sockNum].socket_id = sockNum;

	// Mark as in use
	sockets[sockNum].inUse = true;

	// Queue Request in SPI-Task
	ethernet_h.spiQueueRequest(__openSocket_CB, (void*) (uintptr_t) (sockNum));
}

void __openSocket_CB(void* pData) {
	uint8_t sockNum = (uint8_t) (uintptr_t) pData;
	SocketHandle_t socket_h = sockets[sockNum];

	// Open socket
	if(socket(sockNum,  socket_h.protocol,  socket_h.port,  socket_h.flag) != sockNum) {
		socket_h.inUse = false;

		// TODO Error Handling
		return;
	}

	// Listen on socket
	if(socket_h.protocol == TCP){
		if(listen(sockNum) != SOCK_OK) {
			socket_h.inUse = false;

			// TODO Error Handling
			return;
		}
	}

	// Enable certain interrupts
	setSn_IMR(sockNum, (Sn_IR_TIMEOUT | Sn_IR_RECV | Sn_IR_DISCON | Sn_IR_CON));
}

void Ethernet_closeSocket(uint8_t sockNum) {
	// Check if invalid sockNum or sock not opened
	if(sockNum >= _WIZCHIP_SOCK_NUM_) {
		// TODO Error Handling
		return;
	}

	// Mark as no longer in use
	sockets[sockNum].inUse = false;

	// Queue Request in SPI-Task
	ethernet_h.spiQueueRequest(__closeSocket_CB, (void*) (uintptr_t) sockNum);
}

void __closeSocket_CB(void* pData) {
	uint8_t sockNum = (uint8_t) (uintptr_t) pData;

	if(close(sockNum) != SOCK_OK) {
		// TODO Error Handling
		sockets[sockNum].inUse = true;
		return;
	}
}

void Ethernet_send(uint8_t sockNum, uint8_t* tx_buffer, uint16_t len) {
	SocketHandle_t socket = sockets[sockNum];

	switch(socket.protocol){
	case TCP:
		if(send(sockNum, tx_buffer, len) != len) {
			/* TODO Error Handling */
		}
		break;
	case UDP:
		uint8_t target_ip[4];
		uint16_t target_port;

		// Differentiate between Debug-Out and normal UDP-Sockets
		if(socket.port == DEBUG_PORT && ETHERNET_DBGOUT_ON) {
			uint8_t temp[] = DEBUG_BROADCAST_IP;
			target_port = (uint16_t) DEBUG_PORT;
			memcpy(target_ip, temp, 4);
		}
		else {
			uint8_t temp[] = WIZ_IP;
			target_port = socket.port;
			memcpy(target_ip, temp, 4);
		}

		if(sendto_W5x00(sockNum, tx_buffer, len, target_ip, target_port) != len) {
			/* TODO Error Handling */
		}
		break;
	default:
		break;
	}
}

int32_t Ethernet_receive(uint8_t sockNum, uint8_t* rx_buffer, uint16_t len) {
	int32_t recvLen = recv(sockNum, rx_buffer, len);
	if(recvLen == 0) {
		return 0;
		// TODO Error Handling
	}

	return recvLen;
}

void Ethernet_setRxBufferSize(uint8_t sockNum, uint16_t size) {
	// TODO
	uint16_t buffer_size = 0;
}

void Ethernet_resetBuffer(BufferHandle_t* buffer) {
	memset(buffer->buffer, 0, buffer->size);
}


/// Helper functions ///
uint8_t __Ethernet_getFreeSocket() {
	for(uint8_t sockNum = 0; sockNum < _WIZCHIP_SOCK_NUM_; sockNum++) {
		if(sockets[sockNum].inUse != true) return sockNum;
	}

	return -1;
}

