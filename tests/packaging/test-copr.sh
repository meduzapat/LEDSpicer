#!/bin/bash
# LEDSpicer COPR Package Testing Script
# Tests packages from COPR on Fedora

set -e
source "$(dirname "$0")/common.sh"

COPR_REPO="meduzapat/ledspicer"
FEDORA_VERSION=$(curl -s https://registry.hub.docker.com/v2/repositories/library/fedora/tags | grep -oP '"name":"\K\d+(?=")' | sort -n | tail -1) || true
[ -n "$FEDORA_VERSION" ] && [[ "$FEDORA_VERSION" =~ ^[0-9]+$ ]] || { print_error "Failed to fetch Fedora version"; exit 1; }
ARCHITECTURES=("x86_64" "aarch64")

test_fedora_version() {
	local version=$1 arch=$2 container_name="ledspicer-copr-test-f${version}-${arch}"

	print_step "Testing Fedora ${version} (${arch})"

	docker run --rm -it --platform linux/${arch} --name ${container_name} fedora:${version} bash -c "
		set -e
		dnf install -y 'dnf-command(copr)'
		dnf copr enable -y ${COPR_REPO}
		dnf install -y ledspicer --setopt=tsflags=

		command -v ledspicerd >/dev/null && command -v processLookup >/dev/null && command -v inputseeker >/dev/null && ledspicerd --version

		plugin_count=\$(find /usr/lib64/ledspicer/devices/ /usr/lib/ledspicer/devices/ -name '*.so' 2>/dev/null | wc -l)
		[ \$plugin_count -eq 0 ] || { echo 'ERROR: Base package contains plugins'; exit 1; }

		ls /usr/share/doc/ledspicer/examples >/dev/null 2>&1 || { echo 'ERROR: Examples directory missing'; exit 1; }

		PLUGINS=('ledspicer-nanoled' 'ledspicer-pacdrive' 'ledspicer-pacled64' 'ledspicer-ultimateio' 'ledspicer-ledwiz32' 'ledspicer-howler' 'ledspicer-adalight')
		for plugin in \${PLUGINS[@]}; do dnf install -y \$plugin || { echo \"ERROR: Failed to install \$plugin\"; exit 1; }; done

		plugin_count=\$(find /usr/lib64/ledspicer/devices/ /usr/lib/ledspicer/devices/ -name '*.so' 2>/dev/null | wc -l)
		[ \$plugin_count -eq 7 ] || { echo \"ERROR: Expected 7 plugins, found \$plugin_count\"; exit 1; }
	"

	if [ $? -eq 0 ]; then
		print_step "✓ Fedora ${version} (${arch}) PASSED"
		return 0
	else
		print_error "✗ Fedora ${version} (${arch}) FAILED"
		return 1
	fi
}

main() {
	print_step "Starting tests (Fedora $FEDORA_VERSION)"
	local failed=0 total=0 failed_tests=""

	for arch in "${ARCHITECTURES[@]}"; do
		total=$((total + 1))
		if ! test_fedora_version "$FEDORA_VERSION" "$arch"; then
			failed=$((failed + 1))
			failed_tests+="  - Fedora ${FEDORA_VERSION} (${arch})\n"
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
