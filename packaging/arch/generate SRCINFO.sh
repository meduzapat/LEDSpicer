#!/usr/bin/env bash
set -e

docker run --rm \
	-v "$PWD:/pkg" \
	-w /pkg \
	archlinux:latest \
	bash -c '
		set -e

		pacman -Sy --noconfirm archlinux-keyring
		pacman -S --noconfirm --needed base-devel git sudo

		useradd -m builder
		echo "builder ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
		chown -R builder:builder /pkg

		su - builder -c "
			cd /pkg

			echo \"==> CHECKSUMS (print-only)\"
			makepkg -g --print

			echo
			echo \"==> .SRCINFO (print-only)\"
			makepkg --printsrcinfo
		"
	'
