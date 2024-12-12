# Notes

Edgy details I've learned while working on this project.

## Sockets

Platforms don't allow multiple sockets to receive the same traffic, i.e. when creating 2 sockets which are bound to the
same port, only one of them will receive the traffic. On macOS there is a slight nuance, where multicast traffic will be
received by both sockets.

## Merging

Merging Anubis doesn't seem to implement the RTSP SETUP and PLAY commands. It would have been nice to be able to use
this as a means to set up a unicast connection.

# LAWO

LAWO devices (A__madi6 specifically) don't offer a way of creating unicast connections through the web interface. Only
multicast addresses are supported. The manual suggests that there are apis to achieve this.
