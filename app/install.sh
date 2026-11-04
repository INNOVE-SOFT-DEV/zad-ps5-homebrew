#!/usr/bin/env bash
# Depose ZAD sur la PS5 par FTP puis lance l'installeur. Usage : PS5_HOST=192.168.x.x ./install.sh
# (serveur FTP de la console, port 2121 par defaut ; chargeur d'ELF sur 9021). Base sur l'exemple du SDK.
set -euo pipefail
cd "$(dirname "$0")"
: "${PS5_HOST:?definir PS5_HOST}"; FTP_PORT=${FTP_PORT:-2121}; PS5_PORT=${PS5_PORT:-9021}
T=ZADA00001; FTP="ftp://$PS5_HOST:$FTP_PORT"
make >/dev/null
curl -Q "MTRW" "$FTP" >/dev/null
for d in /system_ex/app/$T /system_ex/app/$T/sce_sys /user/app/$T /user/app/$T/sce_sys /data/homebrew /data/homebrew/zad; do
  curl -Q "MKD $d" "$FTP" >/dev/null || true
done
curl -T $T/eboot.bin "$FTP/system_ex/app/$T/"
curl -T $T/sce_sys/param.json "$FTP/user/app/$T/sce_sys/"
curl -T $T/sce_sys/icon0.png "$FTP/user/app/$T/sce_sys/"
curl -T ../build/zad.elf "$FTP/data/homebrew/zad/"
"$PS5_PAYLOAD_SDK/bin/prospero-deploy" -h "$PS5_HOST" -p "$PS5_PORT" installer.elf
curl -T $T/sce_sys/param.json.system "$FTP/system_ex/app/$T/sce_sys/param.json"
echo "Installe : cherchez ZAD sur l'ecran d'accueil. Desinstaller : prospero-deploy uninstaller.elf"
