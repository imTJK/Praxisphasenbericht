/**
 * @brief HAL for the WIZnet W5500-IC
 * @details Partially derived HAL for the WIZnet W5500-IC
 * Class-based combination of socket and base-driver provided by WIZnet.
 *
 * @note Original Driver can be found at:
 *       https://docs.wiznet.io/Product/iEthernet/W5500/driver
 */
class W5500 : protected SPIDevice, public GPIO_EXTI_Handler {
public:
    // Command structure for easy extensibility
    typedef struct {
        const char* command;
        void (W5500::*handler)(uint8_t sock_num, const char* args, 
                               char* response, size_t max_response_len);
        const char* help_text;
    } W5500_Command;

    W5500();
    ~W5500() {};

    // Socket API
    int8_t socket(uint8_t sock_num, uint8_t protocol, 
                  uint16_t port, uint8_t flag);
    int8_t listen(uint8_t sock_num);
    int8_t connect(uint8_t sock_num, uint8_t* pAddr, uint16_t port);
    int8_t disconnect(uint8_t sock_num);
    int8_t close(uint8_t sock_num);
    
    // Data Transfer
    int32_t send(uint8_t sock_num, uint8_t* pBuf, uint16_t len);
    int32_t recv(uint8_t sock_num, uint8_t* pBuf, uint16_t len);
    int32_t sendto(uint8_t sock_num, uint8_t* pBuf, uint16_t len, 
                   uint8_t* addr, uint16_t port);
    int32_t recvfrom(uint8_t sock_num, uint8_t* pBuf, uint16_t len, 
                     uint8_t* addr, uint16_t* port);
    
    // Network Configuration
    void setNetInfo(NetInfo* pnetinfo);
    void getNetInfo(NetInfo* pnetinfo);
    
    // Command Interpreter
    void processIncomingMessage(uint8_t sock_num, uint8_t* buffer, 
                                uint16_t len);
    
    // ... weitere Methoden
};
