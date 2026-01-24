#!/bin/bash
# LEDSpicer Package Testing - Common Functions

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_step() { echo -e "${GREEN}==>${NC} $1"; }
print_warning() { echo -e "${YELLOW}Warning:${NC} $1"; }
print_error() { echo -e "${RED}Error:${NC} $1"; }

check_docker() {
	command -v docker &> /dev/null || { print_error "Docker required"; exit 1; }
}

# Standard plugin list (excluding RaspberryPi - library issues)
PLUGINS=('ledspicer-nanoled' 'ledspicer-pacdrive' 'ledspicer-pacled64' 'ledspicer-ultimateio' 'ledspicer-ledwiz32' 'ledspicer-howler' 'ledspicer-adalight')
EXPECTED_PLUGIN_COUNT=7
