from scapy.all import rdpcap, IP, TCP

def analyze_https_sessions(file_path, host_ip):
    packets = rdpcap(file_path)
    sessions = {}

    for packet in packets:
        if IP in packet and TCP in packet:
            tcp_layer = packet[TCP]

            # Filter only HTTPS traffic
            if tcp_layer.dport == 443 or tcp_layer.sport == 443:
                ip_layer = packet[IP]
                packet_size = len(packet)
                session_key = (
                    ip_layer.src,
                    tcp_layer.sport,
                    ip_layer.dst,
                    tcp_layer.dport,
                )

                if session_key not in sessions:
                    sessions[session_key] = {
                        "to_host_volume": 0,
                        "from_host_volume": 0,
                        "to_host_packets": 0,
                        "from_host_packets": 0,
                        "packet_sizes": [],
                        "interpacket_intervals": [],
                        "initial_timestamp": packet.time,
                        "last_timestamp": None,
                    }

                session = sessions[session_key]

                if ip_layer.dst == host_ip:
                    session["to_host_volume"] += packet_size
                    session["to_host_packets"] += 1
                elif ip_layer.src == host_ip:
                    session["from_host_volume"] += packet_size
                    session["from_host_packets"] += 1

                session["packet_sizes"].append(packet_size)

                if session["last_timestamp"] is not None:
                    interval = packet.time - session["last_timestamp"]
                    session["interpacket_intervals"].append(interval)
                session["last_timestamp"] = packet.time

    session_stats = []
    for session_key, session in sessions.items():
        total_packets = session["to_host_packets"] + session["from_host_packets"]
        mean_packet_size = (
            sum(session["packet_sizes"]) / total_packets if total_packets > 0 else 0
        )
        mean_interpacket_interval = (
            sum(session["interpacket_intervals"])
            / len(session["interpacket_intervals"])
            if session["interpacket_intervals"]
            else 0
        )
        total_session_time = (
            session["last_timestamp"] - session["initial_timestamp"]
            if session["last_timestamp"]
            else 0
        )

        session_stats.append(
            {
                "session": session_key,
                "to_host_volume": session["to_host_volume"],
                "from_host_volume": session["from_host_volume"],
                "to_host_packets": session["to_host_packets"],
                "from_host_packets": session["from_host_packets"],
                "mean_packet_size": mean_packet_size,
                "mean_interpacket_interval": mean_interpacket_interval,
                "initial_timestamp": session["initial_timestamp"],
                "total_session_time": total_session_time,
            }
        )

    for stat in session_stats:
        print(f"Session: {stat['session']}")
        print(f"  Total Volume to Host: {stat['to_host_volume']} bytes")
        print(f"  Total Volume from Host: {stat['from_host_volume']} bytes")
        print(f"  Total Packets to Host: {stat['to_host_packets']}")
        print(f"  Total Packets from Host: {stat['from_host_packets']}")
        print(f"  Mean Packet Size: {stat['mean_packet_size']:.2f} bytes")
        print(f"  Mean Interpacket Interval: {stat['mean_interpacket_interval']:.6f} seconds")
        print(f"  Initial Timestamp: {stat['initial_timestamp']}")
        print(f"  Total Session Time: {stat['total_session_time']:.6f} seconds")
        print()

    return session_stats


if __name__ == "__main__":
    pcap_file = input("Enter the path to the PCAP file: ")
    host_ip = input("Enter the IP address of the host machine: ")

    try:
        results = analyze_https_sessions(pcap_file, host_ip)
    except FileNotFoundError:
        print("The specified file was not found:", pcap_file)
    except Exception as e:
        print("Error:", e)
