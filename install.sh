#!/bin/bash
SERVICE=/etc/systemd/system/sgcs.service


mkdir /opt/sgcs
cp -r plugins /opt/sgcs
cp lib* /opt/sgcs
cp SGCSApp /opt/sgcs

# service

if test -f "$SERVICE"; then
    echo "$SERVICE exists, create new...";
    rm "$SERVICE"
fi

touch "$SERVICE"
chmod 664 "$SERVICE"
echo "[Unit]" >> "$SERVICE"
echo "Description=SGCS" >> "$SERVICE"
echo "" >> "$SERVICE"
echo "[Service]" >> "$SERVICE"
echo "Type=simple" >> "$SERVICE"
echo "ExecStart=/opt/sgcs/SGCSApp" >> "$SERVICE"
echo "" >> $SERVICE
echo "[Install]" >> "$SERVICE"
echo "WantedBy=multi-user.target" >> "$SERVICE"

systemctl enable sgcs
systemctl start sgcs
