/*! \file */
#include <stdint.h>

#ifndef WIZCHIP_DRIVER_H
#define WIZCHIP_DRIVER_H

//#define OOP_USE_RTOS  // Uncomment if used and initialized within an  RTOS-task

// Typedefs
typedef enum {
   IK_WOL               = (1 << 4),   ///< Wake On LAN by receiving the magic packet. Valid in W500.
   IK_PPPOE_TERMINATED  = (1 << 5),   ///< PPPoE Disconnected
   IK_DEST_UNREACH      = (1 << 6),   ///< Destination IP & Port Unreachable, No use in W5200
   IK_IP_CONFLICT       = (1 << 7),   ///< IP conflict occurred

   IK_SOCK_0            = (1 << 8),   ///< Socket 0 interrupt
   IK_SOCK_1            = (1 << 9),   ///< Socket 1 interrupt
   IK_SOCK_2            = (1 << 10),  ///< Socket 2 interrupt
   IK_SOCK_3            = (1 << 11),  ///< Socket 3 interrupt
   IK_SOCK_4            = (1 << 12),  ///< Socket 4 interrupt, No use in 5100
   IK_SOCK_5            = (1 << 13),  ///< Socket 5 interrupt, No use in 5100
   IK_SOCK_6            = (1 << 14),  ///< Socket 6 interrupt, No use in 5100
   IK_SOCK_7            = (1 << 15),  ///< Socket 7 interrupt, No use in 5100

   IK_SOCK_ALL          = (0xFF << 8) ///< All Socket interrupt
} intr_kind;

typedef struct W5500_Message {
	uint8_t* msg;
	uint16_t len;
	char* sender_id;
} W5500_Message;


// Misc
#define SOCK0_INTERRUPT_ENABLE 0x01

/**
 * @defgroup WIZCHIP_CONF Wizchip Configuration
 * @details Contains Networking-Details for the W5500-IC. \n
 * Change based on Application
 *
 * @{
 */
#define WIZ_SOCK_NUM 0
#define WIZ_MAC {0xC0, 0xE0, 0x07, 0x11, 0x11, 0x11}
#define WIZ_IP {169, 254, 116, 50}
#define WIZ_PORT 80
#define WIZ_SUBNETMASK {255, 255, 0, 0}
#define WIZ_DATA_BUFFER_SIZE 2048
/** @} */


/**
 * @defgroup PHY_CONFIG Physical Layer Config
 *
 * @{
 */
typedef struct PhyConf_t {
	  uint8_t by;       ///< set by \ref PHY_CONFBY_HW or \ref PHY_CONFBY_SW
	  uint8_t mode;     ///< set by \ref PHY_MODE_MANUAL or \ref PHY_MODE_AUTONEGO
	  uint8_t speed;    ///< set by \ref PHY_SPEED_10 or \ref PHY_SPEED_100
	  uint8_t duplex;   ///< set by \ref PHY_DUPLEX_HALF \ref PHY_DUPLEX_FULL
} PhyConf;
/** @} */


/**
 * @defgroup NET_CONFIG Network Config
 *
 * @{
 */
typedef enum {
   NETINFO_STATIC = 1,    ///< Static IP configuration by manually.
   NETINFO_DHCP           ///< Dynamic IP configruation from a DHCP sever
} dhcp_mode;

typedef struct NetInfo_t {
   uint8_t mac[6];  ///< Source Mac Address
   uint8_t ip[4];   ///< Source IP Address
   uint8_t sn[4];   ///< Subnet Mask
   uint8_t gw[4];   ///< Gateway IP Address
   uint8_t dns[4];  ///< DNS server IP Address
   dhcp_mode dhcp;  ///< 1 - Static, 2 - DHCP
} NetInfo;

typedef enum {
   NM_FORCEARP    = (1<<1),  ///< Force to APP send whenever udp data is sent. Valid only in W5500
   NM_WAKEONLAN   = (1<<5),  ///< Wake On Lan
   NM_PINGBLOCK   = (1<<4),  ///< Block ping-request
   NM_PPPOE       = (1<<3),  ///< PPPoE mode
} netmode_type;

typedef struct NetTimeout_t {
   uint8_t  retry_cnt;     ///< retry count
   uint16_t time_100us;    ///< time unit 100us
} NetTimeout;
/** @} */


/**
 * @defgroup SOCK_CONTROL Socket Control
 *
 * @{
 */
typedef enum {
   SIK_CONNECTED     = (1 << 0),    ///< connected
   SIK_DISCONNECTED  = (1 << 1),    ///< disconnected
   SIK_RECEIVED      = (1 << 2),    ///< data received
   SIK_TIMEOUT       = (1 << 3),    ///< timeout occurred
   SIK_SENT          = (1 << 4),    ///< send ok
   SIK_ALL           = 0x1F         ///< all interrupt
} sockint_kind;


typedef enum {
   CS_SET_IOMODE,          ///< set socket IO mode with \ref SOCK_IO_BLOCK or \ref SOCK_IO_NONBLOCK
   CS_GET_IOMODE,          ///< get socket IO mode
   CS_GET_MAXTXBUF,        ///< get the size of socket buffer allocated in TX memory
   CS_GET_MAXRXBUF,        ///< get the size of socket buffer allocated in RX memory
   CS_CLR_INTERRUPT,       ///< clear the interrupt of socket with \ref sockint_kind
   CS_GET_INTERRUPT,       ///< get the socket interrupt. refer to \ref sockint_kind
} ctlsock_type;

typedef enum {
   SO_FLAG,           ///< Valid only in getsockopt(), For set flag of socket refer to <I>flag</I> in \ref socket().
   SO_TTL,              ///< Set TTL. \ref Sn_TTL  ( \ref setSn_TTL(), \ref getSn_TTL() )
   SO_TOS,              ///< Set TOS. \ref Sn_TOS  ( \ref setSn_TOS(), \ref getSn_TOS() )
   SO_MSS,              ///< Set MSS. \ref Sn_MSSR ( \ref setSn_MSSR(), \ref getSn_MSSR() )
   SO_DESTIP,           ///< Set the destination IP address. \ref Sn_DIPR ( \ref setSn_DIPR(), \ref getSn_DIPR() )
   SO_DESTPORT,         ///< Set the destination Port number. \ref Sn_DPORT ( \ref setSn_DPORT(), \ref getSn_DPORT() )
   SO_KEEPALIVESEND,    ///< Valid only in setsockopt. Manually send keep-alive packet in TCP mode, Not supported in W5100
   SO_KEEPALIVEAUTO, ///< Set/Get keep-alive auto transmission timer in TCP mode, Not supported in W5100, W5200
   SO_SENDBUF,          ///< Valid only in getsockopt. Get the free data size of Socekt TX buffer. \ref Sn_TX_FSR, \ref getSn_TX_FSR()
   SO_RECVBUF,          ///< Valid only in getsockopt. Get the received data size in socket RX buffer. \ref Sn_RX_RSR, \ref getSn_RX_RSR()
   SO_STATUS,           ///< Valid only in getsockopt. Get the socket status. \ref Sn_SR, \ref getSn_SR()
   SO_REMAINSIZE,       ///< Valid only in getsockopt. Get the remained packet size in other then TCP mode.
   SO_PACKINFO          ///< Valid only in getsockopt. Get the packet information as \ref PACK_FIRST, \ref PACK_REMAINED, and \ref PACK_COMPLETED in other then TCP mode.
} sockopt_type;
/** @} */


typedef   uint8_t   iodata_t;

/* Defines */
// Wizchip Defines

/**
 * @defgroup WIZ_GENERAL Wizchip General Defines
 *
 * @{
 */
#define _SOCK_NUM_          8   // The count of independent sockets

#define _W5500_SPI_VDM_OP_          0x00
#define _W5500_SPI_FDM_OP_LEN1_     0x01
#define _W5500_SPI_FDM_OP_LEN2_     0x02
#define _W5500_SPI_FDM_OP_LEN4_     0x03

#define _WIZCHIP_IO_MODE_NONE_      0x0000
#define _WIZCHIP_IO_MODE_BUS_       0x0100 /**< Bus interface mode */
#define _WIZCHIP_IO_MODE_SPI_       0x0200 /**< SPI interface mode */

#define _WIZCHIP_IO_MODE_BUS_DIR_      (_WIZCHIP_IO_MODE_BUS_ + 1) /**< BUS interface mode for direct  */
#define _WIZCHIP_IO_MODE_BUS_INDIR_    (_WIZCHIP_IO_MODE_BUS_ + 2) /**< BUS interface mode for indirect */

#define _WIZCHIP_IO_MODE_SPI_VDM_      (_WIZCHIP_IO_MODE_SPI_ + 1) /**< SPI interface mode for variable length data*/
#define _WIZCHIP_IO_MODE_SPI_FDM_      (_WIZCHIP_IO_MODE_SPI_ + 2) /**< SPI interface mode for fixed length data mode*/
#define _WIZCHIP_IO_MODE_SPI_5500_     (_WIZCHIP_IO_MODE_SPI_ + 3) /**< SPI interface mode for fixed length data mode*/

#ifndef _WIZCHIP_IO_MODE_
	#define _WIZCHIP_IO_MODE_       _WIZCHIP_IO_MODE_SPI_VDM_
#endif
/** @} */

/**
 * @defgroup WIZ_REGISTER Wizchip General Register Defines
 *
 * @{
 */
#define _W5500_IO_BASE_				0x00000000

#define _W5500_SPI_READ_    		(0x00 << 2) //< SPI interface Read operation in Control Phase
#define _W5500_SPI_WRITE_			(0x01 << 2) //< SPI interface Write operation in Control Phase

#define _CREG_BLOCK          0x00 	//< Common register block
#define SREG_BLOCK(N)       (1+4*N) //< Socket N register block
#define TXBUF_BLOCK(N)      (2+4*N) //< Socket N Tx buffer address block
#define RXBUF_BLOCK(N)      (3+4*N) //< Socket N Rx buffer address block

#define WIZCHIP_OFFSET_INC(ADDR, N) (ADDR + (N<<8)) //< Increase offset address
/** @} */


/**
 * @defgroup WIZ_SOCKET Wizchip Socket Defines
 *
 * @{
 */
#define SOCKET uint8_t  ///< SOCKET type define for legacy driver
#define SOCK_ANY_PORT_NUM  0xC000

#define SOCK_OK               1        ///< Result is OK about socket process.
#define SOCK_BUSY             0        ///< Socket is busy on processing the operation. Valid only Non-block IO Mode.
#define SOCK_FATAL            -1000    ///< Result is fatal error about socket process.
/** @} */


/**
 * @defgroup WIZ_ERROR Wizchip Error Defines
 *
 * @{
 */
#define SOCK_ERROR            0
#define SOCKERR_SOCKNUM       (SOCK_ERROR - 1)     ///< Invalid socket number
#define SOCKERR_SOCKOPT       (SOCK_ERROR - 2)     ///< Invalid socket option
#define SOCKERR_SOCKINIT      (SOCK_ERROR - 3)     ///< Socket is not initialized or SIPR is Zero IP address when Sn_MR_TCP
#define SOCKERR_SOCKCLOSED    (SOCK_ERROR - 4)     ///< Socket unexpectedly closed.
#define SOCKERR_SOCKMODE      (SOCK_ERROR - 5)     ///< Invalid socket mode for socket operation.
#define SOCKERR_SOCKFLAG      (SOCK_ERROR - 6)     ///< Invalid socket flag
#define SOCKERR_SOCKSTATUS    (SOCK_ERROR - 7)     ///< Invalid socket status for socket operation.
#define SOCKERR_ARG           (SOCK_ERROR - 10)    ///< Invalid argument.
#define SOCKERR_PORTZERO      (SOCK_ERROR - 11)    ///< Port number is zero
#define SOCKERR_IPINVALID     (SOCK_ERROR - 12)    ///< Invalid IP address
#define SOCKERR_TIMEOUT       (SOCK_ERROR - 13)    ///< Timeout occurred
#define SOCKERR_DATALEN       (SOCK_ERROR - 14)    ///< Data length is zero or greater than buffer max size.
#define SOCKERR_BUFFER        (SOCK_ERROR - 15)    ///< Socket buffer is not enough for data communication.

#define SOCKFATAL_PACKLEN     (SOCK_FATAL - 1)     ///< Invalid packet length. Fatal Error.
/** @} */


/**
 * @defgroup WIZ_FLAG Wizchip Flag Defines
 *
 * @{
 */
#define SF_ETHER_OWN           (Sn_MR_MFEN)        ///< In \ref Sn_MR_MACRAW, Receive only the packet as broadcast, multicast and own packet
#define SF_IGMP_VER2           (Sn_MR_MC)          ///< In \ref Sn_MR_UDP with \ref SF_MULTI_ENABLE, Select IGMP version 2.
#define SF_TCP_NODELAY         (Sn_MR_ND)          ///< In \ref Sn_MR_TCP, Use to nodelayed ack.
#define SF_MULTI_ENABLE        (Sn_MR_MULTI)       ///< In \ref Sn_MR_UDP, Enable multicast mode.
#define SF_BROAD_BLOCK         (Sn_MR_BCASTB)      ///< In \ref Sn_MR_UDP or \ref Sn_MR_MACRAW, Block broadcast packet. Valid only in W5500
#define SF_MULTI_BLOCK         (Sn_MR_MMB)     	   ///< In \ref Sn_MR_MACRAW, Block multicast packet. Valid only in W5500
#define SF_IPv6_BLOCK          (Sn_MR_MIP6B)       ///< In \ref Sn_MR_MACRAW, Block IPv6 packet. Valid only in W5500
#define SF_UNI_BLOCK           (Sn_MR_UCASTB)      ///< In \ref Sn_MR_UDP with \ref SF_MULTI_ENABLE. Valid only in W5500

#define SF_IO_NONBLOCK         0x01                ///< Socket nonblock io mode. It used parameter in \ref socket().
/** @} */


/**
 * @defgroup WIZ_IO_MODE Wizchip IO-Mode Defines
 *
 * @{
 */
#define SOCK_IO_BLOCK          0  				   ///< Socket Block IO Mode in \ref setsockopt().
#define SOCK_IO_NONBLOCK       1  				   ///< Socket Non-block IO Mode in \ref setsockopt().

/**
 * @defgroup WIZ_UDP_MACRAW Wizchip UDP & MACRAW Packet Information Defines
 *
 * @{
 */
#define PACK_FIRST               0x80              ///< In Non-TCP packet, It indicates to start receiving a packet. (When W5300, This flag can be applied)
#define PACK_REMAINED            0x01              ///< In Non-TCP packet, It indicates to remain a packet to be received. (When W5300, This flag can be applied)
#define PACK_COMPLETED           0x00              ///< In Non-TCP packet, It indicates to complete to receive a packet. (When W5300, This flag can be applied)
/** @} */


/**
 * @defgroup WIZ_COMMON_REGISTER Wizchip Commong Register Defines
 *
 * @{
 */
#define MR                 (_W5500_IO_BASE_ + (0x0000 << 8) + (_CREG_BLOCK << 3))
#define GAR                (_W5500_IO_BASE_ + (0x0001 << 8) + (_CREG_BLOCK << 3))
#define SUBR               (_W5500_IO_BASE_ + (0x0005 << 8) + (_CREG_BLOCK << 3))
#define SHAR               (_W5500_IO_BASE_ + (0x0009 << 8) + (_CREG_BLOCK << 3))
#define SIPR               (_W5500_IO_BASE_ + (0x000F << 8) + (_CREG_BLOCK << 3))
#define INTLEVEL           (_W5500_IO_BASE_ + (0x0013 << 8) + (_CREG_BLOCK << 3))
#define IR                 (_W5500_IO_BASE_ + (0x0015 << 8) + (_CREG_BLOCK << 3))
#define _IMR_              (_W5500_IO_BASE_ + (0x0016 << 8) + (_CREG_BLOCK << 3))
#define SIR                (_W5500_IO_BASE_ + (0x0017 << 8) + (_CREG_BLOCK << 3))
#define SIMR               (_W5500_IO_BASE_ + (0x0018 << 8) + (_CREG_BLOCK << 3))
#define RTR                (_W5500_IO_BASE_ + (0x0019 << 8) + (_CREG_BLOCK << 3))
#define _RCR_              (_W5500_IO_BASE_ + (0x001B << 8) + (_CREG_BLOCK << 3))
#define PTIMER             (_W5500_IO_BASE_ + (0x001C << 8) + (_CREG_BLOCK << 3))
#define PMAGIC             (_W5500_IO_BASE_ + (0x001D << 8) + (_CREG_BLOCK << 3))
#define PHAR               (_W5500_IO_BASE_ + (0x001E << 8) + (_CREG_BLOCK << 3))
#define PSID               (_W5500_IO_BASE_ + (0x0024 << 8) + (_CREG_BLOCK << 3))
#define PMRU               (_W5500_IO_BASE_ + (0x0026 << 8) + (_CREG_BLOCK << 3))
#define UIPR               (_W5500_IO_BASE_ + (0x0028 << 8) + (_CREG_BLOCK << 3))
#define UPORTR             (_W5500_IO_BASE_ + (0x002C << 8) + (_CREG_BLOCK << 3))
#define PHYCFGR            (_W5500_IO_BASE_ + (0x002E << 8) + (_CREG_BLOCK << 3))
#define VERSIONR           (_W5500_IO_BASE_ + (0x0039 << 8) + (_CREG_BLOCK << 3))
/** @} */


/**
 * @defgroup WIZ_SOCKET_REGISTER Wizchip Socket Defines
 *
 * @{
 */
// Socket Registers
#define Sn_MR(N)           (_W5500_IO_BASE_ + (0x0000 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_CR(N)           (_W5500_IO_BASE_ + (0x0001 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_IR(N)           (_W5500_IO_BASE_ + (0x0002 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_SR(N)           (_W5500_IO_BASE_ + (0x0003 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_PORT(N)         (_W5500_IO_BASE_ + (0x0004 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_DHAR(N)         (_W5500_IO_BASE_ + (0x0006 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_DIPR(N)         (_W5500_IO_BASE_ + (0x000C << 8) + (SREG_BLOCK(N) << 3))
#define Sn_DPORT(N)        (_W5500_IO_BASE_ + (0x0010 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_MSSR(N)         (_W5500_IO_BASE_ + (0x0012 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_TOS(N)          (_W5500_IO_BASE_ + (0x0015 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_TTL(N)          (_W5500_IO_BASE_ + (0x0016 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_RXBUF_SIZE(N)   (_W5500_IO_BASE_ + (0x001E << 8) + (SREG_BLOCK(N) << 3))
#define Sn_TXBUF_SIZE(N)   (_W5500_IO_BASE_ + (0x001F << 8) + (SREG_BLOCK(N) << 3))
#define Sn_TX_FSR(N)       (_W5500_IO_BASE_ + (0x0020 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_TX_RD(N)        (_W5500_IO_BASE_ + (0x0022 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_TX_WR(N)        (_W5500_IO_BASE_ + (0x0024 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_RX_RSR(N)       (_W5500_IO_BASE_ + (0x0026 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_RX_RD(N)        (_W5500_IO_BASE_ + (0x0028 << 8) + (SREG_BLOCK(N) << 3))
#define Sn_RX_WR(N)        (_W5500_IO_BASE_ + (0x002A << 8) + (SREG_BLOCK(N) << 3))
#define Sn_IMR(N)          (_W5500_IO_BASE_ + (0x002C << 8) + (SREG_BLOCK(N) << 3))
#define Sn_FRAG(N)         (_W5500_IO_BASE_ + (0x002D << 8) + (SREG_BLOCK(N) << 3))
#define Sn_KPALVTR(N)      (_W5500_IO_BASE_ + (0x002F << 8) + (SREG_BLOCK(N) << 3))
/** @} */


/**
 * @defgroup WIZ_MODE_REGISTER Wizchip Mode Register Defines
 *
 * @{
 */
// MODE Register values
#define MR_RST                       0x80
#define MR_WOL                       0x20
#define MR_PB                        0x10
#define MR_PPPOE                     0x08
#define MR_FARP                      0x02
#define IR_CONFLICT                  0x80
#define IR_PPPoE                     0x20
#define IR_MP                        0x10
/** @} */


/**
 * @defgroup WIZ_PHYCFG_REGISTER Wizchip PHYCFG register defines
 *
 * @{
 */
#define PHYCFGR_RST                 ~(1<<7)  //< For PHY reset, must operate AND mask.
#define PHYCFGR_OPMD                 (1<<6)   // Configre PHY with OPMDC value
#define PHYCFGR_OPMDC_ALLA           (7<<3)
#define PHYCFGR_OPMDC_PDOWN          (6<<3)
#define PHYCFGR_OPMDC_NA             (5<<3)
#define PHYCFGR_OPMDC_100FA          (4<<3)
#define PHYCFGR_OPMDC_100F           (3<<3)
#define PHYCFGR_OPMDC_100H           (2<<3)
#define PHYCFGR_OPMDC_10F            (1<<3)
#define PHYCFGR_OPMDC_10H            (0<<3)
#define PHYCFGR_DPX_FULL             (1<<2)
#define PHYCFGR_DPX_HALF             (0<<2)
#define PHYCFGR_SPD_100              (1<<1)
#define PHYCFGR_SPD_10               (0<<1)
#define PHYCFGR_LNK_ON               (1<<0)
#define PHYCFGR_LNK_OFF              (0<<0)
/** @} */


/**
 * @defgroup WIZ_IM_REGISTER Wizchip Interrupt Mask register defines
 *
 * @{
 */
#define IM_IR7                  	 0x80
#define IM_IR6                  	 0x40
#define IM_IR5                  	 0x20
#define IM_IR4                  	 0x10
/** @} */


/**
 * @defgroup WIZ_Sn_MR_REGISTER Wizchip Sn_MR register default values defines
 *
 * @{
 */
#define Sn_MR_MULTI                  0x80
#define Sn_MR_BCASTB                 0x40
#define Sn_MR_ND                     0x20
#define Sn_MR_UCASTB                 0x10
#define Sn_MR_MACRAW                 0x04
#define Sn_MR_IPRAW                  0x03
#define Sn_MR_UDP                    0x02
#define Sn_MR_TCP                    0x01
#define Sn_MR_CLOSE                  0x00
#define Sn_MR_MFEN                   Sn_MR_MULTI
#define Sn_MR_MMB                    Sn_MR_ND
#define Sn_MR_MIP6B                  Sn_MR_UCASTB
#define Sn_MR_MC                     Sn_MR_ND
#define SOCK_STREAM                  Sn_MR_TCP
#define SOCK_DGRAM                   Sn_MR_UDP
/** @} */


/**
 * @defgroup WIZ_Sn_CR_REGISTER Wizchip Sn_CR register defines
 *
 * @{
 */
#define Sn_CR_OPEN                   0x01
#define Sn_CR_LISTEN                 0x02
#define Sn_CR_CONNECT                0x04
#define Sn_CR_DISCON                 0x08
#define Sn_CR_CLOSE                  0x10
#define Sn_CR_SEND                   0x20
#define Sn_CR_SEND_MAC               0x21
#define Sn_CR_SEND_KEEP              0x22
#define Sn_CR_RECV                   0x40
#define Sn_IR_SENDOK                 0x10
#define Sn_IR_TIMEOUT                0x08
#define Sn_IR_RECV                   0x04
#define Sn_IR_DISCON                 0x02
#define Sn_IR_CON                    0x01
/** @} */


/**
 * @defgroup WIZ_Sn_SR_REGISTER Wizchip Sn_SR register defines
 *
 * @{
 */
#define SOCK_CLOSED                  0x00
#define SOCK_INIT                    0x13
#define SOCK_LISTEN                  0x14
#define SOCK_SYNSENT                 0x15
#define SOCK_SYNRECV                 0x16
#define SOCK_ESTABLISHED             0x17
#define SOCK_FIN_WAIT                0x18
#define SOCK_CLOSING                 0x1A
#define SOCK_TIME_WAIT               0x1B
#define SOCK_CLOSE_WAIT              0x1C
#define SOCK_LAST_ACK                0x1D
#define SOCK_UDP                     0x22
#define SOCK_IPRAW                   0x32     /**< IP raw mode socket */
#define SOCK_MACRAW                  0x42
/** @} */


/**
 * @defgroup WIZ_IP_PROTOCOLS Wizchip IP-Protocols defines
 *
 * @{
 */
#define IPPROTO_IP                   0        //< Dummy for IP
#define IPPROTO_ICMP                 1        //< Control message protocol
#define IPPROTO_IGMP                 2        //< Internet group management protocol
#define IPPROTO_GGP                  3        //< Gateway^2 (deprecated)
#define IPPROTO_TCP                  6        //< TCP
#define IPPROTO_PUP                  12       //< PUP
#define IPPROTO_UDP                  17       //< UDP
#define IPPROTO_IDP                  22       //< XNS idp
#define IPPROTO_ND                   77       //< UNOFFICIAL net disk protocol
#define IPPROTO_RAW                  255      //< Raw IP packet
/** @} */

#endif
