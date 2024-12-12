# PTP Notes

## Names for major entities (6.1.3)

- PTP Instance
- PTP Node
- Boundary Clock
- Ordinary Clock
- Transparent Clock
- Local Clock
- Local PTP Clock
- PTP Management Node

## Device types (6.5.1)

- Ordinary Clock
- Boundary Clock
- End-to-End Transparent Clock
- Peer-to-Peer Transparent Clock
- PTP Management Node

## PTP Port states (6.6.2.2)

### Master
The PTP Port is the source of time on the PTP Communication Path served by the PTP
Port.

### Slave
The PTP Port synchronizes to the PTP Port on the PTP Communication Path that is in the
MASTER state.

### Passive
The PTP Port is not the source of time on the PTP Communication Path nor does it
synchronize to a Master Clock.

## Oscillator frequency (7.2.2)
Oscillators used to establish or maintain the timescale of a PTP Instance (see 7.2.1) shall maintain a period,
that is, the inverse of the frequency, deviating no more than 0.01% from the period of a perfect oscillator of
the same nominal frequency.

## Epoch (7.2.3)
The PTP epoch (the epoch of timescale PTP) is 1 January 1970 00:00:00 TAI.

## PTP Messages (7.3)

### Attributes (7.3.2)

- Message class
- Message source port identity
- Message type
- Message sequence ID
- Flags defining options

### Event messages (7.3.3.1)

- Sync
- Delay_Req
- Pdelay_Req
- Pdelay_Resp

### general messages (7.3.3.2)

- Announce
- Follow_Up
- Delay_Resp
- Pdelay_Resp_Follow_Up
- Management
- Signaling