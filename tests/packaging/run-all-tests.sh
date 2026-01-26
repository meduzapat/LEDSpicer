#!/bin/bash
# LEDSpicer Package Testing - Master Test Runner
# Runs all distribution tests in sequence

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_header() {
	echo -e "${BLUE}"
	echo "╔═══════════════════════════════════════════════════════════════╗"
	echo "║                LEDSpicer Package Testing Suite                ║"
	echo "╚═══════════════════════════════════════════════════════════════╝"
	echo -e "${NC}"
}

print_step() {
	echo -e "\n${GREEN}==>${NC} $1"
}

print_warning() {
	echo -e "${YELLOW}Warning:${NC} $1"
}

print_error() {
	echo -e "${RED}Error:${NC} $1"
}

print_summary() {
	local total=$1
	local passed=$2
	local failed=$3
	local skipped=$4
	
	echo -e "\n${BLUE}╔═══════════════════════════════════════════════════════════════╗${NC}"
	echo -e "${BLUE}║                    TEST SUMMARY                               ║${NC}"
	echo -e "${BLUE}╠═══════════════════════════════════════════════════════════════╣${NC}"
	printf "${BLUE}║${NC} Total Tests:   ${YELLOW}%-47d${NC}${BLUE}║${NC}\n" $total
	printf "${BLUE}║${NC} Passed:        ${GREEN}%-47d${NC}${BLUE}║${NC}\n" $passed
	printf "${BLUE}║${NC} Failed:        ${RED}%-47d${NC}${BLUE}║${NC}\n" $failed
	printf "${BLUE}║${NC} Skipped:       ${YELLOW}%-47d${NC}${BLUE}║${NC}\n" $skipped
	echo -e "${BLUE}╚═══════════════════════════════════════════════════════════════╝${NC}"
}

check_docker() {
	if ! command -v docker &> /dev/null; then
		print_error "Docker is required for automated testing"
		echo "Install Docker: https://docs.docker.com/get-docker/"
		echo ""
		echo "Alternatively, run tests manually:"
		echo "  - See TESTING.md for manual testing instructions"
		return 1
	fi
	return 0
}

# Test results tracking
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
SKIPPED_TESTS=0

run_test_suite() {
	local test_name=$1
	local test_script=$2
	
	TOTAL_TESTS=$((TOTAL_TESTS + 1))
	
	print_step "Running ${test_name}..."
	
	if [ ! -f "$test_script" ]; then
		print_error "Test script not found: $test_script"
		SKIPPED_TESTS=$((SKIPPED_TESTS + 1))
		return 1
	fi
	
	if bash "$test_script"; then
		PASSED_TESTS=$((PASSED_TESTS + 1))
		print_step "✓ ${test_name} PASSED"
		return 0
	else
		FAILED_TESTS=$((FAILED_TESTS + 1))
		print_error "✗ ${test_name} FAILED"
		return 1
	fi
}

# Main test execution
main() {
	print_header
	
	# Check prerequisites
	if ! check_docker; then
		echo ""
		echo "Cannot run automated tests without Docker."
		echo "Exiting..."
		exit 1
	fi
	
	echo "This will run package tests for:"
	echo "  - PPA (Ubuntu)"
	echo "  - COPR (Fedora)"
	echo "  - AUR (Arch Linux)"
	echo ""
	
	read -p "Continue with automated testing? [y/N] " -n 1 -r
	echo
	if [[ ! $REPLY =~ ^[Yy]$ ]]; then
		echo "Testing cancelled."
		exit 0
	fi
	
	# Run PPA tests
	run_test_suite "PPA (Ubuntu) Tests" "./test-ppa.sh" || true
	
	# Run COPR tests
	run_test_suite "COPR (Fedora) Tests" "./test-copr.sh" || true
	
	# Run AUR tests
	run_test_suite "AUR (Arch) Tests" "./test-aur.sh" || true
	
	# Print summary
	print_summary $TOTAL_TESTS $PASSED_TESTS $FAILED_TESTS $SKIPPED_TESTS
	
	# Generate report
	generate_report
	
	# Exit with appropriate code
	if [ $FAILED_TESTS -gt 0 ]; then
		exit 1
	fi
}

generate_report() {
	local report_file="test-report-$(date +%Y%m%d-%H%M%S).txt"
	
	cat > "$report_file" <<EOF
LEDSpicer Package Testing Report
Generated: $(date)

================================
Summary
================================
Total Tests:   $TOTAL_TESTS
Passed:        $PASSED_TESTS
Failed:        $FAILED_TESTS
Skipped:       $SKIPPED_TESTS

================================
Distribution Status
================================
$([ $PASSED_TESTS -gt 0 ] && echo "✓" || echo "✗") Ubuntu PPA    - $([ $PASSED_TESTS -gt 0 ] && echo "Passed" || echo "Failed/Pending")
$([ $PASSED_TESTS -gt 0 ] && echo "✓" || echo "✗") Fedora COPR  - $([ $PASSED_TESTS -gt 0 ] && echo "Passed" || echo "Failed/Pending")
$([ $PASSED_TESTS -gt 0 ] && echo "✓" || echo "✗") Arch AUR     - $([ $PASSED_TESTS -gt 0 ] && echo "Passed" || echo "Failed/Pending")

================================
Next Steps
================================
$(if [ $FAILED_TESTS -gt 0 ]; then
	echo "1. Review failed test output above"
	echo "2. Fix any issues found"
	echo "3. Re-run tests"
	echo "4. Update package repositories"
else
	echo "1. All tests passed!"
	echo "2. Packages are ready for release"
	echo "3. Update documentation"
	echo "4. Announce new release"
fi)

================================
Testing Environment
================================
Date: $(date)
System: $(uname -a)
Docker: $(docker --version 2>/dev/null || echo "Not available")

EOF
	
	print_step "Report saved to: $report_file"
}

# Run main function
main "$@"
