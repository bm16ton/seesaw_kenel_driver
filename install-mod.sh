#!/bin/bash
sudo cp seesaw.ko /lib/modules/$(uname -r)/updates/dkms
sudo depmod

