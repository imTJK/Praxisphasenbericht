// Common Registers - Getter
uint8_t W5500::getMR()          { return read_register(MR);   }
void W5500::getGAR(uint8_t* p)  { read_buffer(GAR, p, 4);     }
void W5500::getSUBR(uint8_t* p) { read_buffer(SUBR, p, 4);    }
void W5500::getSHAR(uint8_t* p) { read_buffer(SHAR, p, 6);    }
void W5500::getSIPR(uint8_t* p) { read_buffer(SIPR, p, 4);    }
uint8_t W5500::getPHYCFGR()     { return read_register(PHYCFGR); }
uint8_t W5500::getVERSIONR()    { return read_register(VERSIONR); }

// Common Registers - Setter
void W5500::setMR(uint8_t mr)       { write_register(MR, mr);       }
void W5500::setGAR(uint8_t* pgar)   { write_buffer(GAR, pgar, 4);   }
void W5500::setSUBR(uint8_t* psubr) { write_buffer(SUBR, psubr, 4); }
void W5500::setSHAR(uint8_t* pshar) { write_buffer(SHAR, pshar, 6); }
void W5500::setSIPR(uint8_t* psipr) { write_buffer(SIPR, psipr, 4); }

// Socket Registers - Getter
uint8_t W5500::getSn_MR(uint8_t sn)  { return read_register(Sn_MR(sn));  }
uint8_t W5500::getSn_CR(uint8_t sn)  { return read_register(Sn_CR(sn));  }
uint8_t W5500::getSn_IR(uint8_t sn)  { return read_register(Sn_IR(sn));  }
uint8_t W5500::getSn_SR(uint8_t sn)  { return read_register(Sn_SR(sn));  }

// Socket Buffer Access
uint16_t W5500::getSn_RxMAX(uint8_t sn) { 
    return ((uint16_t)getSn_RXBUF_SIZE(sn) << 10); 
}
uint16_t W5500::getSn_TxMAX(uint8_t sn) { 
    return ((uint16_t)getSn_TXBUF_SIZE(sn) << 10); 
}
