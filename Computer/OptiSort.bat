@echo off
cd /d "%~dp0"
if exist ..\.venv\Scripts\activate (
    call ..\.venv\Scripts\activate
)
echo Starting OptiSort...
python app.py
