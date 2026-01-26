#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PlatformIO Pre-Upload Script for mini-doom
Prepares the environment before firmware upload to ESP32
"""

Import("env")
import time
import sys

def before_upload(source, target, env):
    """
    Called before upload starts.
    Adds a small delay to ensure COM port is ready.
    """
    print("\n" + "="*60)
    print("[PRE-UPLOAD] Preparing ESP32 for firmware upload...")
    print("="*60)
    
    upload_port = env.get("UPLOAD_PORT")
    print(f"[PRE-UPLOAD] Target port: {upload_port}")
    print("[PRE-UPLOAD] Waiting for port readiness...")
    
    # Small delay to ensure port is available
    time.sleep(0.5)
    
    print("[PRE-UPLOAD] ✓ Ready for upload")
    print("="*60 + "\n")

# Register the pre-upload hook
env.AddPreAction("upload", before_upload)
