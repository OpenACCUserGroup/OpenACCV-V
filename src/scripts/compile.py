#!/usr/bin/env python3
"""
Command-line compiler script for OpenACCV-V.
Compiles test files according to the build configuration.
"""

import json
import subprocess
import time
import argparse
import sys
import os
from os.path import join, basename, splitext, dirname, exists, abspath

from results import results

def main():
    parser = argparse.ArgumentParser(description="Compile a test file for OpenACCV-V")
    parser.add_argument('--src')
    args = parser.parse_args()
    
    # Resolve paths
    src_path = abspath(args.src)
    
    if not exists(src_path):
        print(f"Error: Source file not found: {src_path}", file=sys.stderr)
        sys.exit(1)
    
    project_root = os.getcwd()
    config_path = join(project_root, "build/testsuite_config/", "build_config.json")
        
    if not exists(config_path):
        print(f"Error: Config file not found: {config_path}", file=sys.stderr)
        sys.exit(1)
    
    # Load build config
    with open(config_path) as f:
        config = json.load(f)
        
    # Get file info
    ext = splitext(src_path)[1].lower()
    src_name = basename(src_path)
    verbose = config["testing"].get("verbose", False)
    serial = config["testing"].get("serial", True)

    # Determine compiler and flags from config
    if ext == ".c":
        compiler = config["compilers"].get("cc").split()[0]
        flags = config["flags"].get("c_flags", "").split()
        include = config["includes"].get("c_includes", "")
        bin_name = src_name + ".o"
        
    elif ext == ".cpp":
        compiler = config["compilers"].get("cxx").split()[0]
        flags = config["flags"].get("cxx_flags", "").split()
        include = config["includes"].get("cxx_includes", "")
        bin_name = src_name + ".o"

    elif ext == ".f90":
        compiler = config["compilers"].get("fc").split()[0]
        flags = config["flags"].get("f_flags", "").split()
        include = config["includes"].get("f_includes", "")
        bin_name = src_name + ".o"
        
    else:
        return {"success": False, "error": f"Unknown file extension: {ext}"}

    # Get directory paths        
    bin_directory = join(project_root, "build/bin")    
    
    os.makedirs(bin_directory, exist_ok=True)
    executable_path = join(bin_directory, bin_name)
            
    # Construct full compile command
    cmd = [compiler] + [include] + flags + ["-o", executable_path, src_path]
            
    if verbose and serial:
        print(' '.join(cmd))

    # Compile
    start = time.time()
    try:
        process = subprocess.run(
            cmd,
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
            "runtime": runtime
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
            "runtime": runtime
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

    results(project_root, src_path, result, None)
                


if __name__ == "__main__":
    main()