void W5500::setNetInfo(NetInfo* pnetinfo) {
    setSHAR(pnetinfo->mac);   // Set MAC address
    setGAR(pnetinfo->gw);     // Set gateway
    setSUBR(pnetinfo->sn);    // Set subnet mask
    setSIPR(pnetinfo->ip);    // Set IP address
    
    // Store DNS configuration locally
    _DNS_[0] = pnetinfo->dns[0];
    _DNS_[1] = pnetinfo->dns[1];
    _DNS_[2] = pnetinfo->dns[2];
    _DNS_[3] = pnetinfo->dns[3];
    _DHCP_   = pnetinfo->dhcp;
}

void W5500::getNetInfo(NetInfo* pnetinfo) {
    getSHAR(pnetinfo->mac);
    getGAR(pnetinfo->gw);
    getSUBR(pnetinfo->sn);
    getSIPR(pnetinfo->ip);
    pnetinfo->dns[0] = _DNS_[0];
    pnetinfo->dns[1] = _DNS_[1];
    pnetinfo->dns[2] = _DNS_[2];
    pnetinfo->dns[3] = _DNS_[3];
    pnetinfo->dhcp   = _DHCP_;
}
