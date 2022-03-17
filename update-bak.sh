#!/bin/bash
current_time=$(date +"%Y-%m-%d_%H-%M-%S")
mkdir -p /mnt/c/Users/14244/Desktop/Document
file_name="/mnt/c/Users/14244/Desktop/Document/$current_time.zip"
cd ..
zip -r $file_name shared
