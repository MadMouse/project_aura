# Releasing Firmware (GitHub Release Assets)

## 1) Bump firmware version
- Edit `platformio.ini`:
  - `-DAPP_VERSION=\"X.Y.Z\"`

## 2) Build and prepare release assets
Run:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\prepare_release_assets.ps1 -Version X.Y.Z -Tag vX.Y.Z -Repo 21cncstudio/project_aura
```

Output folder:
- `release-assets/vX.Y.Z`

## 3) Create GitHub Release
- GitHub -> `project_aura` -> Releases -> Draft new release
- Tag: `vX.Y.Z`
- Title: `Project Aura vX.Y.Z`
- Body: use notes from `docs/releases/vX.Y.Z.md`

Upload all files from `release-assets/vX.Y.Z`:
- `bootloader.bin`
- `partitions.bin`
- `boot_app0.bin`
- `firmware.bin`
- `littlefs.bin`
- `manifest.json`
- `manifest-update.json`
- `project_aura_X.Y.Z_ota_firmware.bin`
- `project_aura_X.Y.Z_web_installer_full.zip`
- `sha256sums.txt`

## 4) URLs to use on website
- Full install manifest:
  - `https://github.com/21cncstudio/project_aura/releases/download/vX.Y.Z/manifest.json`
- Update-only manifest:
  - `https://github.com/21cncstudio/project_aura/releases/download/vX.Y.Z/manifest-update.json`

## 5) OTA file for dashboard upload
- Use `project_aura_X.Y.Z_ota_firmware.bin` (or `firmware.bin`, same payload).
