# voip-rtp

Implemented a voip phone using RTP.On the client side,the packets are recorded periodically using a timer,encoded using g711 and thereafter sent on the network using RTP.
The server waits for a connection after which ,it periodically picks up voice packets,decodes them and plays using pulseaudio.
Used a C library for RTP.
