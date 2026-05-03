# Development and Release Workflow

## Repository Structure

```
LEDSpicer/
├── CMakeLists.txt                ← Single source of truth (version, url, maintainer, email)
├── CHANGELOG.md                  ← Changes in Keep a Changelog format
├── debian/
│   ├── changelog                 ← Generated
│   ├── control
│   ├── rules
│   ├── copyright
│   ├── source/format
│   └── *.install
└── packaging/
    ├── README.md                 ← this file
    ├── generate-changelogs.sh
    ├── arch/
    │   ├── PKGBUILD              ← Updated by generate-changelogs.sh
    │   ├── ledspicer.install
    │   └── .SRCINFO              ← Generated (hidden file, do not forget)
    └── rpm/
        └── ledspicer.spec        ← Updated by generate-changelogs.sh
```

---

## Branch Strategy

```
master  ←  development  ←  feature branches
```

- `master` and `development` are protected — no direct pushes.
- All changes go through pull requests on GitHub.
- Any PR targeting `master` triggers the automated unit test suite.

---

## Development Cycle

Repeat this cycle for each task or fix before a release.

1. Pick a task or issue to work on.

2. Create a feature branch off `development`:
   ```bash
   git checkout development
   git checkout -b feature/my-task
   ```

3. Code the changes.

4. Test manually.

5. Run the automated test suite:
   ```bash
   cmake -S . -B build -DENABLE_TESTS=ON -DENABLE_DRY_RUN=ON
   cmake --build build/tests -j$(nproc)
   ctest --test-dir build/tests
   ```

6. Commit and push the branch:
   ```bash
   git add -A
   git commit -m "Short description of change"
   git push origin feature/my-task
   ```

7. Open a pull request on GitHub: `feature/my-task` → `development` and merge it there.

Repeat until the set of changes is ready to ship.

---

## Release

### 1. Prepare

1. Update the version in `CMakeLists.txt`:
   ```
   project(LEDSpicer VERSION x.y.z ...)
   ```

2. Add a new section to `CHANGELOG.md` summarising all changes since the last release.

3. Run the changelog generator:
   ```bash
   ./packaging/generate-changelogs.sh
   ```
   This updates:
   - `debian/changelog`
   - `packaging/arch/PKGBUILD` (version + checksum)
   - `packaging/arch/.SRCINFO`
   - `packaging/rpm/ledspicer.spec` (version + changelog entry)

4. Review the generated files, then commit, tag, and push:
   ```bash
   git add -A
   git commit -m "Release x.y.z"
   git tag x.y.z
   git push
   git push --tags
   ```

5. Open a pull request on GitHub: `development` → `master`. This triggers the automated unit
   test suite — merge only after it passes.

### 2. PPA (Ubuntu / Debian)

**Option A — Launchpad git remote** (triggers build automatically):
```bash
git push launchpad master
```

**Option B — dput**:
```bash
dput ppa:meduzapat/ledspicer ../ledspicer_VERSION_source.changes
```

Monitor at https://launchpad.net/~meduzapat/+archive/ubuntu/ledspicer

### 3. AUR (Arch Linux)

The generator already updated `PKGBUILD` and `.SRCINFO` — both must be pushed.

First-time clone:
```bash
git clone ssh://aur@aur.archlinux.org/ledspicer.git ~/aur/ledspicer
```

Copy and push:
```bash
cd ~/aur/ledspicer
cp /path/to/LEDSpicer/packaging/arch/PKGBUILD .
cp /path/to/LEDSpicer/packaging/arch/.SRCINFO .
git add PKGBUILD .SRCINFO
git commit -m "Update to x.y.z"
git push
```

Verify at https://aur.archlinux.org/packages/ledspicer

### 4. COPR (Fedora / RPM)

The generator already updated `packaging/rpm/ledspicer.spec`. Build the SRPM:

```bash
VERSION=x.y.z
rm ~/rpmbuild/SOURCES/*
cp packaging/rpm/ledspicer.spec ~/rpmbuild/SPECS/
git archive --format=tar.gz --prefix=LEDSpicer-${VERSION}/ HEAD \
    > ~/rpmbuild/SOURCES/LEDSpicer-${VERSION}.tar.gz
rpmbuild -bs ~/rpmbuild/SPECS/ledspicer.spec
```

This produces `~/rpmbuild/SRPMS/ledspicer-VERSION*.src.rpm`. Submit it with either:

**Option A — Web UI**: go to https://copr.fedorainfracloud.org/coprs/meduzapat/ledspicer/ →
New Build → Upload SRPM.

**Option B — copr-cli**:
```bash
copr-cli build meduzapat/ledspicer ~/rpmbuild/SRPMS/ledspicer-VERSION*.src.rpm
```
Requires one-time setup: install `copr-cli` and place your API token in `~/.config/copr`
(available at https://copr.fedorainfracloud.org/api/).
