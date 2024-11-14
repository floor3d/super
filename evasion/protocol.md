# Evasion Protocol
Traffic Volume: Total bytes per minute, reflecting overall network load; specific to a protocol

Packet Sizes: Distribution of packet sizes to approximate average, minimum, and maximum packet lengths; specific to a protocol

Inter-Packet Timing: Time intervals between packets in their respective sessions to replicate the rhythm of traffic flow; specific to a protocol

Protocol Mix: Proportion of different protocols used in the network

DICT str:int volume_to_server

DICT str:int volume_to_host

DICT str:int mean_packet_size

DICT str:int mean_interpacket_interval

[[[XXX]]] DICT str:int protocol_ratios

Algorithm: 
1. take all packets grabbed and calculate these metrics
2. find the most popular protocol
3. do the ssl version of that :P
4. send back volume per minute and mean packet size

implant takes: volume, packet size --> amt keystrokes, and protocol


# Pktmon
Idea: use pktmon to give these metrics to the protocol

Pktmon constraints: it's a SHIT TON OF DATA!!!!!!!!!!!

--> I can limit the ports I use: my ideas are 53, 80, 443, and not really anything outside of that ... in context it wouldn't really make sense

--> In context, what the hell else makes sense except for HTTPS traffic? 

  --> Bypass SSL with encryption so as to not get Found

# SoA is GAN --> generative adversarial networks
--> look at traffic:

session duration, protocol types, data volume, and flow direction (inbound vs. outbound)

packet size and payload length

--> GAN can generate anomalous traffic which is barely anomalous / rare / hard-to-capture anomalies to train itself

Therefore: I must create something that ACTS like an actual website ... needs to have a shit ton of stuff on it so that there is a lot of normal traffic such as grabbing favicon and JS files and CSS files etc, and hide the data in html code n stuff!
--> make generatively large/small html result based on how big packets are, 

The requests must LOOK like they come from a browser, otherwise it's going to get caught because it looks anomalous ... they will also be in https, but there is a possibility of ssl bypass too


# The Big Picture

The target: A normal worker in a company with a Windows laptop, probably has several browser tabs open and stuff which are constantly sending packets

Step 1: use pktmon to grab data and save it to a pcap

Step 2: get this data to the server somehow (idc how for now)

Step 3: calculate stats, such as volume to server, volume to host, mean packet size to/from host, interpacket interval, etc. -- try to do this PER SESSION/STREAM so it doesn't get twisted and amalgamate those

Step 4: Generate website which LOOKS a lot like a real one and has a lot of things to request like a favicon and js etc., because that is what real websites look like and that is what will be studied by the GAN

Step 5: In HTML comments (stupid I know), tell the implant a bunch of requests to make, and split up the keylogs into small chunks in each of those requests (with numbering so i know which goes where); put it encoded in a high entropy header like a cookie OR in some post data that looks like JSON or something

Step 6: Ensure that the session only goes on for as long as normal sessions go for, and drop and recreate sessions when the time is up. Recreate session after completely random interval? idk
