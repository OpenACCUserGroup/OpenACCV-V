#!/usr/bin/env python3
import json
import os
import argparse
import platform
import datetime
import subprocess

def parse_make_vars(filepath):
    result = {}
    with open(filepath) as f:
        for line in f:
            if '=' in line:
                k, v = line.strip().split('=', 1)
                result[k.strip()] = v.strip()
    return result

def shell_version(cmd):
    try:
        return subprocess.check_output(cmd, shell=True, text=True).strip()
    except:
        return "unknown"

parser = argparse.ArgumentParser()
parser.add_argument("--vars_file", required=True)
parser.add_argument("--output", required=True)
args = parser.parse_args()

vars = parse_make_vars(args.vars_file)

data = {
    "system": vars.get("SYSTEM", "generic"),
    "log_note": vars.get("LOG_NOTE", "none"),
    "openacc_version": vars.get("OACC_VERSION", "unknown"),
    "compiler_version": vars.get("COMPILER_VERSION", "unknown"),
    "compilers": {
        "cc": vars.get("CC", ""),
        "cxx": vars.get("CXX", ""),
        "fc": vars.get("FC", "")
    },
    "flags": {
        "c_flags": vars.get("CFLAGS", ""),
        "cxx_flags": vars.get("CXXFLAGS", ""),
        "f_flags": vars.get("FFLAGS", "")
    },
    "includes": {
        "c_includes": vars.get("C_INCLUDES", ""),
        "cxx_includes": vars.get("CXX_INCLUDES", ""),
        "f_includes": vars.get("F_INCLUDES", "")
    },
    "testing": {
        "verbose": vars.get("VERBOSE", "0") == "1",
        "serial": vars.get("SERIAL", "0") == "1",
        "project_root": vars.get("PROJ_ROOT", ""),
    },
    "env": {
        "user": os.environ.get("USER", ""),
        "host": platform.node(),
        "date": datetime.datetime.now().isoformat(sep=" ", timespec="seconds"),
        "git_sha": shell_version("git rev-parse HEAD"),
        "git_branch": shell_version("git rev-parse --abbrev-ref HEAD"),
    }
}

os.makedirs(os.path.dirname(args.output), exist_ok=True)
with open(args.output, "w") as f:
    json.dump(data, f, indent=2)