#!/usr/bin/env bash
set -euo pipefail

# Resolve this script's directory (works even if called via symlink)
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

# Sources are one folder up: ../WestEngine/Core/...
SRC_ROOT="${SCRIPT_DIR}/../WestEngine/Core"
SOURCE_SCRIPTS="${SRC_ROOT}/Game/Scripts"
SOURCE_ASSETS="${SRC_ROOT}/Game/Assets"
SOURCE_SHADERS="${SRC_ROOT}/Shader"

# Targets under current folder
TARGET_SCRIPTS_DEBUG="${SCRIPT_DIR}/Debug/lua"
TARGET_SCRIPTS_RELEASE="${SCRIPT_DIR}/Release/lua"

TARGET_ASSETS_DEBUG="${SCRIPT_DIR}/Debug/assets"
TARGET_ASSETS_RELEASE="${SCRIPT_DIR}/Release/assets"

TARGET_SHADERS_DEBUG="${SCRIPT_DIR}/Debug/shader"
TARGET_SHADERS_RELEASE="${SCRIPT_DIR}/Release/shader"

copy_dir() {
  local src="$1"
  local dst="$2"

  if [[ ! -d "$src" ]]; then
    echo "WARNING: Source directory not found: $src" >&2
    return 0
  fi

  mkdir -p "$dst"
  echo "Copying: $src  -->  $dst"

  if command -v rsync >/dev/null 2>&1; then
    # -a: archive (preserve attrs), trailing slashes copy contents
    rsync -a "$src"/ "$dst"/
  else
    # Fallback: cp -a (use /. to copy contents not the dir itself)
    cp -a "$src"/. "$dst"/
  fi
}

echo "== Copy Scripts =="
copy_dir "$SOURCE_SCRIPTS" "$TARGET_SCRIPTS_DEBUG"
copy_dir "$SOURCE_SCRIPTS" "$TARGET_SCRIPTS_RELEASE"

# Flip _DEBUG for the Release Lua build
LUA_FILE="${TARGET_SCRIPTS_RELEASE}/Utils.lua"
if [[ -f "$LUA_FILE" ]]; then
  echo "Editing Lua file: $LUA_FILE"
  # Replace any occurrences of `_DEBUG = true` with `_DEBUG = false`
  sed -i 's/_DEBUG[[:space:]]*=[[:space:]]*true/_DEBUG = false/g' "$LUA_FILE"
else
  echo "NOTE: Lua file not found (skipping edit): $LUA_FILE"
fi

echo "== Copy Assets =="
copy_dir "$SOURCE_ASSETS" "$TARGET_ASSETS_DEBUG"
copy_dir "$SOURCE_ASSETS" "$TARGET_ASSETS_RELEASE"

echo "== Copy Shaders =="
copy_dir "$SOURCE_SHADERS" "$TARGET_SHADERS_DEBUG"
copy_dir "$SOURCE_SHADERS" "$TARGET_SHADERS_RELEASE"

echo "All files have been copied successfully."

