#!/usr/bin/env python3
import os
import sys

# Forward execution to portsync/python/collector.py
script_dir = os.path.dirname(os.path.abspath(__file__))
target_collector = os.path.join(script_dir, "portsync", "python", "collector.py")
target_python_dir = os.path.join(script_dir, "portsync", "python")

sys.path.insert(0, target_python_dir)

with open(target_collector, "r") as f:
    code = compile(f.read(), target_collector, "exec")
    exec(code, {"__name__": "__main__", "__file__": target_collector})
