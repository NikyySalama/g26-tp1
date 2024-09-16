#!/bin/bash

mkdir -p md5Files

for i in $(seq 1 100); do
    count=$(( (RANDOM % (500 - 10 + 1)) + 10 ))
    
    dd if=/dev/urandom of=md5Files/file_$i.bin bs=1M count=$count status=none
done

echo "Se han generado 100 archivos con tamaños aleatorios entre 10 MB y 500 MB en la carpeta md5Files."
