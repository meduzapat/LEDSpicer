# Development and Release Workflow

A personal maintainer runbook for developing, releasing, and packaging LEDSpicer.
The publishing steps (PPA, AUR, COPR) rely on `meduzapat`'s own signing keys and
accounts, so in practice only the maintainer can run them — this is a memo to
self, not a contributor guide.

- The **core guide** (sections 1–4) is the linear path: code → release → publish.
- One-time machine/account setup lives in the
  [Appendix: one-time setup](#appendix-one-time-setup). Core steps link to it where
  needed, so you set it up once and skip it afterwards.
- [Optional: test the packages](#optional-test-the-packages) is at the very end.

`CMakeLists.txt` is the single source of truth — version, URL, maintainer, and
email are read from it by `generate-changelogs.sh`. Never hardcode them elsewhere.

**Conventions used below:** run commands from the project root (your local clone
sitting on `development`). `BUILD_DIR` is wherever you build out-of-tree — set it
once to suit your box, e.g. `BUILD_DIR=~/build/ledspicer`.

> This document is the **manual fallback**. The goal is for tagging a release to
> publish to AUR, Fedora (COPR) and Ubuntu (PPA) automatically; until that is in
> place, or when something breaks, follow the steps here by hand.

## 1. Repository structure

```
LEDSpicer/
├── CMakeLists.txt                ← single source of truth (version, url, maintainer, email)
├── CHANGELOG.md                  ← changes in Keep a Changelog format
├── debian/
│   ├── changelog                 ← generated
│   ├── control
│   ├── rules
│   ├── copyright
│   ├── source/format
│   └── *.install
└── packaging/
    ├── README.md                 ← this file
    ├── generate-changelogs.sh    ← updates all distro files from CMakeLists.txt + CHANGELOG.md
    ├── arch/
    │   ├── PKGBUILD              ← updated by generate-changelogs.sh (checksum stays SKIP, see 4.5)
    │   ├── generate SRCINFO.sh   ← optional Docker helper to validate checksums/.SRCINFO
    │   ├── ledspicer.install
    │   └── .SRCINFO             ← generated (hidden file, do not forget to push to AUR)
    └── rpm/
        └── ledspicer.spec        ← updated by generate-changelogs.sh
```

Keeping the packaging metadata inside the code repo is intentional and required:
Launchpad reads `debian/`, COPR reads `packaging/rpm/ledspicer.spec`, and the AUR
`PKGBUILD` is sourced from `packaging/arch/`.

## 2. Branch strategy

```
master  ←  development  ←  feature branches
```

- `master` and `development` are protected — no direct pushes.
- All changes go through pull requests on GitHub.
- A PR targeting `master` triggers the build and unit-test workflows.
- `master` is effectively the release branch; a dedicated release branch is overkill.

## 3. Development cycle

Repeat for each task or fix before a release.

1. Create a feature branch off `development`:
   ```bash
   git checkout development
   git checkout -b feature/my-task
   ```
2. Code the changes and test manually.
3. Run the unit tests (no hardware needed, they use their own mocks). Tests are
   gated by the `ENABLE_TESTS` option (default `OFF`); turning it on makes CMake
   require GoogleTest and pull in `tests/CMakeLists.txt`, which builds everything
   under `${BUILD_DIR}/tests`. `ENABLE_DRY_RUN` is unrelated (mock hardware for
   the main binary) and not needed here.
   ```bash
   cmake -S . -B "${BUILD_DIR}" -DENABLE_TESTS=ON
   cd "${BUILD_DIR}/tests" && make && ctest
   ```
   Your IDE can do the same once configured with `-DENABLE_TESTS=ON`.
4. Commit and push:
   ```bash
   git add -A
   git commit -m "Short description of change"
   git push origin feature/my-task
   ```
5. Open a PR `feature/my-task` → `development` and merge once green.

## 4. Release

Ground rules that explain the ordering:

- The release **tag is created by the GitHub Release**, after `development` is
  merged into `master`. So everything that must live inside the tag — the Fedora
  `.spec` and the Debian `changelog` — has to be committed on `development`
  *before* the PR (step 4.1), so it travels into `master` and into the tag.
- After the tag exists, **nothing goes back into the code repo.** The only
  post-tag value is the Arch checksum, which lives solely in the separate AUR
  repo (see 4.5).

> **If you spot something off mid-release** (missing code, wrong changelog, a
> packaging fix) after the Release/tag is cut: delete the GitHub Release **and**
> delete the tag, go back to `development`, fix it, and start over from 4.1. Do
> not patch on top of a published tag — re-cut it. (`git push --delete origin
> <tag>` removes the tag once the Release is deleted in the UI.)

### 4.0 Set release variables

Sit in the project root on `development`. `VERSION` is read from `CMakeLists.txt`,
so nothing is hardcoded:

```bash
AUR_DIR=~/aur/ledspicer        # local clone of the AUR repo (see appendix)
RPMBUILD=~/rpmbuild            # only needed for the manual COPR fallback
```

### 4.1 Prepare on `development`

1. Bump the version in `CMakeLists.txt` (and `DATA_VERSION` if the data format
   changed):
   ```
   project(LEDSpicer VERSION x.y.z ...)
   ```
2. Add a new section to `CHANGELOG.md` (Keep a Changelog format). To draft it,
   feed this prompt to an AI with access to the repo history (Copilot, Claude, …):

   > Prepare the CHANGELOG.md entry for LEDSpicer vX.Y.Z. Review everything merged
   > into `development` since tag `<previous>`. Output only a "Keep a Changelog"
   > section starting with `## [X.Y.Z] - YYYY-MM-DD`, using only the subsections
   > that have content (Added, Changed, Fixed, Removed, Breaking Changes). One
   > concise line per bullet, user-facing impact, cite issue/PR numbers in
   > parentheses when known. No marketing, no duplicates, no fluff.

3. Re-read the version now that you changed it:
   ```bash
   VERSION=$(sed -n 's/.*project(LEDSpicer VERSION \([0-9.]*\).*/\1/p' CMakeLists.txt)
   echo "Releasing $VERSION"
   ```
4. Run the generator. The tag does not exist yet, so the Arch checksum falls back
   to `SKIP` — that is expected and stays that way in the repo (fixed at AUR
   publish time in 4.5):
   ```bash
   ./packaging/generate-changelogs.sh
   ```
   It updates `debian/changelog`, `packaging/rpm/ledspicer.spec`,
   `packaging/arch/PKGBUILD` (version), and `packaging/arch/.SRCINFO`.
5. Review the generated files, then commit and push:
   ```bash
   git add -A
   git commit -m "Release ${VERSION}"
   git push origin development
   ```

### 4.2 PR to `master`

Open a PR `development` → `master`. This runs the build and unit-test workflows.
Merge only after they pass; a short "release candidate" merge message is fine.
The spec and debian changelog are now in `master`.

### 4.3 Cut the GitHub Release

On GitHub, switch to `master` and create the Release. This is what **creates the
`${VERSION}` tag** and publishes the source tarball.

For a polished release description, feed this prompt to an AI with the changelog
section:

> Write a concise GitHub Release description for LEDSpicer vX.Y.Z based strictly
> on this changelog section: `<paste>`. Open with one sentence on the release's
> theme, then short grouped highlights (Highlights / Fixes / Breaking) only where
> there is content. Under ~150 words. No install instructions, no hyperbole. End
> with a link to the full changelog.

### 4.4 Publish — PPA (Ubuntu / Debian)

First time? See [PPA / Launchpad setup](#ppa--launchpad-setup).

```bash
git push launchpad master
```

The Launchpad recipe does **not** build on push automatically. Either enable
"Build daily" on the recipe, or open the recipe page and click **Request
build(s)** to build now. Monitor at
https://launchpad.net/~meduzapat/+archive/ubuntu/ledspicer

### 4.5 Publish — AUR (Arch Linux)

First time? See [AUR setup](#aur-setup).

Now that the tag tarball exists, fill in the real Arch checksum, push it to the
**AUR repo only**, then discard the local change so the code repo keeps `SKIP`:

```bash
./packaging/generate-changelogs.sh                 # now fetches the real sha256
cp packaging/arch/PKGBUILD  "${AUR_DIR}/"
cp packaging/arch/.SRCINFO "${AUR_DIR}/"
git -C "${AUR_DIR}" add PKGBUILD .SRCINFO
git -C "${AUR_DIR}" commit -m "Update to ${VERSION}"
git -C "${AUR_DIR}" push
git restore packaging/arch/PKGBUILD packaging/arch/.SRCINFO   # keep the code repo at SKIP
```

> Why the checksum never lives in the code repo: the tarball it hashes *contains*
> `PKGBUILD`, so writing the hash into `PKGBUILD` would change the tarball and
> invalidate the hash. It is only meaningful in the AUR repo. If it still prints
> `SKIP`, GitHub has not finished publishing the tag tarball — wait a minute and
> re-run. Verify at https://aur.archlinux.org/packages/ledspicer

### 4.6 Publish — COPR (Fedora / RPM)

**Primary — build from the repo automatically.** Once COPR is configured to build
from the Git source (one-time, see [COPR setup](#copr-setup)), cutting the Release
in 4.3 is all that is needed: the webhook makes COPR clone the tag, find
`packaging/rpm/ledspicer.spec`, and build. Nothing to do here. Monitor at
https://copr.fedorainfracloud.org/coprs/meduzapat/ledspicer/

<details>
<summary>Fallback — build and upload an SRPM by hand</summary>

Use this only if COPR auto-build is not set up, or it failed. Needs a Fedora
machine (or container). First time? See [Fedora build tree](#fedora-build-tree)
and [COPR setup](#copr-setup).

```bash
rm -f "${RPMBUILD}/SOURCES/"*
cp packaging/rpm/ledspicer.spec "${RPMBUILD}/SPECS/"
git archive --format=tar.gz --prefix="LEDSpicer-${VERSION}/" "${VERSION}" \
    > "${RPMBUILD}/SOURCES/LEDSpicer-${VERSION}.tar.gz"
rpmbuild -bs "${RPMBUILD}/SPECS/ledspicer.spec"
copr-cli build meduzapat/ledspicer "${RPMBUILD}/SRPMS/ledspicer-${VERSION}"*.src.rpm
```

Archiving from the `${VERSION}` tag (not `HEAD`) guarantees the SRPM matches the
released tarball. You can also upload the `.src.rpm` via the COPR web UI.
</details>

---

## 5. Automation (the happy path)

Once the setup below is in place, **cutting the GitHub Release (4.3) is the only
manual trigger** — `.github/workflows/release.yml` and Launchpad's daily build do
the rest. The step-by-step publish actions in [section 4](#4-release) become the
**fallback** for when automation is off or a build breaks.

What happens when you publish a Release:

| Target | Trigger | Mechanism |
|---|---|---|
| **AUR** | `release: published` | `release.yml` regenerates `PKGBUILD`/`.SRCINFO` with the real checksum and pushes to the AUR repo over SSH |
| **Fedora / COPR** | `release: published` | `release.yml` POSTs to COPR's custom webhook; COPR rebuilds from the tag |
| **Ubuntu / PPA** | within ~24h | Launchpad recipe "Build daily" — **after** you `git push launchpad master` (still manual until an optional GitHub→Launchpad code import is set up) |

The workflow fires on `release: published` on purpose: only then do the tag and
its source tarball exist, which both the AUR checksum and the Fedora `Source0`
depend on. AUR and COPR run as independent jobs, so one distro failing never
blocks the other.

**Required GitHub Actions secrets** (repo → Settings → Secrets → Actions):

| Secret | What it is | Blast radius |
|---|---|---|
| `AUR_SSH_PRIVATE_KEY` | private half of a **dedicated CI** SSH key registered on the AUR account | revoke just that key if leaked; main AUR key untouched |
| `COPR_WEBHOOK_URL` | the full COPR **custom** webhook URL for the `ledspicer` package | package-scoped — can only trigger a build of this one package |

PPA still needs no GitHub secret: Launchpad builds and signs with the GPG key held
on its own side.

---


Per-machine / per-account setup. Do it once, then ignore it on later releases.
Accounts stay under the personal `meduzapat` namespace across all platforms.

### Build dependencies

Needed to compile and run the unit tests locally.

```bash
# Debian / Ubuntu
sudo apt-get install build-essential cmake pkg-config \
    libtinyxml2-dev libpulse-dev libasound2-dev libusb-1.0-0-dev libgtest-dev

# Fedora
sudo dnf install gcc-c++ cmake pkgconf tinyxml2-devel \
    pulseaudio-libs-devel alsa-lib-devel libusb1-devel gtest-devel

# Arch
sudo pacman -S --needed base-devel cmake pkgconf tinyxml2 libpulse alsa-lib libusb gtest
```

### COPR setup

**SCM auto-build (recommended).** In the COPR web UI, open project
`meduzapat/ledspicer` → Packages → New package → SCM:

- Clone URL: `https://github.com/meduzapat/LEDSpicer.git`
- Committish: `master`
- Spec file: `packaging/rpm/ledspicer.spec`
- SRPM build method: **`rpkg`**
- Enable "auto-rebuild from webhook"

Use `rpkg` (not `make_srpm`) because the spec is a plain spec whose `Source0`
already points at the released tag tarball — `rpkg` just downloads that exact
tarball and builds, with no `.copr/Makefile` to maintain and the same tarball AUR
and the manual flow use. `make_srpm` would only be needed to build untagged
snapshots from the git tree.

**Triggering.** Do **not** register COPR's URL as a GitHub *push* webhook — that
fires on the `development → master` merge, before the tag tarball exists, and
404s. Instead, grab the **custom** webhook URL (Settings → Integrations → the
`/webhooks/custom/...` one, *not* `/webhooks/github/...` or `/webhooks/custom-dir/...`),
store it in the `COPR_WEBHOOK_URL` secret, and let
[`release.yml`](#5-automation-the-happy-path) POST to it after the Release is cut.

**copr-cli (only for the manual fallback).**

```bash
sudo dnf install copr-cli
```

Place your API token in `~/.config/copr` — generate it at
https://copr.fedorainfracloud.org/api/

### Fedora build tree

Only needed for the manual COPR fallback, on a Fedora machine. `rpmbuild` expects
`~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}` to exist; it is not created
automatically:

```bash
sudo dnf install rpmdevtools
rpmdev-setuptree
```

### PPA / Launchpad setup

Add the Launchpad git remote to your local clone (same working dir you release
from):

```bash
git remote add launchpad git+ssh://<your-lp-user>@git.launchpad.net/ledspicer
```

Create a source-package **recipe** on Launchpad pointing at `master`. Note that
recipes do not build on push — enable "Build daily", or click "Request build(s)"
after pushing. Building requires your GPG signing key registered with Launchpad.

### AUR setup

For the **manual** flow, upload your personal SSH public key to your AUR account
and clone the package repo once:

```bash
git clone ssh://aur@aur.archlinux.org/ledspicer.git ~/aur/ledspicer
```

This is the `AUR_DIR` used in [4.5](#45-publish--aur-arch-linux).

For the **automated** flow, generate a **dedicated CI keypair** (don't reuse your
personal key), register its public half on the AUR account, and put the private
half in the `AUR_SSH_PRIVATE_KEY` secret. A separate key means a leak is contained
to CI — revoke it on AUR without touching your own key:

```bash
ssh-keygen -t ed25519 -f aur-ci -C "ledspicer-ci" -N ""
# add aur-ci.pub to https://aur.archlinux.org/account → SSH Public Key (one per line)
# paste the contents of aur-ci (private) into the GitHub secret AUR_SSH_PRIVATE_KEY
```

---

## Optional: test the packages

Before releasing, you can confirm each package builds cleanly. None of this is
required, but it catches packaging mistakes early.

**Unit tests (all platforms).** Same as the development cycle:

```bash
cmake -S . -B "${BUILD_DIR}" -DENABLE_TESTS=ON
cd "${BUILD_DIR}/tests" && make && ctest
```

**Arch — validate checksums and `.SRCINFO`.** The Docker helper builds in a clean
Arch container and prints the generated checksums and `.SRCINFO` without touching
your system:

```bash
cd packaging/arch
./"generate SRCINFO.sh"
```

For a full build, run `makepkg -si` from `packaging/arch` (ideally in a clean
chroot via `extra-x86_64-build`).

**Fedora — local build and lint** (on a Fedora machine):

```bash
rpmbuild -bb "${RPMBUILD}/SPECS/ledspicer.spec"   # full build
rpmlint  "${RPMBUILD}/SPECS/ledspicer.spec"       # spec sanity check
mock -r fedora-rawhide-x86_64 "${RPMBUILD}/SRPMS/ledspicer-${VERSION}"*.src.rpm
```

**Debian — local build:**

```bash
debuild -us -uc          # from the repo root, unsigned build
```
