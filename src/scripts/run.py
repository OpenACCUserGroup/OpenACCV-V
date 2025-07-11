#!/usr/bin/env python3
"""
Command-line test runner for OpenACCV-V.
Executes compiled test binaries and records results.
"""

import json
import subprocess
import time
import argparse
import sys
from os.path import join, dirname, exists, abspath
from results import results

def main():
    parser = argparse.ArgumentParser(description="Run a compiled test for OpenACCV-V")
    parser.add_argument('--executable')
    args = parser.parse_args()
    
    # Resolve paths
    executable_path = abspath(args.executable)
    
    try:
        exists(executable_path)
    except Exception as e:
        result = {
            "command": executable_path,
            "success": False,
            "return_code": "",
            "errors": str(e),
            "output": sys.stderr,
            "runtime": 0.0
        }     
    
        results(project_root, executable_path, None, result)
        return
        
    project_root = abspath(executable_path[:executable_path.find('/OpenACCV-V/') + len('/OpenACCV-V/')])
    config_path = join(project_root, "build/testsuite_config/", "build_config.json")
    
    if not exists(config_path):
        print(f"Error: Config file not found: {config_path}", file=sys.stderr)
        sys.exit(1)
    
    # Load build config
    with open(config_path) as f:
        config = json.load(f)
    
    verbose = config["testing"].get("verbose", False)
    serial = config["testing"].get("serial", True)
    
    cmd = [executable_path]
        
    if verbose and serial:
        print(' '.join(cmd))

    # Run the test with a timeout
    start = time.time()
    try:
        process = subprocess.run(
            cmd,
            cwd=dirname(executable_path),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=False,
            timeout=30  # Set timeout in seconds
        )
        end = time.time()
        runtime = round(end - start, 6)

        result = {
            "command": " ".join(cmd),
            "success": process.returncode == 0,
            "return_code": process.returncode,
            "errors": process.stderr if process.returncode != 0 else "",
            "output": process.stdout,
            "runtime": runtime,
        }
    except subprocess.TimeoutExpired as e:
        end = time.time()
        runtime = round(end - start, 6)

        result = {
            "command": " ".join(cmd),
            "success": False,
            "return_code": "",
            "errors": f"Timeout expired after {runtime} seconds",
            "output": "",
            "runtime": runtime,
        }
    except Exception as e:
        result = {
            "command": " ".join(cmd),
            "success": False,
            "return_code": "",
            "errors": str(e),
            "output": "",
            "runtime": 0.0
        }

    results(project_root, executable_path, None, result)


if __name__ == "__main__":
    main()