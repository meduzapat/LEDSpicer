#!/bin/bash
# LEDSpicer Interactive Install Script
# https://github.com/meduzapat/LEDSpicer
#
# This script helps configure, build, and install LEDSpicer
# It uses whiptail for interactive dialogs (pre-installed on Debian/Ubuntu)

set -e

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Versions (auto-detected from CMakeLists.txt)
VERSION="unknown"
DATA_VERSION=""
PROJECT_NAME=""

# Default values
PREFIX="/usr/local"
SYSCONFDIR="/etc"
BUILD_DIR="build"

# Detected values
ARCH=""
IS_ARM=false
IS_MISTER=false

# Package manager
PKG_MANAGER=""
PKG_INSTALL=""
PKG_UPDATE=""

# User/Group for daemon
RUN_USER=""
USER_ID=""
GROUP_ID=""

# Selected options
DEVICES=()
ENABLE_PULSEAUDIO=OFF
ENABLE_ALSAAUDIO=OFF
ENABLE_MISTER=OFF
ENABLE_RASPBERRYPI=OFF

# Additional globals for modifications
comment_pulse_lines=false
PULSE_USER_ID=1000
IS_DAEMON=false

# Sudo variables for different contexts
SUDO=""            # For system operations (packages, ldconfig, udev, systemd)
SUDO_INSTALL=""    # For make install (depends on PREFIX)
SUDO_CONF=""       # For config file operations (depends on SYSCONFDIR)
IS_SYSTEM_INSTALL=false  # True if installing to system paths (needs ldconfig, systemd, etc.)

# =============================================================================
# Utility Functions
# =============================================================================

detect_version() {
	if [[ -f "CMakeLists.txt" ]]; then
		VERSION=$(sed -n 's/.*project(LEDSpicer VERSION \([0-9.]*\).*/\1/p' CMakeLists.txt)
		DATA_VERSION=$(sed -n 's/.*set(DATA_VERSION[[:space:]]*"\([0-9.]*\)".*/\1/p' CMakeLists.txt)
		PROJECT_NAME=$(sed -n 's/.*project(\([^ ]*\) VERSION.*/\1/p' CMakeLists.txt)
	fi
	VERSION="${VERSION:-unknown}"
	DATA_VERSION="${DATA_VERSION:-1.1}"
	PROJECT_NAME="${PROJECT_NAME:-LEDSpicer}"
}

print_banner() {
	local title="Interactive Install Script v${VERSION}"
	local width=73
	local padding_left=$(( (width - ${#title}) / 2 ))
	local padding_right=$(( width - ${#title} - padding_left ))

	clear
	echo -e "${BLUE}"
	echo "╔═════════════════════════════════════════════════════════════════════════╗"
	echo "║                                                                         ║"
	echo "║   ██╗     ███████╗██████╗ ███████╗██████╗ ██╗ ██████╗███████╗██████╗    ║"
	echo "║   ██║     ██╔════╝██╔══██╗██╔════╝██╔══██╗██║██╔════╝██╔════╝██╔══██╗   ║"
	echo "║   ██║     █████╗  ██║  ██║███████╗██████╔╝██║██║     █████╗  ██████╔╝   ║"
	echo "║   ██║     ██╔══╝  ██║  ██║╚════██║██╔═══╝ ██║██║     ██╔══╝  ██╔══██╗   ║"
	echo "║   ███████╗███████╗██████╔╝███████║██║     ██║╚██████╗███████╗██║  ██║   ║"
	echo "║   ╚══════╝╚══════╝╚═════╝ ╚══════╝╚═╝     ╚═╝ ╚═════╝╚══════╝╚═╝  ╚═╝   ║"
	echo "║                                                                         ║"
	printf "║%*s%s%*s║\n" "$padding_left" "" "$title" "$padding_right" ""
	echo "║                                                                         ║"
	echo "╚═════════════════════════════════════════════════════════════════════════╝"
	echo -e "${NC}"
}

print_step() {
	echo -e "\n${GREEN}==>${NC} ${1}"
}

print_warning() {
	echo -e "${YELLOW}Warning:${NC} ${1}"
}

print_error() {
	echo -e "${RED}Error:${NC} ${1}"
}

check_whiptail() {
	if ! command -v whiptail &> /dev/null; then
		print_error "whiptail is required but not installed."
		echo "Install it with: sudo apt install whiptail"
		exit 1
	fi
}

check_root() {
	if [[ $EUID -eq 0 ]]; then
		print_warning "Running as root. Installation will proceed without sudo."
		SUDO=""
	else
		SUDO="sudo"
	fi
}

# Check if a path requires sudo to write to
# Returns 0 (true) if sudo is needed, 1 (false) if not
path_needs_sudo() {
	local path="$1"

	# If running as root, never need sudo
	[[ $EUID -eq 0 ]] && return 1

	# Find the first existing directory in the path
	local check_path="$path"
	while [[ ! -e "$check_path" && "$check_path" != "/" ]]; do
		check_path=$(dirname "$check_path")
	done

	# Check if writable
	[[ -w "$check_path" ]] && return 1

	return 0
}

# Update sudo variables based on installation paths
# Call this after paths are configured
update_sudo_for_paths() {
	if path_needs_sudo "$PREFIX"; then
		SUDO_INSTALL="sudo"
		IS_SYSTEM_INSTALL=true
	else
		SUDO_INSTALL=""
		IS_SYSTEM_INSTALL=false
	fi

	if path_needs_sudo "$SYSCONFDIR"; then
		SUDO_CONF="sudo"
	else
		SUDO_CONF=""
	fi
}

# =============================================================================
# Package Manager Detection
# =============================================================================

detect_package_manager() {
	if command -v apt-get &>/dev/null; then
		PKG_MANAGER="apt"
		PKG_UPDATE="apt-get update"
		PKG_INSTALL="apt-get install -y"
	elif command -v dnf &>/dev/null; then
		PKG_MANAGER="dnf"
		PKG_UPDATE="dnf check-update || true"
		PKG_INSTALL="dnf install -y"
	elif command -v pacman &>/dev/null; then
		PKG_MANAGER="pacman"
		PKG_UPDATE="pacman -Sy"
		PKG_INSTALL="pacman -S --noconfirm"
	elif command -v zypper &>/dev/null; then
		PKG_MANAGER="zypper"
		PKG_UPDATE="zypper refresh"
		PKG_INSTALL="zypper install -y"
	else
		PKG_MANAGER=""
		PKG_INSTALL=""
		PKG_UPDATE=""
	fi
}

# Translate package names for different distros (apt names as base)
get_package_name() {
	local pkg="$1"

	case "$PKG_MANAGER" in
		dnf)
			case "$pkg" in
				build-essential)        echo "gcc-c++ make" ;;
				g++)                    echo "gcc-c++" ;;
				pkg-config)             echo "pkgconf" ;;
				libtinyxml2-dev)        echo "tinyxml2-devel" ;;
				libusb-1.0-0-dev)       echo "libusb1-devel" ;;
				libpulse-dev)           echo "pulseaudio-libs-devel" ;;
				libasound2-dev)         echo "alsa-lib-devel" ;;
				*)                      echo "$pkg" ;;
			esac
			;;
		pacman)
			case "$pkg" in
				build-essential)        echo "base-devel" ;;
				g++)                    echo "" ;;  # included in base-devel
				make)                   echo "" ;;  # included in base-devel
				pkg-config)             echo "pkgconf" ;;
				libtinyxml2-dev)        echo "tinyxml2" ;;
				libusb-1.0-0-dev)       echo "libusb" ;;
				libpulse-dev)           echo "libpulse" ;;
				libasound2-dev)         echo "alsa-lib" ;;
				*)                      echo "$pkg" ;;
			esac
			;;
		zypper)
			case "$pkg" in
				build-essential)        echo "gcc-c++ make" ;;
				g++)                    echo "gcc-c++" ;;
				pkg-config)             echo "pkgconf" ;;
				libtinyxml2-dev)        echo "tinyxml2-devel" ;;
				libusb-1.0-0-dev)       echo "libusb-1_0-devel" ;;
				libpulse-dev)           echo "libpulse-devel" ;;
				libasound2-dev)         echo "alsa-devel" ;;
				*)                      echo "$pkg" ;;
			esac
			;;
		*)
			# apt/debian names (default)
			echo "$pkg"
			;;
	esac
}

# Get translated package names for display
get_translated_packages() {
	local packages=("$@")
	local translated=()
	local pkg_name

	for pkg in "${packages[@]}"; do
		pkg_name=$(get_package_name "$pkg")
		# Handle packages that expand to multiple (e.g., "gcc-c++ make")
		if [[ -n "$pkg_name" ]]; then
			for p in $pkg_name; do
				translated+=("$p")
			done
		fi
	done

	echo "${translated[@]}"
}

install_packages() {
	local packages=("$@")
	local translated
	translated=($(get_translated_packages "${packages[@]}"))

	if [[ ${#translated[@]} -eq 0 ]]; then
		return 0
	fi

	# No package manager detected or user cancelled
	if [[ -z "$PKG_MANAGER" ]]; then
		print_error "No supported package manager found (apt, dnf, pacman, zypper)."
		echo ""
		echo "Please install these packages manually:"
		printf "  %s\n" "${packages[@]}"
		echo ""
		read -p "Press Enter after installing the packages to continue..."
		return 0
	fi

	print_step "Installing packages: ${translated[*]}"

	# Update package cache
	if [[ -n "$PKG_UPDATE" ]]; then
		$SUDO $PKG_UPDATE || true
	fi

	# Install packages
	if ! $SUDO $PKG_INSTALL "${translated[@]}"; then
		print_error "Failed to install some packages."
		echo ""
		echo "Please install manually:"
		printf "  %s\n" "${translated[@]}"
		echo ""
		read -p "Press Enter after installing the packages to continue..."
	fi

	return 0
}

# =============================================================================
# CMake Settings Detection
# =============================================================================

detect_existing_cmake_settings() {
	if [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
		return 1
	fi

	local cache="$BUILD_DIR/CMakeCache.txt"

	# Ask user if they want to use existing settings
	if ! whiptail --title "${PROJECT_NAME} ${VERSION} - Existing Build Found" \
		--yesno "We found an existing build configuration in the '$BUILD_DIR' directory.\n\nWould you like to reuse these settings for this installation?" 12 60; then
		return 1
	fi

	print_step "Loading settings from previous build..."

	# Prefix
	local cached_prefix
	cached_prefix=$(grep "^CMAKE_INSTALL_PREFIX:" "$cache" 2>/dev/null | cut -d= -f2)
	[[ -n "$cached_prefix" ]] && PREFIX="$cached_prefix"

	# Sysconfdir
	local cached_sysconfdir
	cached_sysconfdir=$(grep "^CMAKE_INSTALL_SYSCONFDIR:" "$cache" 2>/dev/null | cut -d= -f2)
	[[ -n "$cached_sysconfdir" ]] && SYSCONFDIR="$cached_sysconfdir"

	# Audio
	grep -q "ENABLE_PULSEAUDIO:BOOL=ON" "$cache" 2>/dev/null && ENABLE_PULSEAUDIO=ON
	grep -q "ENABLE_ALSAAUDIO:BOOL=ON" "$cache" 2>/dev/null && ENABLE_ALSAAUDIO=ON

	# MiSTer
	grep -q "ENABLE_MISTER:BOOL=ON" "$cache" 2>/dev/null && ENABLE_MISTER=ON && IS_MISTER=true

	# Devices
	grep -q "ENABLE_NANOLED:BOOL=ON" "$cache" 2>/dev/null && DEVICES+=("NANOLED")
	grep -q "ENABLE_PACDRIVE:BOOL=ON" "$cache" 2>/dev/null && DEVICES+=("PACDRIVE")
	grep -q "ENABLE_PACLED64:BOOL=ON" "$cache" 2>/dev/null && DEVICES+=("PACLED64")
	grep -q "ENABLE_ULTIMATEIO:BOOL=ON" "$cache" 2>/dev/null && DEVICES+=("ULTIMATEIO")
	grep -q "ENABLE_LEDWIZ32:BOOL=ON" "$cache" 2>/dev/null && DEVICES+=("LEDWIZ32")
	grep -q "ENABLE_HOWLER:BOOL=ON" "$cache" 2>/dev/null && DEVICES+=("HOWLER")
	grep -q "ENABLE_ADALIGHT:BOOL=ON" "$cache" 2>/dev/null && DEVICES+=("ADALIGHT")
	grep -q "ENABLE_RASPBERRYPI:BOOL=ON" "$cache" 2>/dev/null && DEVICES+=("RASPBERRYPI") && ENABLE_RASPBERRYPI=ON

	return 0
}

# =============================================================================
# User/Group Detection
# =============================================================================

detect_user() {
	# If running with sudo, get the original user
	if [[ -n "$SUDO_USER" ]]; then
		RUN_USER="$SUDO_USER"
	else
		RUN_USER="$(whoami)"
	fi

	# Get UID and GID
	USER_ID=$(id -u "$RUN_USER" 2>/dev/null || echo "1000")
	GROUP_ID=$(id -g "$RUN_USER" 2>/dev/null || echo "1000")
}

configure_user() {
	detect_user

	local input
	input=$(whiptail --title "${PROJECT_NAME} ${VERSION} - User Configuration" \
		--inputbox "Please enter the username that will run the LEDSpicer program:\n\n(Current detection: User ID $USER_ID, Group ID $GROUP_ID)" 12 60 \
		"$RUN_USER" \
		3>&1 1>&2 2>&3) || return 0

	if [[ -n "$input" && "$input" != "$RUN_USER" ]]; then
		# Validate user exists
		if ! id "$input" &>/dev/null; then
			whiptail --title "${PROJECT_NAME} ${VERSION} - Error" \
				--msgbox "The user '$input' does not exist on this system.\n\nWe'll stick with the default: $RUN_USER" 10 50
			return 0
		fi

		RUN_USER="$input"
		USER_ID=$(id -u "$RUN_USER")
		GROUP_ID=$(id -g "$RUN_USER")
	fi
}

# =============================================================================
# Platform Detection
# =============================================================================

detect_architecture() {
	local machine
	machine=$(uname -m)

	case "$machine" in
		x86_64|amd64)
			ARCH="x86_64"
			IS_ARM=false
			;;
		armv7l|armhf)
			ARCH="armhf"
			IS_ARM=true
			;;
		aarch64|arm64)
			ARCH="arm64"
			IS_ARM=true
			;;
		*)
			ARCH="unknown"
			IS_ARM=false
			;;
	esac
}

select_platform() {
	local options=()
	local default_item=""

	# Build options based on detected architecture
	if [[ "$ARCH" == "x86_64" ]]; then
		options+=("x86_64" "Standard x86_64/amd64 build" ON)
		default_item="x86_64"
	elif [[ "$ARCH" == "armhf" ]]; then
		options+=("armhf" "ARM 32-bit (Raspberry Pi, etc.)" ON)
		options+=("mister" "MiSTer FPGA (ARM 32-bit)" OFF)
		default_item="armhf"
	elif [[ "$ARCH" == "arm64" ]]; then
		options+=("arm64" "ARM 64-bit (Raspberry Pi 4+, etc.)" ON)
		default_item="arm64"
	else
		options+=("x86_64" "Standard x86_64/amd64 build" ON)
		options+=("armhf" "ARM 32-bit (Raspberry Pi, etc.)" OFF)
		options+=("arm64" "ARM 64-bit (Raspberry Pi 4+, etc.)" OFF)
		options+=("mister" "MiSTer FPGA (ARM 32-bit)" OFF)
	fi

	local platform
	platform=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Platform Selection" \
		--radiolist "We've detected your system architecture as: $ARCH\n\nPlease select the target platform for this build:" 16 70 4 \
		"${options[@]}" \
		3>&1 1>&2 2>&3) || exit 1

	case "$platform" in
		mister)
			IS_MISTER=true
			IS_ARM=true
			ENABLE_MISTER=ON
			;;
		armhf|arm64)
			IS_ARM=true
			;;
	esac

	ARCH="$platform"
}

# =============================================================================
# Device Selection
# =============================================================================

select_devices() {
	local options=()

	# Build options with current selections marked
	local sel_nanoled=OFF sel_pacdrive=OFF sel_pacled64=OFF sel_ultimateio=OFF
	local sel_ledwiz32=OFF sel_howler=OFF sel_adalight=OFF sel_raspberrypi=OFF

	for dev in "${DEVICES[@]}"; do
		case "$dev" in
			NANOLED)     sel_nanoled=ON ;;
			PACDRIVE)    sel_pacdrive=ON ;;
			PACLED64)    sel_pacled64=ON ;;
			ULTIMATEIO)  sel_ultimateio=ON ;;
			LEDWIZ32)    sel_ledwiz32=ON ;;
			HOWLER)      sel_howler=ON ;;
			ADALIGHT)    sel_adalight=ON ;;
			RASPBERRYPI) sel_raspberrypi=ON ;;
		esac
	done

	# Common USB devices (always available)
	options+=("NANOLED" "Ultimarc NanoLed" $sel_nanoled)
	options+=("PACDRIVE" "Ultimarc Pac Drive" $sel_pacdrive)
	options+=("PACLED64" "Ultimarc PacLed 64" $sel_pacled64)
	options+=("ULTIMATEIO" "Ultimarc Ipac Ultimate IO" $sel_ultimateio)
	options+=("LEDWIZ32" "Groovy Game Gear Led-Wiz 32" $sel_ledwiz32)
	options+=("HOWLER" "Wolfware Howler" $sel_howler)
	options+=("ADALIGHT" "Adalight Compatible" $sel_adalight)

	# Raspberry Pi GPIO (ARM only)
	if [[ "$IS_ARM" == true ]]; then
		options+=("RASPBERRYPI" "Raspberry Pi GPIO" $sel_raspberrypi)
	fi

	local selected
	selected=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Device Selection" \
		--checklist "Which LED controllers would you like to enable support for?\n(Use spacebar to select/deselect, then Enter to confirm)" 20 70 8 \
		"${options[@]}" \
		3>&1 1>&2 2>&3) || exit 1

	# Parse selected devices
	DEVICES=()
	for device in $selected; do
		# Remove quotes
		device="${device//\"/}"
		DEVICES+=("$device")

		if [[ "$device" == "RASPBERRYPI" ]]; then
			ENABLE_RASPBERRYPI=ON
		fi
	done

	if [[ ${#DEVICES[@]} -eq 0 ]]; then
		if ! whiptail --title "${PROJECT_NAME} ${VERSION} - Warning" \
			--yesno "You haven't selected any devices.\n\nAre you sure you want to proceed without enabling any LED controllers?" 10 60; then
			select_devices
		fi
	fi
}

# =============================================================================
# Audio Selection
# =============================================================================

select_audio() {
	local options=()

	if [[ "$IS_MISTER" == true ]]; then
		# MiSTer: ALSA only
		whiptail --title "${PROJECT_NAME} ${VERSION} - Audio Backend" \
			--msgbox "MiSTer platform detected.\n\nOnly ALSA audio is supported on MiSTer, so it will be enabled automatically." 12 60
		ENABLE_ALSAAUDIO=ON
		ENABLE_PULSEAUDIO=OFF
	else
		local sel_pulse=OFF sel_alsa=OFF
		[[ "$ENABLE_PULSEAUDIO" == "ON" ]] && sel_pulse=ON
		[[ "$ENABLE_ALSAAUDIO" == "ON" ]] && sel_alsa=ON

		options+=("PULSEAUDIO" "PulseAudio (recommended for desktop environments)" $sel_pulse)
		options+=("ALSAAUDIO" "ALSA (for direct hardware access)" $sel_alsa)

		local selected
		selected=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Audio Backend" \
			--checklist "Which audio backends would you like to enable for audio-reactive features?\n(Use spacebar to select/deselect, then Enter to confirm)" 14 70 3 \
			"${options[@]}" \
			3>&1 1>&2 2>&3) || exit 1

		# Reset and parse selected audio backends
		ENABLE_PULSEAUDIO=OFF
		ENABLE_ALSAAUDIO=OFF
		for audio in $selected; do
			audio="${audio//\"/}"
			case "$audio" in
				PULSEAUDIO) ENABLE_PULSEAUDIO=ON ;;
				ALSAAUDIO)  ENABLE_ALSAAUDIO=ON ;;
			esac
		done
	fi
}

# =============================================================================
# Path Configuration
# =============================================================================

configure_paths() {
	# Ask if user wants to change defaults
	if ! whiptail --title "${PROJECT_NAME} ${VERSION} - Installation Paths" \
		--yesno "Current installation paths:\n\n  Prefix:     $PREFIX\n  Config dir: $SYSCONFDIR\n\nWould you like to customize these paths?" 14 60; then
		return
	fi

	# Prefix
	local new_prefix
	new_prefix=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Installation Prefix" \
		--inputbox "Enter the installation prefix:\n(This is where binaries, libraries, etc., will be placed)" 12 70 \
		"$PREFIX" \
		3>&1 1>&2 2>&3) || return

	if [[ -n "$new_prefix" ]]; then
		PREFIX="$new_prefix"
	fi

	# Config dir
	local new_sysconfdir
	new_sysconfdir=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Configuration Directory" \
		--inputbox "Enter the configuration directory:\n(This is where ledspicer.conf will be installed)" 12 70 \
		"$SYSCONFDIR" \
		3>&1 1>&2 2>&3) || return

	if [[ -n "$new_sysconfdir" ]]; then
		SYSCONFDIR="$new_sysconfdir"
	fi
}

# =============================================================================
# Build Configuration
# =============================================================================

generate_cmake_command() {
	local cmd="cmake .."

	# Paths
	cmd+=" -DCMAKE_INSTALL_PREFIX=$PREFIX"
	cmd+=" -DCMAKE_INSTALL_SYSCONFDIR=$SYSCONFDIR"

	# Optimizations.
	cmd+=" -DCMAKE_CXX_FLAGS='-g0 -O3'"

	# MiSTer mode
	if [[ "$ENABLE_MISTER" == "ON" ]]; then
		cmd+=" -DENABLE_MISTER=ON"
	fi

	# Audio backends
	cmd+=" -DENABLE_PULSEAUDIO=$ENABLE_PULSEAUDIO"
	cmd+=" -DENABLE_ALSAAUDIO=$ENABLE_ALSAAUDIO"

	# Devices
	for device in "${DEVICES[@]}"; do
		cmd+=" -DENABLE_$device=ON"
	done

	echo "$cmd"
}

show_summary() {
	local cmake_cmd
	cmake_cmd=$(generate_cmake_command)

	local device_list
	if [[ ${#DEVICES[@]} -eq 0 ]]; then
		device_list="(none)"
	else
		device_list="${DEVICES[*]}"
	fi

	local audio_list=""
	[[ "$ENABLE_PULSEAUDIO" == "ON" ]] && audio_list+="PulseAudio "
	[[ "$ENABLE_ALSAAUDIO" == "ON" ]] && audio_list+="ALSA "
	[[ -z "$audio_list" ]] && audio_list="(none)"

	# Build sudo/install type info
	local install_type
	if [[ "$IS_SYSTEM_INSTALL" == true ]]; then
		install_type="System (sudo required, ldconfig/systemd enabled)"
	else
		install_type="Local (no sudo, ldconfig/systemd skipped)"
	fi

	local summary="
Platform:     $ARCH
MiSTer mode:  $ENABLE_MISTER

Devices:      $device_list
Audio:        $audio_list

Prefix:       $PREFIX
Config dir:   $SYSCONFDIR
Install type: $install_type

CMake command:
$cmake_cmd
"

	whiptail --title "${PROJECT_NAME} ${VERSION} - Configuration Summary" \
		--scrolltext \
		--yesno "$summary\n\nDoes this look correct? Proceed with the build?" 32 78
}

# =============================================================================
# Dependency Checking (based on user selections)
# =============================================================================

check_and_install_dependencies() {
	local missing_packages=()
	local all_deps_info=""
	local need_build_tools=false

	print_step "Checking dependencies for your configuration..."

	# Build tools (grouped under build-essential/base-devel)
	for cmd in make g++; do
		if ! command -v "$cmd" &> /dev/null; then
			need_build_tools=true
			all_deps_info+="  - $cmd (missing)\n"
		else
			all_deps_info+="  * $cmd\n"
		fi
	done

	# Add build-essential/base-devel if make or g++ missing
	if [[ "$need_build_tools" == true ]]; then
		missing_packages+=("build-essential")
	fi

	# Individual tools (not part of build-essential)
	for cmd in cmake pkg-config; do
		if ! command -v "$cmd" &> /dev/null; then
			missing_packages+=("$cmd")
			all_deps_info+="  - $cmd (missing)\n"
		else
			all_deps_info+="  * $cmd\n"
		fi
	done

	# --- Required libraries (always needed) ---
	local required_libs=(
		"libtinyxml2-dev:tinyxml2"
		"libusb-1.0-0-dev:libusb-1.0"
	)

	for lib_entry in "${required_libs[@]}"; do
		local pkg="${lib_entry%%:*}"
		local pc="${lib_entry##*:}"
		if ! pkg-config --exists "$pc" 2>/dev/null; then
			missing_packages+=("$pkg")
			all_deps_info+="  - $pkg (missing)\n"
		else
			all_deps_info+="  * $pkg\n"
		fi
	done

	# --- Optional libraries (based on user selections) ---

	# PulseAudio (if selected)
	if [[ "$ENABLE_PULSEAUDIO" == "ON" ]]; then
		if ! pkg-config --exists libpulse 2>/dev/null; then
			missing_packages+=("libpulse-dev")
			all_deps_info+="  - libpulse-dev (missing)\n"
		else
			all_deps_info+="  * libpulse-dev\n"
		fi
	fi

	# ALSA (if selected)
	if [[ "$ENABLE_ALSAAUDIO" == "ON" ]]; then
		if ! pkg-config --exists alsa 2>/dev/null; then
			missing_packages+=("libasound2-dev")
			all_deps_info+="  - libasound2-dev (missing)\n"
		else
			all_deps_info+="  * libasound2-dev\n"
		fi
	fi

	# pigpio (if Raspberry Pi device selected)
	if [[ "$ENABLE_RASPBERRYPI" == "ON" ]]; then
		if ! ldconfig -p 2>/dev/null | grep -q libpigpio; then
			missing_packages+=("pigpio")
			all_deps_info+="  - pigpio (missing)\n"
		else
			all_deps_info+="  * pigpio\n"
		fi
	fi

	# --- Handle missing packages ---
	if [[ ${#missing_packages[@]} -gt 0 ]]; then
		# Get translated package names for display
		local display_packages
		display_packages=$(get_translated_packages "${missing_packages[@]}")

		local msg="Dependency Status:\n\n$all_deps_info\n\nMissing packages:\n  $display_packages"

		if [[ -z "$PKG_MANAGER" ]]; then
			# No package manager - show what to install manually
			whiptail --title "${PROJECT_NAME} ${VERSION} - Dependencies" \
				--msgbox "$msg\n\nWe couldn't detect a supported package manager.\nPlease install the missing packages manually using your system's tools." 24 78
			echo ""
			echo "Required packages (install manually):"
			printf "  %s\n" $display_packages
			echo ""
			read -p "Press Enter after installing the packages to continue..."
		elif whiptail --title "${PROJECT_NAME} ${VERSION} - Dependencies" \
			--yesno "$msg\n\nWould you like us to install the missing packages now using $PKG_MANAGER?" 24 78; then
			install_packages "${missing_packages[@]}"
		else
			print_error "Cannot continue without required dependencies."
			echo ""
			echo "Required packages:"
			printf "  %s\n" $display_packages
			exit 1
		fi
	else
		echo -e "$all_deps_info"
		echo ""
		print_step "All dependencies satisfied!"
	fi
}

# =============================================================================
# Build Process
# =============================================================================

run_build() {
	local cmake_cmd
	cmake_cmd=$(generate_cmake_command)

	print_step "Creating build directory..."
	mkdir -p "$BUILD_DIR"
	cd "$BUILD_DIR"

	print_step "Running CMake..."
	echo -e "${YELLOW}$cmake_cmd${NC}"
	rm -f CMakeCache.txt
	eval "$cmake_cmd"

	print_step "Building ${PROJECT_NAME} ${VERSION}..."
	local jobs
	jobs=$(nproc 2>/dev/null || echo 2)
	make -j"$jobs"

	echo ""
	print_step "Build completed successfully!"
	cd ..
}

run_install() {
	if whiptail --title "${PROJECT_NAME} ${VERSION} - Install" \
		--yesno "The build is complete!\n\nWould you like to install LEDSpicer now?\n\n(Note: This may require administrative privileges for system directories)" 14 70; then

		print_step "Installing ${PROJECT_NAME} ${VERSION}..."
		cd "$BUILD_DIR"
		$SUDO_INSTALL make install
		cd ..

		# Refresh library cache (only for system installs)
		if [[ "$IS_SYSTEM_INSTALL" == true ]] && command -v ldconfig &>/dev/null; then
			print_step "Updating library cache..."
			$SUDO ldconfig
		fi

		# Run post-install configuration
		post_install_configuration

		local msg="${PROJECT_NAME} ${VERSION} installed successfully!"
		local box_width=67
		local pad_left=$(( (box_width - ${#msg}) / 2 ))
		local pad_right=$(( box_width - ${#msg} - pad_left ))

		echo ""
		echo -e "${GREEN}╔═══════════════════════════════════════════════════════════════════╗${NC}"
		printf "${GREEN}║%*s%s%*s║${NC}\n" "$pad_left" "" "$msg" "$pad_right" ""
		echo -e "${GREEN}╚═══════════════════════════════════════════════════════════════════╝${NC}"
		echo ""
		echo "Next steps:"
		echo "  1. Edit configuration: $SYSCONFDIR/ledspicer.conf"
		if [[ "$IS_DAEMON" == true ]]; then
			echo "  2. Start the daemon:   sudo systemctl start ledspicerd"
			echo "  3. Enable at boot:     sudo systemctl enable ledspicerd"
		else
			echo "  2. Start the program manually: ledspicerd"
			echo "     (You may need to log out and back in for permission changes to take effect)"
		fi
		echo ""
		echo "Documentation: https://github.com/meduzapat/LEDSpicer/wiki"
	else
		echo ""
		echo "Build completed. To install later, run:"
		if [[ -n "$SUDO_INSTALL" ]]; then
			echo "  cd $BUILD_DIR && sudo make install"
		else
			echo "  cd $BUILD_DIR && make install"
		fi
	fi
}

# =============================================================================
# Post-Install Configuration
# =============================================================================

# Map device option names to XML config names
get_device_xml_name() {
	local device="$1"
	case "$device" in
		ULTIMATEIO)  echo "UltimarcUltimate" ;;
		PACLED64)    echo "UltimarcPacLed64" ;;
		PACDRIVE)    echo "UltimarcPacDrive" ;;
		NANOLED)     echo "UltimarcNanoLed" ;;
		LEDWIZ32)    echo "LedWiz32" ;;
		HOWLER)      echo "Howler" ;;
		ADALIGHT)    echo "Adalight" ;;
		RASPBERRYPI) echo "RaspberryPi" ;;
		*)           echo "$device" ;;
	esac
}

post_install_configuration() {
	detect_user

	# For non-system installs, skip daemon/service configuration entirely
	if [[ "$IS_SYSTEM_INSTALL" == false ]]; then
		print_step "Local install detected - skipping system service configuration."

		# Only offer config file creation
		if [[ ! -f "$SYSCONFDIR/ledspicer.conf" ]]; then
			if whiptail --title "${PROJECT_NAME} ${VERSION} - Configuration" \
				--yesno "Would you like to create a basic configuration file?\n\nLocation: $SYSCONFDIR/ledspicer.conf" 10 60; then
				IS_DAEMON=false
				configure_user
				create_skeleton_config
			fi
		fi
		return
	fi

	print_step "Configuring run mode..."

	if whiptail --title "${PROJECT_NAME} ${VERSION} - Daemon Run Mode" \
		--yesno "Would you like the ledspicerd daemon to run as root (system service)?\n\n- Yes: Runs automatically as a system daemon (recommended for most users)\n- No: Runs manually as a regular user (for custom or testing setups)" 14 70; then
		IS_DAEMON=true
		RUN_USER="root"
		USER_ID=0
		GROUP_ID=0
		comment_pulse_lines=false
		PULSE_USER_ID=1000
		if [[ "$ENABLE_PULSEAUDIO" == "ON" ]]; then
			if whiptail --title "${PROJECT_NAME} ${VERSION} - PulseAudio Configuration" \
				--yesno "Is your PulseAudio session running under a different user than root?\n\n(This is common in desktop environments; select Yes if unsure)" 14 70; then
				PULSE_USER_ID=$(whiptail --inputbox "Enter the user ID for the PulseAudio session:\n\n(Usually your main user's ID, like 1000 - enter to confirm)" 14 70 "1000" 3>&1 1>&2 2>&3)
				if [[ -z "$PULSE_USER_ID" ]]; then PULSE_USER_ID=1000; fi
			else
				comment_pulse_lines=true
			fi
		fi
	else
		IS_DAEMON=false
		configure_user
	fi

	# Build options dynamically based on system state
	local options=()

	# Config file (only if missing)
	if [[ ! -f "$SYSCONFDIR/ledspicer.conf" ]]; then
		options+=("config" "Create a basic configuration file template" ON)
	fi

	# Service update (system installs only)
	if [[ "$IS_DAEMON" == true && ("$PULSE_USER_ID" != "1000" || "$comment_pulse_lines" == true) ]]; then
		options+=("ledspicerd" "Update ledspicerd service (PulseAudio settings)" ON)
	fi

	# Udev rules (system installs only)
	local rules_src="${PREFIX}/share/doc/${PROJECT_NAME}/examples/21-ledspicer.rules"
	local rules_dst="/etc/udev/rules.d/21-ledspicer.rules"
	local udev_desc="Install udev rules (for USB device permissions)"
	if [[ -f "$rules_dst" ]]; then
		udev_desc="Update udev rules (for USB device permissions)"
	fi

	if [[ "$IS_DAEMON" == false || ! -f "$rules_dst" ]] || ! diff -q "$rules_src" "$rules_dst" &>/dev/null; then
		options+=("udev" "$udev_desc" ON)
	fi

	# Skip if nothing to configure
	if [[ ${#options[@]} -eq 0 ]]; then
		print_step "No additional configuration needed."
	else
		# Show checklist
		local selected
		selected=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Post-Install Configuration" \
			--checklist "Please select the configuration tasks you'd like to perform:\n\nRun user: $RUN_USER (UID: $USER_ID, GID: $GROUP_ID)" 18 70 4 \
			"${options[@]}" \
			3>&1 1>&2 2>&3) || return

		# Track if we need to reload systemd
		local need_daemon_reload=false

		# Execute selected tasks
		for item in $selected; do
			item="${item//\"/}"
			case "$item" in
				config)      create_skeleton_config ;;
				ledspicerd)  update_ledspicerd_service && need_daemon_reload=true ;;
				udev)        install_udev_rules ;;
			esac
		done

		# Single daemon-reload at end if needed
		if [[ "$need_daemon_reload" == true ]] && command -v systemctl &>/dev/null; then
			print_step "Reloading systemd daemon..."
			$SUDO systemctl daemon-reload
		fi
	fi

	# Handle service enable/disable (system installs only)
	if command -v systemctl &>/dev/null; then
		if [[ "$IS_DAEMON" == true ]]; then
			if whiptail --title "${PROJECT_NAME} ${VERSION} - Enable Service" \
				--yesno "Would you like to enable the ledspicerd service to start automatically at boot?" 10 60; then
				print_step "Enabling ledspicerd service..."
				$SUDO systemctl enable ledspicerd
			fi
		else
			if $SUDO systemctl is-enabled ledspicerd &>/dev/null; then
				print_step "Disabling ledspicerd service (since running in non-daemon mode)..."
				$SUDO systemctl disable ledspicerd
			fi
		fi
	fi

	# Configure user groups for non-daemon mode
	if [[ "$IS_DAEMON" == false && -n "$RUN_USER" && "$RUN_USER" != "root" ]]; then
		configure_user_groups "$RUN_USER"
	fi
}

create_skeleton_config() {
	# Backup existing config if present
	if [[ -f "$SYSCONFDIR/ledspicer.conf" ]]; then
		print_step "Backing up existing config..."
		$SUDO_CONF cp "$SYSCONFDIR/ledspicer.conf" "$SYSCONFDIR/ledspicer.conf.bak.$(date +%Y%m%d%H%M%S)"
	fi

	print_step "Creating skeleton configuration file..."

	# Build devices XML
	local devices_xml=""
	for device in "${DEVICES[@]}"; do
		local xml_name
		xml_name=$(get_device_xml_name "$device")
		devices_xml+="\t\t<device name=\"$xml_name\">\n\t\t</device>\n"
	done

	# If no devices, add empty placeholder
	if [[ -z "$devices_xml" ]]; then
		devices_xml="\t\t<device name=\"\">\n\t\t</device>\n"
	fi

	# Create config file
	local config_content="<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<LEDSpicer
	version=\"${DATA_VERSION}\"
	type=\"Configuration\"
	fps=\"15\"
	port=\"16161\"
	colors=\"basicColors\"
	logLevel=\"Debug\"
	userId=\"${USER_ID}\"
	groupId=\"${GROUP_ID}\"
	craftProfile=\"true\"
	dataSource=\"controls.ini,file,mame\"
	colorsFile=\"true\"
>
	<restrictors>
	</restrictors>
	<devices>
		$devices_xml
	</devices>
	<layout defaultProfile=\"default\">
	</layout>
</LEDSpicer>"

	echo "$config_content" | $SUDO_CONF tee "$SYSCONFDIR/ledspicer.conf" > /dev/null

	print_step "Configuration file created at $SYSCONFDIR/ledspicer.conf"
}

update_ledspicerd_service() {
	print_step "Updating ledspicerd service file..."

	local service_dir
	service_dir=$(pkg-config systemd --variable=systemdsystemunitdir 2>/dev/null)
	[[ -z "$service_dir" ]] && service_dir="/usr/lib/systemd/system"

	local service_file="$service_dir/ledspicerd.service"

	if [[ ! -f "$service_file" ]]; then
		print_warning "Service file not found at $service_file"
		print_warning "You may need to update the user ID manually after locating the file."
		return 1
	fi

	if [[ "$comment_pulse_lines" == true ]]; then
		$SUDO sed -i "/^ExecStartPre=\/bin\/sh -c 'for i in {1..30}; do \[ -S \/run\/user\/.*pulse\/native \] && exit 0; sleep 1; done; exit 1'/s/^/#/" "$service_file"
		$SUDO sed -i "/^Environment=PULSE_SERVER=unix:\/run\/user\/.*pulse\/native/s/^/#/" "$service_file"
	else
		$SUDO sed -i "s|/run/user/1000/|/run/user/${PULSE_USER_ID}/|g" "$service_file"
	fi

	print_step "Service file updated."
	return 0
}

install_udev_rules() {
	print_step "Installing udev rules..."

	local rules_src="${PREFIX}/share/doc/${PROJECT_NAME}/examples/21-ledspicer.rules"
	local rules_dst="/etc/udev/rules.d/21-ledspicer.rules"

	if [[ ! -f "$rules_src" ]]; then
		print_warning "Udev rules file not found at $rules_src"
		return 1
	fi

	$SUDO cp "$rules_src" "$rules_dst"

	# Reload udev rules
	if command -v udevadm &>/dev/null; then
		$SUDO udevadm control --reload-rules
		$SUDO udevadm trigger
		print_step "Udev rules installed and reloaded."
		print_warning "You may need to replug USB devices for changes to take effect."
	else
		print_step "Udev rules installed."
	fi

	return 0
}

configure_user_groups() {
	local target_user="$1"
	local groups_added=()
	local groups_skipped=()
	local groups_missing=()

	print_step "Checking group membership for user '$target_user'..."

	# Define required groups based on selected devices
	local required_groups=()

	# USB device access (always needed for USB devices)
	local has_usb_device=false
	for dev in "${DEVICES[@]}"; do
		case "$dev" in
			NANOLED|PACDRIVE|PACLED64|ULTIMATEIO|LEDWIZ32|HOWLER)
				has_usb_device=true
				break
				;;
		esac
	done

	if [[ "$has_usb_device" == true ]]; then
		required_groups+=("users:USB device access")
		required_groups+=("input:USB device access")
	fi

	# Serial devices (Adalight)
	for dev in "${DEVICES[@]}"; do
		if [[ "$dev" == "ADALIGHT" ]]; then
			required_groups+=("dialout:serial device access (Adalight)")
			break
		fi
	done

	# Raspberry Pi GPIO
	for dev in "${DEVICES[@]}"; do
		if [[ "$dev" == "RASPBERRYPI" ]]; then
			required_groups+=("gpio:Raspberry Pi GPIO access")
			break
		fi
	done

	# No groups needed
	if [[ ${#required_groups[@]} -eq 0 ]]; then
		echo "  No special group membership required for selected devices."
		return 0
	fi

	# Check and add each group
	for entry in "${required_groups[@]}"; do
		local group="${entry%%:*}"
		local reason="${entry##*:}"

		# Check if group exists
		if ! getent group "$group" &>/dev/null; then
			groups_missing+=("$group ($reason)")
			continue
		fi

		# Check if user is already in group
		if id -nG "$target_user" 2>/dev/null | grep -qw "$group"; then
			groups_skipped+=("$group")
		else
			# Add user to group
			if $SUDO usermod -aG "$group" "$target_user" 2>/dev/null; then
				groups_added+=("$group")
			else
				groups_missing+=("$group (failed to add)")
			fi
		fi
	done

	# Report results
	if [[ ${#groups_added[@]} -gt 0 ]]; then
		echo -e "  ${GREEN}Added to groups:${NC} ${groups_added[*]}"
	fi

	if [[ ${#groups_skipped[@]} -gt 0 ]]; then
		echo "  Already member of: ${groups_skipped[*]}"
	fi

	if [[ ${#groups_missing[@]} -gt 0 ]]; then
		echo -e "  ${YELLOW}Groups not available:${NC} ${groups_missing[*]}"
	fi

	if [[ ${#groups_added[@]} -gt 0 ]]; then
		print_warning "You must log out and back in for group changes to take effect."
	fi

	return 0
}

# =============================================================================
# Main
# =============================================================================

main() {
	check_whiptail
	check_root
	detect_version
	detect_package_manager
	detect_user

	print_banner
	sleep 1

	# Check for existing CMake settings
	if detect_existing_cmake_settings; then
		print_step "Loaded settings from previous build."
	fi

	# Platform detection and selection
	detect_architecture
	select_platform

	# Configure options
	select_devices
	select_audio
	configure_paths

	# Update sudo requirements based on selected paths
	update_sudo_for_paths

	# Show summary and confirm
	if ! show_summary; then
		print_warning "Build cancelled by user."
		exit 0
	fi

	# Check and install dependencies (based on selections)
	check_and_install_dependencies

	# Build
	run_build

	# Install
	run_install
}

# Run main function
main "$@"
