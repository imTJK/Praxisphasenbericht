/*
 * ethernet.h
 *
 *  Created on: Nov 28, 2024
 *      Author: TK
 */

#ifndef INC__ETHERNET_H_
#define INC__ETHERNET_H_

#ifdef __cplusplus
extern "C" {
#endif


// Includes
extern "C++" {
#include "gpio_exti_handler.h"
}

#include "spi_device.h"
#include "../Inc/W5500_conf.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>



/**
 * @defgroup WIZnet_API
 * @brief WIZnet-derived API for the W5500-IC. Altered for use in <a href="https://atn-berlin.de/">ATN Automatisierungstechnik Niemeier</a>
 * @details This API was derived from the socket.c and w5500.c provided by WIZNet as Drivers for the W5500-IC. \n
 *          Class-Integration to allow for OOP and slight variable name changes.
 *
 * @{
 */

/**
 * @defgroup WIZnet_Socket_API Socket API-Calls for the WIZnet W5500
 * @brief Socket-Specific API-Calls
 */

// Class Declaration
/**
 * @brief HAL for the WIZnet W5500-IC
 * @details Partially derived HAL for the WIZnet W5500-IC \n
 * Class-based combination of socket and base-driver provided by WIZnet. \n
 * Small changes in variable naming and documentation.
 *
 * @note Original Driver can be found <a href="https://docs.wiznet.io/Product/iEthernet/W5500/driver">here</a>
 */
class W5500 : protected SPIDevice, public GPIO_EXTI_Handler {
public:
	// Typdefs
	// Command structure for easy extensibility
	typedef struct {
	    const char* command;
	    void (W5500::*handler)(uint8_t sock_num, const char* args, char* response, size_t max_response_len);
	    const char* help_text;
	} W5500_Command;

	/* Constructors and Destructors */
	/**
	 * @brief Initialization of the W5500-IC
	 * @details Sets up the W5500-IC based on the provided details in \ref WIZCHIP_CONF
	 */
	W5500();


	/**
	 * @brief Basic Destructor
	 */
	~W5500() {};

	/* Variables */
	/* Methods */


	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief Opens the socket \p sock_num.
	 *
	 * @param sock_num Socket number. <b>[0-7]</b>
	 * @param protocol Protocol type to operate in such as TCP, UDP and MACRAW.
	 * @param port Port number to bind.
	 * @param flag Socket flags like SF_BROAD_BLOCK, SF_MULTI_BLOCK, SF_IPv6_BLOCK and SF_UNI_BLOCK
	 *
	 * @returns @b Success : \p sock_num : Socket successfully established \n
	 * 			@b Failure : \n
	 * 			 		   \ref SOCKERR_SOCKNUM - Invalid socket number \n
	 * 			 		   \ref SOCKERR_SOCKMODE - Unsupported socket mode \n
	 * 			 		   \ref SOCKERR_SOCKFLAG - Invalid socket flag
	 */
	int8_t socket(uint8_t sock_num, uint8_t protocol, uint16_t port, uint8_t flag);


	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief Control socket.
	 * @details Controls IO mode, Interrupt & Mask of socket and gets socket buffer information.
	 *           Refer to \ref ctlsock_type.
	 * @param sock_num socket number
	 * @param cstype type of control socket. \ref ctlsock_type.
	 * @param arg Data type and value is determined according to \ref ctlsock_type. \n
	 *             <table>
	 *                  <tr> <td> <b>\p cstype</b> </td> <td> @b data type</td><td>@b value</td></tr>
	 *                  <tr> <td> \ref CS_SET_IOMODE \n \ref CS_GET_IOMODE </td> <td> uint8_t </td><td>\ref SOCK_IO_BLOCK, \ref SOCK_IO_NONBLOCK</td></tr>
	 *                  <tr> <td> \ref CS_GET_MAXTXBUF \n \ref CS_GET_MAXRXBUF </td> <td> uint16_t </td><td> 0 ~ 16K </td></tr>
	 *                  <tr> <td> \ref CS_CLR_INTERRUPT \n \ref CS_GET_INTERRUPT \n \ref CS_SET_INTMASK \n \ref CS_GET_INTMASK </td> <td> \ref sockint_kind </td><td> \ref SIK_CONNECTED, etc.  </td></tr>
	 *             </table>
	 *  @returns @b Success : \ref SOCK_OK \n
	 *           @b Failure : \ref SOCKERR_ARG         - Invalid argument
	 */
	int8_t ctlsocket(uint8_t sock_num, ctlsock_type cstype, void* arg);


	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief Listens for a connection request from a client.
	 *
	 * @details If connection request is accepted successfully, the connection is established automatically.
	 * 	Socket \p sock_num is used in passive (Server) mode.
	 *
	 * @param sock_num Socket number. <b>[0-7]</b>.
	 *
	 * @returns @b Successs : \ref SOCK_OK - Connection Established \n
	 * 			@b Failure : \n
	 * 					   \ref SOCKERR_SOCKINIT -  Socket is not initialized \n
	 * 					   \ref SOCKERR_SOCKCLOSED -  Socket closed unexpectedly
	 */
	int8_t listen(uint8_t sock_num);


	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief Try to connect to a server.
	 * @details It requests connection to the server with destination IP address and port number passed as parameters.\n
	 *
	 * @note It is valid only in TCP client mode.\n
	 *       In blocking IO-mode (\ref SOCK_IO_BLOCK), it does not return until connection is completed.\n
	 *       In non-blocking IO-mode (\ref SOCK_IO_NONBLOCK), it returns \ref SOCK_BUSY immediately.
	 *
	 * @param sock_num Socket number. <b>[0-7]</b>
	 * @param pAddr Destination IP address Pointer. It should be allocated 4 bytes.
	 * @param port Destination port number.
	 *
	 * @returns @b Success : \ref SOCK_OK - Connection Established\n
	 * 			@b Failure : \n
	 * 					   \ref SOCKERR_SOCKNUM   - Invalid socket number\n
	 * 					   \ref SOCKERR_SOCKMODE  - Invalid socket mode\n
	 * 					   \ref SOCKERR_SOCKINIT  - Socket is not initialized\n
	 * 					   \ref SOCKERR_IPINVALID - Wrong server IP address\n
	 * 					   \ref SOCKERR_PORTZERO  - Server port zero\n
	 * 					   \ref SOCKERR_TIMEOUT   - Timeout occurred during request connection\n
	 * 					   \ref SOCK_BUSY         - In non-block io mode, it returned immediately\n
	 */
	int8_t connect(uint8_t sock_num, uint8_t* pAddr, uint16_t port);


	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief Try to disconnect a connected socket.
	 * @details It sends a request message to disconnect the TCP socket \p sock_num from the server or client.
	 *
	 * @note It is valid only in TCP server or client mode. \n
	 *       In blocking IO-Mode (\ref SOCK_IO_BLOCK), it does not return until disconnection is completed. \n
	 *       In Non-block IO-Mode (\ref SOCK_IO_NONBLOCK), it returns \ref SOCK_BUSY immediately. \n

	 * @param sock_num Socket number. <b>[0-7]</b>.
	 * @returns @b Success : \ref SOCK_OK \n
	 *          @b Failure : \n
	 *                     \ref SOCKERR_SOCKNUM  - Invalid socket number \n
	 *                     \ref SOCKERR_SOCKMODE - Invalid operation in the socket \n
	 *                     \ref SOCKERR_TIMEOUT  - Timeout occurred \n
	 *                     \ref SOCK_BUSY        - Socket is busy.
	 */
	int8_t disconnect(uint8_t sock_num);


	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief Closes the socket \p sock_num.
	 *
	 * @see WIZ_SOCKET
	 * @see WIZ_ERROR
	 *
	 * @param sock_num Socket number. <b>[0-7]</b>
	 *
	 * @returns @b Success : \ref SOCK_OK : Socket successfully closed \n
	 * @b Failure : \ref SOCKERR_SOCKNUM : Invalid socket number
	 */
	int8_t close(uint8_t sock_num);

	// - Send & Receive
	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief Send data to the connected peer.
	 *
	 * @note    It is valid only in TCP server or client mode. It can't send data greater than socket buffer size. \n
	 *          In blocking IO-mode (\ref SOCK_IO_BLOCK), it doesn't return until the send-operation is completed.\n
	 *          In non-blocking IO-mode (\ref SOCK_IO_NONBLOCK), \ref SOCK_BUSY is immediately returned if the socket buffer is not large enough. \n
	 *
	 *
	 * @param sock_num Socket number. <b>[0-7]</b>.
	 * @param pBbuf Buffer-Pointer containing the data to be sent.
	 * @param len The byte length of data in \p pBuf.
	 * @returns	@b Success : \p len - The size of the sent data \n
	 *          @b Failure : \n
	 *          		   \ref SOCKERR_SOCKSTATUS - Invalid socket status for socket operation \n
	 *          		   \ref SOCKERR_TIMEOUT    - Timeout occurred \n
	 *          		   \ref SOCKERR_SOCKMODE 	- Invalid operation in the socket \n
	 *          		   \ref SOCKERR_SOCKNUM    - Invalid socket number \n
	 *          		   \ref SOCKERR_DATALEN    - zero data length \n
	 *          		   \ref SOCK_BUSY          - Socket is busy.
	 */
	int32_t send(uint8_t sock_num, uint8_t* pBuf, uint16_t len);

	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief	Receive data from the connected peer.
	 * @details It is used to read incoming data from the connected socket.\n
	 *          It waits for data as much as the application wants to receive.
	 *
	 * @note    It is valid only in TCP server or client mode. It can't receive data greater than the socket buffer size. \n
	 *          In blocking IO-Mode (\ref SOCK_IO_BLOCK), it doesn't return until data reception is completed. \n
	 *          In non-blocking IO-Mode (\ref SOCK_IO_NONBLOCK), it returns \ref SOCK_BUSY immediately when \p len  is greater than data size in socket buffer. \n
	 *
	 * @param sock_num  Socket number. <b>[0-7]</b>
	 * @param pBbuf Buffer-Pointer to read incoming data into.
	 * @param len The max data length of data in \p pBuf.
	 * @returns	@b Success : The real size of the received data \n
	 *          @b Failure : \n
	 *                     \ref SOCKERR_SOCKSTATUS - Invalid socket status for socket operation \n
	 *                     \ref SOCKERR_SOCKMODE   - Invalid operation in the socket \n
	 *                     \ref SOCKERR_SOCKNUM    - Invalid socket number \n
	 *                     \ref SOCKERR_DATALEN    - zero data length \n
	 *                     \ref SOCK_BUSY          - Socket is busy.
	 */
	int32_t recv(uint8_t sock_num, uint8_t* pBuf, uint16_t len);



	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief	Sends datagram to a peer.
	 * @details It sends datagram of UDP or MACRAW to the peer with the destination IP address and port number passed as parameters.\n
	 *          Even if the connectionless socket has been previously connected to a specific address,
	 *          the address and port number parameters override the destination address for that particular datagram only.
	 *
	 * @note    In blocking IO-Mode (\ref SOCK_IO_BLOCK), It doesn't return until data send is completed. \n
	 *          In non-blocking IO-Mode (\ref SOCK_IO_NONBLOCK), It returns \ref SOCK_BUSY immediately when socket buffer is not enough.
	 *
	 * @param sock_num Socket number. <b>[0-7]</b>.
	 * @param pBbuf Buffer-Pointer to send data from.
	 * @param len The byte length of data in \p pBuf.
	 * @param pAddr Destination IP-Address Pointer. It should be allocated 4 bytes.
	 * @param port Destination port number.
	 *
	 * @returns @b Success : The sent data size \n
	 *          @b Failure : \n
	 *          	       \ref SOCKERR_SOCKNUM     - Invalid socket number \n
	 *          		   \ref SOCKERR_SOCKMODE    - Invalid operation in the socket \n
	 *         	 	       \ref SOCKERR_SOCKSTATUS  - Invalid socket status for socket operation \n
	 *         		       \ref SOCKERR_DATALEN     - zero data length \n
	 *         		       \ref SOCKERR_IPINVALID   - Wrong server IP address\n
	 *         		       \ref SOCKERR_PORTZERO    - Server port zero\n
	 *         		       \ref SOCKERR_SOCKCLOSED  - Socket unexpectedly closed \n
	 *         		       \ref SOCKERR_TIMEOUT     - Timeout occurred \n
	 *         		       \ref SOCK_BUSY           - Socket is busy.
	 */
	int32_t sendto(uint8_t sock_num, uint8_t * pBuf, uint16_t len, uint8_t * pAddr, uint16_t port);

	/**
	 * @ingroup WIZnet_Socket_API
	 * @brief Receive datagram of UDP or MACRAW
	 * @details This function is an application I/F function which is used to receive the data in modes other then TCP. \n
	 *          This function is used to receive UDP/MAC_RAW-Mode and handle the header as well.
	 *          This function can divide the received data into packets.
	 *          With a MACRAW Socket the \p addr and \p port parameters are ignored.
	 * @note    In blocking IO-Mode (\ref SOCK_IO_BLOCK), it doesn't return until data reception is completed. \n
	 *          In non-blocking IO-Mode (\ref SOCK_IO_NONBLOCK), it returns \ref SOCK_BUSY immediately when <I>len</I> is greater than data size in socket buffer.
	 *
	 * @param sock_num Socket number. <b>[0-7]</b>.
	 * @param pBuf Buffer-Pointer to read incoming data into.
	 * @param len  The max data length of data in pBuf.
	 *             When the received packet size <= \p len, receives only packet sized data.
	 *             Otherwise only receives data of size \p len.
	 * @param pAddr Destination IP-Address-Pointer. It should be allocated 4 bytes.
	 *             Valid only on first call of \p recvfrom.
	 *             When valid, \ref  packinfo[7] should be set as '1' after calling \ref getsockopt(sn, SO_PACKINFO, &packinfo).
	 * @param port Destination port number Pointer.
	 *             Valid only on first call of \p recvfrom.
	 *              When valid, \ref  packinfo[7] should be set as '1' after calling \ref getsockopt(sn, SO_PACKINFO, &packinfo).
	 *
	 * @returns	@b Success : This function return real received data size for success.\n
	 *          @b Failure : \n
	 *                     \ref SOCKERR_DATALEN    - zero data length \n
	 *                     \ref SOCKERR_SOCKMODE   - Invalid operation in the socket \n
	 *                     \ref SOCKERR_SOCKNUM    - Invalid socket number \n
	 *                     \ref SOCKBUSY           - Socket is busy.
	 */
	int32_t recvfrom(uint8_t sock_num, uint8_t * pBuf, uint16_t len, uint8_t * pAddr, uint16_t *pPort);

	void discard_receive(uint8_t sock_num, uint16_t len);

	// Interrupt Service Routine
	void ISR();

	uint8_t getIntFlag();

	// Interrupt Handler compatibilty function
	void handleInterrupt() override;



private:
	uint8_t intFlag = false;  // Interrupt flag

	uint8_t    _DNS_[4];      // DNS server ip address
	dhcp_mode  _DHCP_;        // DHCP mode

	uint8_t DATA_BUFFER[WIZ_DATA_BUFFER_SIZE];

	// 2 Kilobytes per Socket (Must add up to 16)
	uint8_t rx_buffer_sizes[8] = {2, 2, 2, 2, 2, 2, 2, 2};
	uint8_t tx_buffer_sizes[8] = {2, 2, 2, 2, 2, 2, 2, 2};

	uint32_t recvLen;

	uint16_t sock_any_port = SOCK_ANY_PORT_NUM;
	uint16_t sock_io_mode = 0;
	uint16_t sock_is_sending = 0;
	uint8_t  sock_pack_info[_SOCK_NUM_] = {0,};

	uint16_t sock_remained_size[_SOCK_NUM_] = {0,0,};

	/* General */
	uint16_t uint16_read_register(uint32_t AddrSel);
	void uint16_write_register(uint32_t AddrSel, uint16_t val);

	void CRITICAL_ENTER(void) {};
	void CRITICAL_EXIT(void) {};

	void reset(void);
	void phy_reset(void);
	void clear_interrupt(intr_kind intr);

	// Read/Writes
	/**
	 * Reads 1 byte value from a register.
	 *
	 * @param AddrSel Register address
	 * @returns The value of the register
	 */
	uint8_t read_register(uint32_t AddrSel);

	/**
	 * Writes 1 byte value to a register.
	 *
	 * @param AddrSel Register address
	 * @param byte Write data
	 * @returns None
	 */
	void write_register(uint32_t AddrSel, uint8_t byte);

	/**
	 * Reads sequential data from registers.
	 *
	 * @param AddrSel Register Adress
	 * @param pBuf Buffer-Pointer to read data into
	 * @param len Data length
	 * @returns None
	 */
	void read_buffer(uint32_t AddrSel, uint8_t* pBuf, uint16_t len);

	/**
	 * Writes sequential data to registers.
	 *
	 * @param AddrSel Register Address
	 * @param pBuf Buffer-Pointer to write data from
	 * @param len Data length
	 * @returns None
	 */
	void write_buffer(uint32_t AddrSel, uint8_t* pBuf, uint16_t len);

	/**
	 * Copies internal Rx memory to a buffer.
	 *
	 * This function reads the Tx write pointer register and copies
	 * len amount of bytes from wizdata into internal TX memory and
	 * updates the Tx write-pointer register.
	 *
	 * @see send()
	 * @see sendto()
	 * @param sock_num Socket number. [0-7]
	 * @param wizdata Buffer-Pointer to read data from.
	 * @param len Data length
	 */
	void send_data(uint8_t sock_num, uint8_t *wizdata, uint16_t len);

	/**
	 * Copies data to buffer from internal RX memory.
	 *
	 * This function read the Rx register and then copies
	 * the data, of length len, to wizdata.
	 *
	 * @see recv()
	 * @param[in] sock_num Socket number. [0-7]
	 * @param[out] wizdata Buffer-Pointer to write data into.
	 * @param[in] len Data length
	 */
	void receive_data(uint8_t sock_num, uint8_t* wizdata, uint16_t len);

	/**
	 * Discard a sequence of data in RX memory.
	 *
	 * @param sn Socket number. [0-7]
	 * @param len Data length
	 */

	/* Socket */
	// - General / Safety
	int8_t CHECK_SOCKNUM(uint8_t sock_num);
	int8_t CHECK_SOCKMODE(uint8_t mode, uint8_t sock_num);
	int8_t CHECK_SOCKINIT(uint8_t sock_num);
	int8_t CHECK_SOCKDATA(uint16_t len);

	/* Getter */
	// - Physical Layer

	/**
	 * Get the PHY configuration without power mode.
	 *
	 * @see setPHYConf()
	 * @param[out] pphyconf PhyConf-Pointer to write configuration data into.
	 * @returns None
	 */
	void   getPHYconf(PhyConf* pphyconf);

	/**
	 * Get the PHY duplex and speed stats.
	 * Not implemented.
	 *
	 * @see setPHYStat()
	 * @param[out] pphyconf PhyConf-Pointer to write configuration data into.
	 * @returns None
	 */
	void   getPHYStat(PhyConf* pphyconf);

	/*
	 * Get the PHY Link status.
	 *
	 * @returns Status of the PHY Link
	 */
	int8_t getPHYLink(void);

	/*
	 * Get the PHY Power mode.
	 *
	 * @returns PHY Power mode
	 */
	int8_t getPHYMode(void);

	// - Interrupts
	/**
	 * Get W5500 Interrupts.
	 *
	 * @return Enum of all active interrupts
	 */
	intr_kind getInterrupt(void);

	/**
	 * Get W5500 Interrupts-Masks.
	 *
	 * @return Enum of all active interrupt masks
	 */
	intr_kind getInterruptMask(void);


	/**
	 * Get retry time value (RTR) and retry count (RCR).
	 *
	 * @param[out] nettime NetTimeout-Pointer to write RTR and RCR data into
	 */
	void getTimeout(NetTimeout* pnettime);

	/**
	 * Gets the network information
	 *
	 * @see NetInfo
	 * @param[out] pnetinfo NetInfo-Pointer to write data into
	 * @returns none
	 */
	void getNetInfo(NetInfo* pnetinfo);

	/**
	 * Gets the network mode (WOL, PPPoE, Ping Block, etc.)
	 *
	 * @see netmode_type
	 * @returns The current network mode
	 */
	netmode_type getNetMode(void);

	int8_t  getsockopt(uint8_t sn, sockopt_type sotype, void* arg);

	//////////////////////
	// Common Registers //
	//////////////////////

	/**
	 * Get Mode Register
	 *
	 * @see setMR()
	 * @returns uint8_t. The value of Mode register.
	 */
	uint8_t getMR();

	/**
	 * Get Gateway IP-Adress
	 *
	 * @see setGAR()
	 * @param[out] pgar uint8_t-Pointer to write gateway IP address into.
	 *  4 bytes.
	 * @returns None
	 */
	void getGAR(uint8_t* pgar);

	/**
	 * Get Subnet Mask Register
	 *
	 * @see setSUBR()
	 * @param[out] psubr uint8_t-Pointer to write Subnet Mask into.
	 *  4 bytes.
	 * @returns None
	 */
	void getSUBR(uint8_t* psubr);

	/**
	 * Get local MAC adress.
	 *
	 * @see setSHAR()
	 * @param[out] pshar uint8_t-Pointer to write MAC adress into. 6 bytes.
	 * @returns None
	 */
	void getSHAR(uint8_t* pshar);

	/**
	 * Get local IP address.
	 *
	 * @see setSIPR()
	 * @param[out] psipr uint8_t-Pointer to write local IP address into.
	 * 	4 bytes.
	 * @returns None
	 */
	void getSIPR(uint8_t* psipr);

	/**
	 * Get INTLEVEL register.
	 * INTLEVEL configures the Interrupt Assert Wait Time (I_awt).
	 * When the next interrupt occurs, INTn will assert to low after
	 * 	INTLEVEL time.
	 * @f [I_{AWT} = (INTLEVEL + 1) \times \frac{1}{PLL_{clk}^3} \times 4 \quad \text{(when INTLEVEL > 0)}]
	 *
	 * @see setINTLEVEL()
	 * @returns Value of INTLEVEL register.
	 */
	uint16_t getINTLEVEL();

	/**
	 * Get IR register.
	 * The Interrupt Register indicates the interrupt status.
	 * The INTn-PIn is asserted low if (IR(n) && IMR(n)) == 1.
	 *
	 * @see setIR()
	 * @see setIMR()
	 * @see getIMR()
	 * @returns Value of IR register.
	 */
	uint8_t getIR(void);

	/**
	 * Get IMR register.
	 * The Interrupt Mask Register enables certain Interrupts.
	 * The INTn-PIn is asserted low if (IR(n) && IMR(n)) == 1.
	 *
	 * @see setIMR()
	 * @see setIR()
	 * @see getIR()
	 * @returns Value of IMR register.
	 */
	uint8_t getIMR(void);

	/**
	 * Get SIR register.
	 * The Socket Interrupt Register indicates the interrupt status of the
	 * 	Sockets.
	 * The INTn-PIn is asserted low if (SIR(n) && SIMR(n)) == 1.
	 *
	 * @see setSIR)
	 * @see setSIMR()
	 * @see getSIMR()
	 * @returns Value of SIR register.
	 */
	uint8_t getSIR(void);

	/**
	 * Get SIR register.
	 * The Socket Interrupt Mask Register enables the interrupts of
	 *  certain sockets.
	 * The INTn-PIn is asserted low if (SIR(n) && SIMR(n)) == 1.
	 *
	 * @see setSIMR)
	 * @see setSIR()
	 * @see getSIR()
	 * @returns Value of SIMR register.
	 */
	uint8_t getSIMR(void);

	/**
	 * Get RTR Register.
	 * The Retry Time-value Register configures the transmission timeout
	 * 	 period.
	 * If the peer does not respond in time the packet is re-transmitted
	 *  or a timeout is issued.
	 * Unit is 100us.
	 *
	 * @see setRTR()
	 * @see getSn_CR()
	 * @see setSn_CR()
	 * @returns Value of RTR Register
	 */
	uint16_t getRTR(void);

	/**
	 * Get RCR Register.
	 * The Retry Count Register configures the number of re-transmissions.
	 * When re-transmitted RCR+1 times, Timeout is issued.
	 *
	 * @see setRCR()
	 * @see getSn_IR()
	 * @see setSn_IR()
	 * @returns Value of RCR Register
	 */
	uint8_t getRCR(void);

	/**
	 * Get PTIMER Register.
	 * PTIMER configures the timer for sending an LCP Echo Request.
	 * Unit is 25ms.
	 *
	 * @see setPTIMER()
	 * @returns Value of PTIMER Register
	 */
	uint8_t getPTIMER(void);

	/**
	 * Get PMAGIC Register.
	 * PMAGIC configures the 4 bytes magic number to be used in the LCP
	 *  Echo Request.
	 *
	 * @see setPMAGIC()
	 * @returns Value of PMAGIC Register
	 */
	uint8_t getPMAGIC(void);

	/**
	 * Get PHAR Register.
	 * The Destination Hardware Adress Register in PPPoE mode.
	 * TODO: Add a better explanation.
	 *
	 * @see setPHAR()
	 * @param[out] pphar uint8_t-Pointer to write destination IP address
	 *  into. 4 bytes.
	 * @returns None
	 */
	void getPHAR(uint8_t* pphar);

	/**
	 * Get PSID Register.
	 * The Sessions ID Register in PPPoE mode.
	 * TODO: Add a better explanation.
	 *
	 * @see setPSID()
	 * @returns Value of PSID Register
	 */
	uint16_t getPSID(void);

	/**
	 * Get PMRU Register.
	 * The PMRU configures the maximum of received units in PPPoE mode.
	 *
	 * @see setPMRU()
	 * @returns Value of PMRU Register
	 */
	uint16_t getPMRU(void);

	/**
	 * Get UIPR Register.
	 * Contains the IP address that was attempted to be reached before
	 *  a "destination unreachable" Package was received.
	 *
	 * @see getUPORTR()
	 * @returns Value of UIPR Register
	 */
	uint8_t getUIPR(void);

	/**
	 * Get UPORTR Register.
	 * Contains the port that was attempted to be reached before
	 *  a "destination unreachable" Package was received.
	 *
	 * @see getUIPR()
	 * @returns Value of UPORT Register
	 */
	uint16_t getUPORTR(void);

	/**
	 * Get PHYCFGR Register.
	 * The W5500 PHY Configuration Register configures the operation
	 *  mode and indicates the status of PHY.
	 *
	 * @see setPHYCFGR()
	 * @returns Value of PHYCFGR Register
	 */
	uint8_t getPHYCFGR(void);

	/**
	 * Get VERSIONR Register.
	 * The W5500 Chip Version Register is always set to 0x04.
	 *
	 * @returns Value of VERSIONR Register (0x04)
	 */
	uint8_t getVERSIONR(void);

	////////////////////////
	// Socket N Registers //
	////////////////////////
	/**
	 * Get Socket n MR Register.
	 * The Socket n Mode Register configures the options or protocol type
	 *  of Socket n.
	 *
	 *
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @see setSn_MR()
	 * @returns Value of Socket n MR
	 */
	uint8_t getSn_MR(uint8_t sock_num);

	/**
	 * Get Socket n CR Register.
	 * The Socket n Command Register sets the command for Socket n.
	 * After the command has been accepted, the Register is automatically
	 *  cleared to 0x00.
	 * For command-completion check Sn_IR or Sn_SR.
	 *
	 *  @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 *  @see setSn_CR()
	 *  @see getSn_IR()
	 *  @see getSn_SR()
	 *  @returns Value of Socket n CR
	 */
	uint8_t getSn_CR(uint8_t sock_num);

	/**
	 * Get Socket n IR Register.
	 * The Socket n Interrupt Register indicates the current interrupt
	 *  status for Socket n.
	 *
	 * @see setSn_IR() // For testing and clearing certain interrupts.
	 * @see getSn_IMR()
	 * @see setSn_IMR()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n IR
	 */
	uint8_t getSn_IR(uint8_t sock_num);

	/**
	 * Get Socket n IMR Register.
	 * The Socket n Interrupt Mask Register indicates the Socket n'
	 *  interrupts.
	 * When an interrupt occurs and the corresponding bit Sn_IMR is 1,
	 * 	the corresponding bit of Sn_IR is set to 1.
	 *
	 * @see setSn_IMR()
	 * @see getSn_IR()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n IMR
	 */
	uint8_t getSn_IMR(uint8_t sock_num);

	/**
	 * Get Socket n SR Register.
	 * The Socket n Status Register indicate the status of Socket n.
	 * Changed by Sn_CR or special control packets (SYN, FIN).
	 *
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n SR
	 */
	uint8_t getSn_SR(uint8_t sock_num);

	/**
	 * Get Socket n PORT Register.
	 * The Socket n Source Port Register configures the source port of
	 *  Socket n.
	 * Only valid in TCP/UDP Mode.
	 * Set before OPEN Command is ordered.
	 *
	 * @see setSn_PORT()
	 *
	 */
	uint16_t getSn_PORT(uint8_t sock_num);


	/**
	 * Get Socket n DHAR Register.
	 * The Socket n Destination Hardware Address Register configures the
	 *  hardware address of Socket n when using the SEND_MAC in UDP mode.
	 *
	 * @see setSn_DHAR()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @param[out] pdhar uint8_t-Pointer to write DHAR-Register into.
	 * @returns None
	 */
	void getSn_DHAR(uint8_t sock_num, uint8_t* pdhar);

	/**
	 * Get Socket n DIPR Register.
	 * The Socket n Destination IP Address Register configures the
	 *  destination IP Address of Socket n.
	 * Only valid TCP/UDP Mode.
	 *
	 * @see setSn_DIPR()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @param[out] pdipr uint8_t-Pointer to write DIPR-Register into.
	 * @returns None
	 */
	void getSn_DIPR(uint8_t sock_num, uint8_t* pdipr);


	/**
	 * Get Socket n DPORT Register.
	 * The Socket n Destination Port Register configures the destination
	 *  port number of Socket n.
	 * Only valid in TCP/UDP mode.
	 *
	 * @see setSn_DPORT()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n DPORT Register.
	 */
	uint16_t getSn_DPORT(uint8_t sock_num);

	/**
	 * Get Socket n MSSR Register.
	 * The Socket n Maximum Segement Size Register configures the maximum
	 *  segment size in TCP Mode.
	 * Set by other party in passive Mode.
	 *
	 * @see setSn_MSSR()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n MSSR Register.
	 */
	uint16_t getSn_MSSR(uint8_t sock_num);

	/**
	 * Get Socket n TOS Register.
	 * The Socket n Type of Service Register configures the TOS-Field
	 *  in the IP-header of Socket n.
	 *
	 * @link http://www.iana.org/assignments/ip-parameters/ip-parameters.xhtml @endlink
	 * @see setSn_TOS()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n TOS Register
	 */
	uint8_t getSn_TOS(uint8_t sock_num);


	/**
	 * Get Socket n TTL Register.
	 * The Socket n Time to Live Register configures the TTL-Field
	 *  in the IP-header of Socket n.
	 *
	 * @link http://www.iana.org/assignments/ip-parameters/ip-parameters.xhtml @endlink
	 * @see setSn_TTL()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n TOS Register
	 */
	uint8_t getSn_TTL(uint8_t sock_num);

	/**
	 * Get Socket n FRAG Register.
	 * The Socket n Fragment Register configures the FRAG-Field in the
	 *  IP-header of Socket n.
	 *
	 * @see setSn_FRAG()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n TOS Register
	 */
	uint16_t getSn_FRAG(uint8_t sock_num);

	/**
	 * Get Socket n KPALVTR Register.
	 * The Socket n Keep Alive Timer Register configures the
	 *  transmit-timer of the "KEEP ALIVE"-Packet of Socket n.
	 * Only Valid in TCP-Mode.
	 * Unit is 5s.
	 *
	 * @see setSn_KPALVTR()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n KPALVTR Register.
	 */
	uint8_t getSn_KPALVTR(uint8_t sock_num);

	/////////////////////
	// Socket N Buffer //
	/////////////////////
	/**
	 * Get Socket n RX_RSR Register.
	 * The Socket n Received Size Register indicates the size of the data
	 *  received and saved in Socket n' RX Buffer Block.
	 * Calculated as the difference between Sn_RX_WR and Sn_RX_RD.
	 *
	 * @see getSn_RX_WR()
	 * @see getSn_RX_RD()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n RX_RSR Register
	 */
	uint16_t getSn_RX_RSR(uint8_t sock_num);

	/**
	 * Get Socket n TX_FSR Register.
	 * The Socket n Free Size Register indicates the free size of
	 *  Socket n' TX Buffer Block.
	 * Saving data bigger then Sn_TX_FSR overwrites in the TX-Buffer
	 *  overwrites previously written data.
	 *
	 * @see setSn_TX_FSR()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n TX_FSR Register.
	 */
	uint16_t getSn_TX_FSR(uint8_t sock_num);

	/**
	 * Get Socket n RX_RD Register.
	 * The Socket n RX Read Pointer Register.
	 * TOOD: Add a better explanation
	 *
	 * @see setSn_RX_RD()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n RX_RD Register
	 */
	uint16_t getSn_RX_RD(uint8_t sock_num);

	/**
	 * Get Socket n TX_RD Register.
	 * The Socket n TX Read Pointer Register.
	 * TOOD: Add a better explanation
	 *
	 * @see setSn_TX_RD()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n TX_RD Register
	 */
	uint16_t getSn_TX_RD(uint8_t sock_num);

	/**
	 * Get Socket n RX_WR Register.
	 * The Socket n RX Write Pointer Register.
	 * TOOD: Add a better explanation
	 *
	 * @see setSn_RX_WR()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n RX_WR Register
	 */
	uint16_t getSn_RX_WR(uint8_t sock_num);

	/**
	 * Get Socket n TX_WR Register.
	 * The Socket n TX Write Pointer Register.
	 * TOOD: Add a better explanation
	 *
	 * @see setSn_TX_WR()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n TX_WR Register
	 */
	uint16_t getSn_TX_WR(uint8_t sock_num);

	/**
	 * Get Socket n RXBUF_SIZE Register.
	 * The Socket n RX Buffer Size Register configures the RX buffer-block size of Socket n.
	 * Valid values: 1, 2, 4, 8 and 16 kBytes.
	 * Sum of every Sockets RX-Buffer Size cannot exceed 16 kBytes.
	 *
	 * @see setSn_RXBUF_SIZE()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n RXBUF_SIZE Register
	 */
	uint8_t getSn_RXBUF_SIZE(uint8_t sock_num);

	/**
	 * Get Socket n TXBUF_SIZE Register.
	 * The Socket n TX Buffer Size Register configures the TX buffer-block size of Socket n.
	 * Valid values: 1, 2, 4, 8 and 16 kBytes.
	 * Sum of every Sockets TX-Buffer Size cannot exceed 16 kBytes.
	 *
	 * @see setSn_TXBUF_SIZE()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Value of Socket n TXBUF_SIZE Register
	 */
	uint8_t getSn_TXBUF_SIZE(uint8_t sock_num);


	/**
	 * Get Socket n max. RX_BUFFER Size
	 *
	 * @see getSn_RXBUF_SIZE()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Socket n. max. RX_BUFFER Size
	 */
	uint16_t getSn_RxMAX(uint8_t sock_num);

	/**
	 * Get Socket n max. TX_BUFFER Size
	 *
	 * @see getSn_TXBUF_SIZE()
	 * @param[in] sock_num uint8_t Number of the addressed socket (0-7).
	 * @returns Socket n. max. TX_BUFFER Size
	 */
	uint16_t getSn_TxMAX(uint8_t sock_num);

	/* Setter */
	// - Physical Layer

	/**
	 * Set the PHY configuration without power mode.
	 *
	 * @see getPHYConf()
	 * @param pphyconf PhyConf-Pointer to read configuration data from.
	 * @returns None
	 */
	void   setPHYConf(PhyConf* pphyconf);


	int8_t setPHYMmode(uint8_t pmode);

	// - Interrupts
	void setinterruptmask(intr_kind intr);

	// - Timeouts
	void setTimeout(NetTimeout* nettime);


	/**
	 * Set the network information for WIZCHIP
	 *
	 * @see NetInfo
	 * @param[in] pnetinfo NetInfo-Pointer
	 * @returns None
	 */
	void setNetInfo(NetInfo* pnetinfo);

	/**
	 * Sets the network mode (WOL, PPPoE, Ping Block, etc.)
	 *
	 * @see netmode_type
	 * @param netmode Value of network mode.
	 * @returns 0 (OK) or -1 (ERROR)
	 */
	int8_t setNetMode(netmode_type netmode);


	/**
	 *  TODO: FIX DOCUMENTATION
	 *  Set socket options suchs as TTL, MSS, TOS, etc.
	 *
	 *	@see sockopt_type
	 *  @param sock_num Socket number
	 *  @param sotype socket option type
	 *  @param arg Data type and value is determined according to <I>sotype</I>. \n
	 *             <table>
	 *                  <tr> <td> @b sotype </td> <td> @b data type</td><td>@b value</td></tr>
	 *                  <tr> <td> \ref SO_TTL </td> <td> uint8_t </td><td> 0 ~ 255 </td> </tr>
	 *                  <tr> <td> \ref SO_TOS </td> <td> uint8_t </td><td> 0 ~ 255 </td> </tr>
	 *                  <tr> <td> \ref SO_MSS </td> <td> uint16_t </td><td> 0 ~ 65535 </td> </tr>
	 *                  <tr> <td> \ref SO_DESTIP </td> <td> uint8_t[4] </td><td>  </td></tr>
	 *                  <tr> <td> \ref SO_DESTPORT </td> <td> uint16_t </td><td> 0 ~ 65535 </td></tr>
	 *                  <tr> <td> \ref SO_KEEPALIVESEND </td> <td> null </td><td> null </td></tr>
	 *                  <tr> <td> \ref SO_KEEPALIVEAUTO </td> <td> uint8_t </td><td> 0 ~ 255 </td></tr>
	 *             </table>
	 * @return
	 * - @b Success : \ref SOCK_OK \n
	 * - @b Fail
	 *  - \ref SOCKERR_SOCKNUM     - Invalid Socket number \n
	 *  - \ref SOCKERR_SOCKMODE    - Invalid socket mode \n
	 *  - \ref SOCKERR_SOCKOPT     - Invalid socket option or its value \n
	 *  - \ref SOCKERR_TIMEOUT     - Timeout occurred when sending keep-alive packet \n
	 */
	int8_t setsockopt(uint8_t sn, sockopt_type sotype, void* arg);


	//////////////////////
	// Common Registers //
	//////////////////////

	/**
	 * Set the mode register
	 * @see getMR()

	 * @brief Set Mode Register
	 * @param mr The uint8_t value to be  value to be set.
	 */
	void setMR(uint8_t mr);

	/**
	 * Set the gateway address register
	 * @see getGAR()
	 *
	 * @param gar uint8_t-Pointer variable to set gateway IP address to. It should be allocated 4 bytes.
	 */
	void setGAR(uint8_t* pgar);

	/**
	 * Set the subnet mask address
	 * @see getSUBR()
	 *
	 * @param subr uint8_t-Pointer variable to set subnet mask address to. It should be allocated 4 bytes.
	 */
	void setSUBR(uint8_t* psubr);
	void setSHAR(uint8_t* pshar);
	void setSIPR(uint8_t* psipr);

	void setINTLEVEL(uint16_t intlevel);

	void setIR(uint8_t ir);
	void setIMR(uint8_t imr);
	void setSIR(uint8_t sir);
	void setSIMR(uint8_t simr);
	void setRTR(uint16_t rtr);
	void setRCR(uint8_t rcr);
	void setPTIMER(uint8_t ptimer);
	void setPMAGIC(uint8_t pmagic);
	void setPHAR(uint8_t* phar);
	void setPSID(uint16_t psid);
	void setPMRU(uint16_t pmru);
	void setPHYCFGR(uint8_t phycfgr);

	// Socket Registers
	void setSn_MR(uint8_t sock_num, uint8_t mr);
	void setSn_CR(uint8_t sock_num, uint8_t cr);
	void setSn_IR(uint8_t sock_num, uint8_t ir);
	void setSn_IMR(uint8_t sock_num, uint8_t imr);
	void setSn_SR(uint8_t sock_num, uint16_t sr);
	void setSn_PORT(uint8_t sock_num, uint16_t port);

	void setSn_DHAR(uint8_t sock_num, uint8_t* pdhar);
	void setSn_DIPR(uint8_t sock_num, uint8_t* pdipr);

	void setSn_DPORT(uint8_t sock_num, uint16_t dport);
	void setSn_MSSR(uint8_t sock_num, uint16_t mssr);
	void setSn_TOS(uint8_t sock_num, uint8_t tos);
	void setSn_TTL(uint8_t sock_num, uint8_t ttl);
	void setSn_FRAG(uint8_t sock_num, uint16_t frag);
	void setSn_KPALVTR(uint8_t sock_num, uint16_t kpalvtr);

	void setSn_RX_RD(uint8_t sock_num, uint16_t rxrd);
	void setSn_TX_WR(uint8_t sock_num, uint16_t txwr);

	// Socket Buffer
	void setSn_RXBUF_SIZE(uint8_t sock_num, uint8_t rxbufsize);
	void setSn_TXBUF_SIZE(uint8_t sock_num, uint8_t txbufsize);

	// Command Interpreter
	void processIncomingMessage(uint8_t sock_num, uint8_t* buffer, uint16_t len);
	void parseAndExecuteCommand(uint8_t sock_num, const char* message);
	void sendResponse(uint8_t sock_num, const char* response);

	// Command handlers
	void handlePingCommand(uint8_t sock_num, const char* args, char* response, size_t max_response_len);
	void handleStatusCommand(uint8_t sock_num, const char* args, char* response, size_t max_response_len);
	void handleResetCommand(uint8_t sock_num, const char* args, char* response, size_t max_response_len);
	void handleHelpCommand(uint8_t sock_num, const char* args, char* response, size_t max_response_len);
	void handleUnknownCommand(uint8_t sock_num, const char* command, char* response, size_t max_response_len);

	// Helper functions
	void trimWhitespace(char* str);
	int parseArguments(const char* args, char argv[][32], int max_args);

  // Response buffer
  char response_buffer[512];

  // Command table
  static const W5500_Command command_table[];
  static const int num_commands;
};


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SRC__H_ */
