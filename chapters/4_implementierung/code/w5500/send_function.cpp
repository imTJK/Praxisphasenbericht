int32_t W5500::send(uint8_t sock_num, uint8_t* pBuf, uint16_t len) {
    uint8_t tmp = 0;
    uint16_t freesize = 0;

    CHECK_SOCKNUM(sock_num);
    CHECK_SOCKMODE(Sn_MR_TCP, sock_num);
    CHECK_SOCKDATA(sock_num);

    tmp = getSn_SR(sock_num);
    if(tmp != SOCK_ESTABLISHED && tmp != SOCK_CLOSE_WAIT) 
        return SOCKERR_SOCKSTATUS;
    
    if(sock_is_sending & (1 << sock_num)) {
        tmp = getSn_IR(sock_num);
        if(tmp & Sn_IR_SENDOK) {
            setSn_IR(sock_num, Sn_IR_SENDOK);
            sock_is_sending &= ~(1 << sock_num);
        }
        else if(tmp & Sn_IR_TIMEOUT) {
            close(sock_num);
            return SOCKERR_TIMEOUT;
        }
        else return SOCK_BUSY;
    }

    freesize = getSn_TxMAX(sock_num);
    if (len > freesize) { len = freesize; }

    while(1) {
        freesize = getSn_TX_FSR(sock_num);
        tmp = getSn_SR(sock_num);
        if ((tmp != SOCK_ESTABLISHED) && (tmp != SOCK_CLOSE_WAIT)) {
            close(sock_num);
            return SOCKERR_SOCKSTATUS;
        }
        if((sock_io_mode & (1<<sock_num)) && (len > freesize)) 
            return SOCK_BUSY;
        if(len <= freesize) break;
    }
    
    send_data(sock_num, pBuf, len);
    setSn_CR(sock_num, Sn_CR_SEND);
    while(getSn_CR(sock_num));  // Wait for command processing

    sock_is_sending |= (1 << sock_num);
    return (int32_t)len;
}
