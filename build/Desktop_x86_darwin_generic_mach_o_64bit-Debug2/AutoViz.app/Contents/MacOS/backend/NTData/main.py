import threading
import time
import subprocess
import signal
import sys
import os
import json

from Start_Server import start_server  # renamed function
from Extract import extract_and_send, stop_event

projectName = sys.argv[1]
# Global so the main thread can access it
(cleanup, start_gradlew) = start_server(projectName)  # Call the renamed function

if __name__ == "__main__":
    # Set up signal handlers in main thread
    signal.signal(signal.SIGTERM, cleanup)
    signal.signal(signal.SIGINT, cleanup)

    # Start gradlew in its own thread
    thread_a = threading.Thread(target=start_gradlew)
    thread_a.start()

    # Wait a little, then start extract_and_send
    time.sleep(3)
    thread_b = threading.Thread(target=extract_and_send)
    thread_b.start()

    # Wait for both
    thread_a.join()
    thread_b.join()

    print("Both threads finished.")