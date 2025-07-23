# Notes

Edgy details I've learned while working on this project.

## Behavior of boost::asio::ip::udp::socket binding on macOS and Windows with respect to multicast and unicast traffic

### macOS

- Binding to address_v4::any() (0.0.0.0):
    - The socket will receive both unicast and multicast traffic (after explicitly joining the multicast group).
    - Multiple sockets bound this way will all receive the same multicast packets.
    - However, only one socket will receive the unicast packets. If that socket closes, the unicast traffic is
      redirected to the next available socket.

- Binding directly to the multicast address:
    - The socket will receive only multicast traffic. Unicast traffic will not be delivered.
    - Multiple sockets bound this way will all receive the same multicast packets.

- Binding to a specific interface address (e.g., 192.168.1.x):
    - The socket will not receive multicast traffic, even after joining a group.

### Windows

- Binding to address_v4::any() (0.0.0.0):
    - The socket will receive both unicast and multicast traffic (after joining the multicast group).
    - Multiple sockets bound this way will all receive the multicast packets, but only one will receive the unicast
      traffic. As with macOS, if the first socket closes, unicast delivery moves to the next socket.

- Binding directly to the multicast address:
    - This is not supported and results in an error.

- Binding to a specific interface address:
    - The socket will receive multicast traffic. However, it’s unclear whether this restricts reception to multicast on
      that specific interface or allows all multicast traffic (it probably does).

  Note: On Windows, there is no mechanism to exclusively receive multicast traffic without also receiving unicast
  traffic. Therefor traffic has to be filtered on the destination address.

## Merging

Merging Anubis doesn't seem to implement the RTSP SETUP and PLAY commands. It would have been nice to be able to use
this as a means to set up a unicast connection.

# LAWO

LAWO devices (A__madi6 specifically) don't offer a way of creating unicast connections through the web interface. Only
multicast addresses are supported. The manual suggests that there are apis to achieve this.
