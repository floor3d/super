# Evasion Protocol
Traffic Volume: Total bytes per minute, reflecting overall network load; specific to a protocol
Packet Sizes: Distribution of packet sizes to approximate average, minimum, and maximum packet lengths; specific to a protocol
Inter-Packet Timing: Time intervals between packets in their respective sessions to replicate the rhythm of traffic flow; specific to a protocol
Protocol Mix: Proportion of different protocols used in the network

DICT str:int volume_per_minute
DICT str:int mean_packet_size
DICT str:int mean_interpacket_interval
DICT str:int protocol_ratios

Algorithm: 
1. take all packets grabbed and calculate these metrics
2. find the most popular protocol
3. do the ssl version of that :P
4. send back volume per minute and mean packet size

implant takes: volume, packet size --> amt keystrokes, and protocol
