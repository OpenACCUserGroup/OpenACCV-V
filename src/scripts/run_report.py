#!/usr/bin/env python3
"""
OpenACCV-V JSON Log Aggregator

Compiles all individual test JSON files from the logs directory
into a single consolidated JSON report file.
"""

import json
import argparse
import os
import sys
from os.path import join, exists, basename, dirname, abspath


def combine_json_logs(logs_dir, output_file, config_file=None, verbose=False):
    """
    Combines all JSON files in the logs directory into a single JSON file.
    
    Args:
        logs_dir: Path to the directory containing individual JSON log files
        output_file: Path to write the combined JSON output
        config_file: Path to the testsuite configuration file
        verbose: Whether to print verbose output
    
    Returns:
        Number of files processed
    """
    if not exists(logs_dir):
        print(f"Error: Logs directory not found: {logs_dir}", file=sys.stderr)
        return 0
    
    # Find all JSON files in the logs directory
    json_files = [f for f in os.listdir(logs_dir) if f.endswith('.json')]
    
    if not json_files:
        print(f"No JSON files found in {logs_dir}", file=sys.stderr)
        return 0
    
    if verbose:
        print(f"Found {len(json_files)} JSON files in {logs_dir}")
    
    # Initialize the structure with testsuite_configuration
    final_result = {
        "testsuite_configuration": {}
    }
    
    # Add testsuite configuration if available
    if config_file and exists(config_file):
        try:
            with open(config_file, 'r') as f:
                config_data = json.load(f)
                final_result["testsuite_configuration"] = config_data
                if verbose:
                    print(f"Loaded testsuite configuration from {config_file}")
        except Exception as e:
            print(f"Error loading configuration file: {e}", file=sys.stderr)
    
    # Read and combine each JSON file
    for json_file in json_files:
        file_path = join(logs_dir, json_file)
        try:
            with open(file_path, 'r') as f:
                data = json.load(f)
                
                # Extract the test name (removing .json extension)
                test_name = json_file[:-5] if json_file.endswith('.json') else json_file
                
                # Add to final result, preserving the compilation/runtime structure
                if isinstance(data, dict):
                    # If the data already has a source file name key, merge it
                    if len(data) == 1 and test_name in data:
                        final_result[test_name] = data[test_name]
                    else:
                        # Otherwise just use the data directly
                        final_result[test_name] = data
                
                if verbose:
                    print(f"Processed: {json_file}")
                    
        except Exception as e:
            print(f"Error processing {file_path}: {e}", file=sys.stderr)
    
    # Write combined results to output file
    try:
        # Create output directory if it doesn't exist
        output_dir = dirname(output_file)
        if output_dir and not exists(output_dir):
            os.makedirs(output_dir, exist_ok=True)
            
        with open(output_file, 'w') as f:
            json.dump(final_result, f, indent=2)
            
        if verbose:
            print(f"Successfully wrote combined results to {output_file}")
            print(f"Processed {len(json_files)} test files")
            
        return len(json_files)
        
    except Exception as e:
        print(f"Error writing combined results: {e}", file=sys.stderr)
        return 0


def main():
    """Command-line entry point."""
    parser = argparse.ArgumentParser(description="Combine OpenACCV-V test JSON logs into a single file")
    parser.add_argument('-l', default="build/logs", help="Directory containing JSON log files")
    parser.add_argument('-o', default="build/results.json", help="Output JSON file path")
    args = parser.parse_args()
    
    # Determine project root
    script_dir = dirname(abspath(__file__))
    project_root = dirname(dirname(script_dir))  # Go up two levels from src/scripts
    
    # Resolve paths
    logs_dir = join(project_root, args.logs_dir)
    output_file = join(project_root, args.output)
    config_file = join(project_root, "build/testsuite_config", "build_config.json")
    
    # Process JSON files
    num_processed = combine_json_logs(logs_dir, output_file, config_file, args.verbose)
    
    if num_processed > 0:
        print(f"Successfully combined {num_processed} test result files into {output_file}")
        sys.exit(0)
    else:
        print("No test results were combined.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()