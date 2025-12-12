typedef enum {
   NETINFO_STATIC = 1,    ///< Static IP configuration
   NETINFO_DHCP           ///< Dynamic IP from DHCP server
} dhcp_mode;

typedef struct NetInfo_t {
   uint8_t mac[6];  ///< Source MAC Address
   uint8_t ip[4];   ///< Source IP Address
   uint8_t sn[4];   ///< Subnet Mask
   uint8_t gw[4];   ///< Gateway IP Address
   uint8_t dns[4];  ///< DNS Server IP Address
   dhcp_mode dhcp;  ///< Configuration mode
} NetInfo;

// Default Configuration
#define WIZ_SOCK_NUM 0
#define WIZ_MAC {0xC0, 0xE0, 0x07, 0x11, 0x11, 0x11}
#define WIZ_IP {169, 254, 116, 50}
#define WIZ_PORT 80
#define WIZ_SUBNETMASK {255, 255, 0, 0}
#define WIZ_DATA_BUFFER_SIZE 2048
