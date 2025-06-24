def start_server(projectName):
    import json
    import subprocess
    import os
    import signal
    import sys


    # Global so the main thread can access it
    gradle_proc = None


    # Get project directory
    cwd = os.getcwd()
    config_loc = os.path.join(cwd, "../projects.json")


    with open(config_loc, "r") as file:
        data = json.load(file)

    project_dir = data[projectName]["Project-Directory"]

    def cleanup(signum, frame):
        print(f"Received signal {signum}, terminating gradlew...")
        global gradle_proc
        if gradle_proc is not None:
            try:
                # Terminate the process group
                # This will kill all processes in the group, including the subprocess
                os.killpg(os.getpgid(gradle_proc.pid), signal.SIGTERM)
            except Exception as e:
                # If the process has already terminated, we might get an error
                print(f"Error killing gradlew: {e}")
        sys.exit(0)

    def start_gradlew():
        try:
            global gradle_proc
            # Check if gradlew is already running
            gradle_proc = subprocess.Popen(
                ["./gradlew", "simulateJava"],
                cwd=project_dir,
                preexec_fn=os.setsid  # Create new process group
            )
            gradle_proc.wait()
        except:
            print("Error starting gradlew. Make sure you are in the correct directory and gradlew is executable.")
            print("Try clearing the cache with 'gradlew clean' in the directory and then running again.")

    return (cleanup, start_gradlew)