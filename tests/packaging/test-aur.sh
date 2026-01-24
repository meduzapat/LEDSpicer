#!/bin/bash
# LEDSpicer AUR Package Testing Script
# Tests packages from AUR on Arch Linux

set -e
source "$(dirname "$0")/common.sh"

AUR_PACKAGE="ledspicer"
ARCHITECTURES=("x86_64" "aarch64")

test_arch_linux() {
	local arch=$1 container_name="ledspicer-aur-test-${arch}"
	
	print_step "Testing Arch Linux (${arch})"
	
	docker run --rm -it --platform linux/${arch} --name ${container_name} archlinux:latest bash -c "
		set -e
		pacman -Syu --noconfirm
		pacman -S --noconfirm base-devel git sudo
		
		useradd -m builder
		echo 'builder ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers
		
		su - builder -c 'git clone https://aur.archlinux.org/yay.git && cd yay && makepkg -si --noconfirm'
		su - builder -c 'yay -S --noconfirm ${AUR_PACKAGE}'
		
		command -v ledspicerd >/dev/null && command -v processLookup >/dev/null && command -v inputseeker >/dev/null && ledspicerd --version
		
		plugin_count=\$(find /usr/lib/ledspicer/devices/ -name '*.so' 2>/dev/null | wc -l)
		[ \$plugin_count -eq 0 ] || { echo 'ERROR: Base package contains plugins'; exit 1; }
		
		ls /usr/share/doc/ledspicer/examples/ >/dev/null 2>&1 || { echo 'ERROR: Examples directory missing'; exit 1; }
		
		PLUGINS=('ledspicer-nanoled' 'ledspicer-pacdrive' 'ledspicer-pacled64' 'ledspicer-ultimateio' 'ledspicer-ledwiz32' 'ledspicer-howler' 'ledspicer-adalight')
		for plugin in \${PLUGINS[@]}; do su - builder -c \"yay -S --noconfirm \$plugin\" || { echo \"ERROR: Failed to install \$plugin\"; exit 1; }; done
		
		plugin_count=\$(find /usr/lib/ledspicer/devices/ -name '*.so' 2>/dev/null | wc -l)
		[ \$plugin_count -eq 7 ] || { echo \"ERROR: Expected 7 plugins, found \$plugin_count\"; ls -la /usr/lib/ledspicer/devices/; exit 1; }
	"
	
	if [ $? -eq 0 ]; then
		print_step "✓ Arch Linux (${arch}) PASSED"
		return 0
	else
		print_error "✗ Arch Linux (${arch}) FAILED"
		return 1
	fi
}

main() {
	print_step "Starting tests (Arch Linux latest)"
	local failed=0 total=0 failed_tests=""
	
	for arch in "${ARCHITECTURES[@]}"; do
		total=$((total + 1))
		if ! test_arch_linux "$arch"; then
			failed=$((failed + 1))
			failed_tests+="  - Arch Linux (${arch})\n"
		fi
		echo ""
	done
	
	echo "======================================"
	echo "Results: Total $total, Passed $((total - failed)), Failed $failed"
	[ -z "$failed_tests" ] || echo -e "Failed:\n$failed_tests"
	echo "======================================"
	[ $failed -eq 0 ] || exit 1
}

check_docker
main "$@"
