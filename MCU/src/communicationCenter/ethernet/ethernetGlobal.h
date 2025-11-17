// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef ETHERNET_GLOBAL_H
#define ETHERNET_GLOBAL_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef uint32_t IP;

#define ETHERNET_MIN_MESSAGE_SIZE (40)
#define ETHERNET_MAX_MESSAGE_SIZE (1520)

int processEthernetMsg(const char* messageBase,
                       unsigned int frameLen);
char* getFreeOutPacket();
int sendCurOutPacket(char* base, uint32_t frameLen);
void ethernetConfigMAC();



#ifdef __cplusplus
 }
#endif 


#endif // ETHERNET_GLOBAL_H