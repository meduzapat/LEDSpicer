#!/bin/bash
# LEDSpicer PPA Package Testing Script
# Tests packages from PPA on Ubuntu LTS

set -e
source "$(dirname "$0")/common.sh"

PPA_REPO="ppa:meduzapat/ledspicer"
# Auto-detect latest Ubuntu LTS from Docker Hub
UBUNTU_VERSION=$(curl -s https://registry.hub.docker.com/v2/repositories/library/ubuntu/tags?page_size=100 | grep -oP '"name":"\K[0-9]+\.04(?=")' | sort -t. -k1,1n | tail -1) || true
[ -n "$UBUNTU_VERSION" ] || { print_error "Failed to fetch Ubuntu LTS version"; exit 1; }
ARCHITECTURES=("amd64" "arm64")

test_ubuntu_version() {
	local version=$1 arch=$2 container_name="ledspicer-ppa-test-${version}-${arch}"

	print_step "Testing Ubuntu ${version} (${arch})"

	docker run --rm -it --platform linux/${arch} --name ${container_name} ubuntu:${version} bash -c "
		set -e
		export DEBIAN_FRONTEND=noninteractive
		apt-get update && apt-get install -y software-properties-common
		add-apt-repository -y ${PPA_REPO} && apt-get update
		apt-get install -y ledspicer ledspicer-doc

		command -v ledspicerd >/dev/null && command -v processLookup >/dev/null && command -v inputseeker >/dev/null && ledspicerd --version

		plugin_count=\$(find /usr/lib/*/ledspicer/devices/ -name '*.so' 2>/dev/null | wc -l)
		[ \$plugin_count -eq 0 ] || { echo 'ERROR: Base package contains plugins'; exit 1; }

		ls /usr/share/doc/ledspicer/examples/ >/dev/null 2>&1 || { echo 'ERROR: Examples directory missing'; exit 1; }

		PLUGINS=('ledspicer-nanoled' 'ledspicer-pacdrive' 'ledspicer-pacled64' 'ledspicer-ultimateio' 'ledspicer-ledwiz32' 'ledspicer-howler' 'ledspicer-adalight')
		for plugin in \${PLUGINS[@]}; do apt-get install -y \$plugin || { echo \"ERROR: Failed to install \$plugin\"; exit 1; }; done

		plugin_count=\$(find /usr/lib/*/ledspicer/devices/ -name '*.so' 2>/dev/null | wc -l)
		[ \$plugin_count -eq 7 ] || { echo \"ERROR: Expected 7 plugins, found \$plugin_count\"; find /usr/lib/*/ledspicer/devices/ -name '*.so' 2>/dev/null; exit 1; }
	"

	if [ $? -eq 0 ]; then
		print_step "✓ Ubuntu ${version} (${arch}) PASSED"
		return 0
	else
		print_error "✗ Ubuntu ${version} (${arch}) FAILED"
		return 1
	fi
}

main() {
	print_step "Starting tests (Ubuntu $UBUNTU_VERSION)"
	local failed=0 total=0 failed_tests=""

	for arch in "${ARCHITECTURES[@]}"; do
		total=$((total + 1))
		if ! test_ubuntu_version "$UBUNTU_VERSION" "$arch"; then
			failed=$((failed + 1))
			failed_tests+="  - Ubuntu ${UBUNTU_VERSION} (${arch})\n"
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
