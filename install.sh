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
		--yesno "Found existing CMake configuration in '$BUILD_DIR'.\n\nWould you like to load these settings?" 10 60; then
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
		--inputbox "Enter the username that will run LEDSpicer daemons:\n\n(User ID: $USER_ID, Group ID: $GROUP_ID)" 12 60 \
		"$RUN_USER" \
		3>&1 1>&2 2>&3) || return 0

	if [[ -n "$input" && "$input" != "$RUN_USER" ]]; then
		# Validate user exists
		if ! id "$input" &>/dev/null; then
			whiptail --title "${PROJECT_NAME} ${VERSION} - Error" \
				--msgbox "User '$input' does not exist.\n\nUsing default: $RUN_USER" 10 50
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
		--radiolist "Detected architecture: $ARCH\n\nSelect target platform:" 16 70 4 \
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
		--checklist "Select the LED controllers you want to enable:\n(Space to select, Enter to confirm)" 20 70 8 \
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
			--yesno "No devices selected!\n\nAre you sure you want to continue without any LED controllers?" 10 60; then
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
			--msgbox "MiSTer platform detected.\n\nOnly ALSA audio is available on MiSTer.\nALSA will be enabled automatically." 12 60
		ENABLE_ALSAAUDIO=ON
		ENABLE_PULSEAUDIO=OFF
	else
		local sel_pulse=OFF sel_alsa=OFF
		[[ "$ENABLE_PULSEAUDIO" == "ON" ]] && sel_pulse=ON
		[[ "$ENABLE_ALSAAUDIO" == "ON" ]] && sel_alsa=ON

		options+=("PULSEAUDIO" "PulseAudio (recommended for desktop)" $sel_pulse)
		options+=("ALSAAUDIO" "ALSA (direct hardware access)" $sel_alsa)

		local selected
		selected=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Audio Backend" \
			--checklist "Select audio backends for audio-reactive animations:\n(Space to select, Enter to confirm)" 14 70 3 \
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
		--yesno "Current installation paths:\n\n  Prefix:     $PREFIX\n  Config dir: $SYSCONFDIR\n\nWould you like to change these?" 14 60; then
		return
	fi

	# Prefix
	local new_prefix
	new_prefix=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Installation Prefix" \
		--inputbox "Enter installation prefix:\n(binaries go to PREFIX/bin, libraries to PREFIX/lib, etc.)" 12 70 \
		"$PREFIX" \
		3>&1 1>&2 2>&3) || return

	if [[ -n "$new_prefix" ]]; then
		PREFIX="$new_prefix"
	fi

	# Config dir
	local new_sysconfdir
	new_sysconfdir=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Configuration Directory" \
		--inputbox "Enter configuration directory:\n(ledspicer.conf will be installed here)" 12 70 \
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

	local summary="
Platform:     $ARCH
MiSTer mode:  $ENABLE_MISTER

Devices:      $device_list
Audio:        $audio_list

Prefix:       $PREFIX
Config dir:   $SYSCONFDIR

CMake command:
$cmake_cmd
"

	whiptail --title "${PROJECT_NAME} ${VERSION} - Configuration Summary" \
		--scrolltext \
		--yesno "$summary\n\nProceed with build?" 30 78
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

	# Add build-essential/base-devel if make or g++ missing
	if [[ "$need_build_tools" == true ]]; then
		missing_packages+=("build-essential")
	fi

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
				--msgbox "$msg\n\nNo supported package manager found.\nPlease install the missing packages manually." 24 78
			echo ""
			echo "Required packages (install manually):"
			printf "  %s\n" $display_packages
			echo ""
			read -p "Press Enter after installing the packages to continue..."
		elif whiptail --title "${PROJECT_NAME} ${VERSION} - Dependencies" \
			--yesno "$msg\n\nInstall missing packages now using $PKG_MANAGER?" 24 78; then
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
		--yesno "Build completed successfully!\n\nWould you like to install LEDSpicer now?\n\n(This will require sudo privileges if installing to system directories)" 14 70; then

		print_step "Installing ${PROJECT_NAME} ${VERSION}..."
		cd "$BUILD_DIR"
		$SUDO make install
		cd ..

		# Refresh library cache
		if command -v ldconfig &>/dev/null; then
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
		echo "  2. Start the daemon:   sudo systemctl start ledspicerd"
		echo "  3. Enable at boot:     sudo systemctl enable ledspicerd"
		echo ""
		echo "Documentation: https://github.com/meduzapat/LEDSpicer/wiki"
	else
		echo ""
		echo "Build completed. To install later, run:"
		echo "  cd $BUILD_DIR && sudo make install"
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
	# Configure user first
	configure_user

	# Build options dynamically based on system state
	local options=()

	# Config file (only if missing)
	if [[ ! -f "$SYSCONFDIR/ledspicer.conf" ]]; then
		options+=("config" "Create skeleton configuration file" ON)
	fi

	# Service update (only if UID differs from default 1000)
	if [[ "$USER_ID" != "1000" ]]; then
		options+=("ledspicerd" "Update ledspicerd service (UID: $USER_ID)" ON)
	fi

	# Udev rules (if missing or different)
	local rules_src="${PREFIX}/share/doc/${PROJECT_NAME}/examples/21-ledspicer.rules"
	local rules_dst="/etc/udev/rules.d/21-ledspicer.rules"

	if [[ ! -f "$rules_dst" ]] || ! diff -q "$rules_src" "$rules_dst" &>/dev/null 2>&1; then
		options+=("udev" "Install udev rules (USB permissions)" ON)
	fi

	# Skip if nothing to configure
	if [[ ${#options[@]} -eq 0 ]]; then
		print_step "No additional configuration needed."
		return
	fi

	# Show checklist
	local selected
	selected=$(whiptail --title "${PROJECT_NAME} ${VERSION} - Post-Install Configuration" \
		--checklist "Select additional configuration tasks:\n\nUser: $RUN_USER (UID: $USER_ID, GID: $GROUP_ID)" 18 70 4 \
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
}

create_skeleton_config() {
	# Backup existing config if present
	if [[ -f "$SYSCONFDIR/ledspicer.conf" ]]; then
		print_step "Backing up existing config..."
		$SUDO cp "$SYSCONFDIR/ledspicer.conf" "$SYSCONFDIR/ledspicer.conf.bak.$(date +%Y%m%d%H%M%S)"
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
$(echo -e "$devices_xml")	</devices>
	<layout defaultProfile=\"default\">
	</layout>
</LEDSpicer>"

	echo "$config_content" | $SUDO tee "$SYSCONFDIR/ledspicer.conf" > /dev/null

	print_step "Configuration file created at $SYSCONFDIR/ledspicer.conf"
}

update_ledspicerd_service() {
	print_step "Updating ledspicerd service file with user ID ${USER_ID}..."

	# Get systemd unit directory (same logic as CMake)
	local service_dir
	service_dir=$(pkg-config systemd --variable=systemdsystemunitdir 2>/dev/null)
	if [[ -z "$service_dir" ]]; then
		service_dir="/usr/lib/systemd/system"
	fi

	local service_file="$service_dir/ledspicerd.service"

	if [[ ! -f "$service_file" ]]; then
		print_warning "Service file not found at $service_file"
		print_warning "You may need to update the user ID manually after locating the file."
		return 1
	fi

	# Replace default 1000 with actual UID in the service file
	$SUDO sed -i "s|/run/user/1000/|/run/user/${USER_ID}/|g" "$service_file"

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
