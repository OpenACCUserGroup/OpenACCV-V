#!/usr/bin/env python3
"""
Command-line compiler script for OpenACCV-V.
Compiles test files according to the build configuration.
"""

import json
import sys
import os
from os.path import join, basename, exists


def results(project_root, file_path, compilation_result, runtime_result):
    
    # Get base filename without extension
    src_name = basename(file_path)
    
    # Remove .o or .FOR.o suffix if present
    if src_name.endswith('.FOR.o'):
        src_name = src_name[:-6]  # Remove 6 chars: '.FOR.o'
    elif src_name.endswith('.o'):
        src_name = src_name[:-2]  # Remove 2 chars: '.o'
        
    log_dir = join(project_root, "build/logs")
    output_json = join(log_dir, f"{src_name}.json")

    # Initialize with empty structure
    test_result = {
        src_name: {
            "compilation": {},
            "runtime": {}
        }
    }
    
    # Read existing data if available
    if exists(output_json):
        try:
            with open(output_json, 'r') as f:
                test_result = json.load(f)
        except Exception as read_error:
            print(f"Error reading existing JSON file: {read_error}", file=sys.stderr)
    
    # Update with new data
    if compilation_result:
        test_result[src_name]["compilation"] = compilation_result
    
    if runtime_result:
        test_result[src_name]["runtime"] = runtime_result
    
    # Write the updated result to the file
    try:
        os.makedirs(log_dir, exist_ok=True)  # Ensure directory exists
        with open(output_json, 'w') as f:
            json.dump(test_result, f, indent=4)
    except Exception as file_error:
        print(f"Error writing to {output_json}: {file_error}", file=sys.stderr)