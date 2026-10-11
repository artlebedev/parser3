@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0run_tests.ps1" %*
