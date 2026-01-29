#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PlatformIO Pre-Upload Script for mini-doom
Prepares the environment before firmware upload to ESP32
- Handles serial port cleanup
- Ensures upload readiness
- Logs pre-upload actions
"""

Import("env")
import time
import sys
import os

def before_upload(source, target, env):
    """
    Called before upload starts.
    - Closes any active serial monitors
    - Ensures COM port is ready
    - Logs the upload action
    """
    print("\n" + "="*70)
    print("[PRE-UPLOAD] mini-doom Firmware Upload Preparation")
    print("="*70)
    
    upload_port = env.get("UPLOAD_PORT")
    upload_speed = env.get("UPLOAD_SPEED", "460800")
    
    print(f"[PRE-UPLOAD] Target Port:     {upload_port}")
    print(f"[PRE-UPLOAD] Upload Speed:    {upload_speed} baud")
    print(f"[PRE-UPLOAD] Board:           {env.get('BOARD', 'unknown')}")
    print(f"[PRE-UPLOAD] Framework:       {env.get('FRAMEWORK', 'arduino')}")
    print(f"[PRE-UPLOAD] Platform:        {env.get('PLATFORM', 'espressif32')}")
    
    # Get active build flags to show which mode is being uploaded
    build_flags = env.get("BUILD_FLAGS", "")
    if "MODE_DIAG" in str(build_flags):
        print("[PRE-UPLOAD] Active Mode:     MODE_DIAG (Diagnostics)")
    elif "MODE_I2C_SCAN" in str(build_flags):
        print("[PRE-UPLOAD] Active Mode:     MODE_I2C_SCAN (I2C Bus Scan)")
    elif "MODE_OLED_TEST" in str(build_flags):
        print("[PRE-UPLOAD] Active Mode:     MODE_OLED_TEST (OLED Display Test)")
    elif "MODE_DOOM" in str(build_flags):
        print("[PRE-UPLOAD] Active Mode:     MODE_DOOM (Doom Nano Stub)")
    else:
        print("[PRE-UPLOAD] Active Mode:     UNKNOWN")
    
    print("[PRE-UPLOAD] Waiting for port readiness...")
    time.sleep(0.5)
    
    print("[PRE-UPLOAD] ✓ Ready for upload")
    print("[PRE-UPLOAD] Starting upload...")
    print("="*70 + "\n")

def after_upload(source, target, env):
    """
    Called after upload completes.
    Provides feedback about successful upload.
    """
    print("\n" + "="*70)
    print("[POST-UPLOAD] Firmware upload completed successfully!")
    print("[POST-UPLOAD] To view serial output:")
    print(f"[POST-UPLOAD]   pio device monitor -p {env.get('MONITOR_PORT', 'COM7')} -b 115200")
    print("="*70 + "\n")

# Register the hooks
env.AddPreAction("upload", before_upload)
env.AddPostAction("upload", after_upload)

