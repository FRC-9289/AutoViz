import logging
import threading

stop_event = threading.Event()

def extract_and_send():
    """
    Extracts data from NetworkTables and sends it over a TCP connection.
    The data includes the angle and velocity of each swerve module.
    """
    import socket
    import json
    import time
    import os
    from networktables import NetworkTables  # type: ignore

    def clear_terminal():
        os.system('cls' if os.name == 'nt' else 'clear')

    # Set up logging
    cwd = os.getcwd()
    config_loc = os.path.join(cwd, "config.json")

    with open(config_loc, "r") as file:
        data = json.load(file)

    try:
        swerve_module_names = data["swerve-modules"]
    except KeyError:
        raise KeyError("Missing 'swerve-modules' key in config.json")

    # Initialize NetworkTables
    NetworkTables.initialize(server='localhost')
    inst = NetworkTables.getDefault()

    start_time = time.time()
    while not inst.isConnected():
        if time.time() - start_time > 10:
            raise TimeoutError("NetworkTables failed to connect within 10 seconds.")
        time.sleep(0.1)

    swerve_table = NetworkTables.getTable("Swerve")

    # Create a TCP/IP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', 0))
    server_socket.listen(1)

    host, port = server_socket.getsockname()
    logging.debug(f"Waiting for a client to connect on {host}:{port}...")

    data["free-port"] = port
    with open(config_loc, "w") as file:
        json.dump(data, file, indent=2)

    client_socket, addr = server_socket.accept()
    client_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

    print(f"Client connected from {addr}")

    prev_ts = time.time()
    try:
        while not stop_event.is_set():
            clear_terminal()
            packet={}
            for type, name in swerve_module_names.items():
                angle = swerve_table.getNumber(f"{name}/angle", 0)
                velocity = swerve_table.getNumber(f"{name}/velocity", 0)
                packet[type]={
                    "angle":angle,
                    "velocity":velocity
                }
            print(
                f"Sending data: {json.dumps(packet, indent=2)}"
            )
            end_time = time.time()
            packet['Robot State'] = swerve_table.getString("Robot State", "Unknown")
            packet['timestamp']=end_time-prev_ts
            prev_ts = end_time
            packet=(json.dumps(packet)+'\n').encode()
            client_socket.sendall(packet)
    except KeyboardInterrupt:
        print("Shutting down gracefully.")
    finally:
        client_socket.close()
        server_socket.close()
