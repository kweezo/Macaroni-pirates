from PIL import Image
import numpy as np
import sys
import os

if len(sys.argv) != 3:
    print("Input 2 args, input and output")
    sys.exit(1)

dat = np.array(Image.open(sys.argv[1]))

out = open(sys.argv[2], "wb")
out.write(bytes(f"""
uint8_t texWidth = {dat.shape[1]};
uint8_t texHeight = {dat.shape[0]};
uint8_t texDat[] = {'{'}
""", encoding="ascii"))

for row in dat:
    for pixel in row:
        for channel in range(min(len(pixel), 4)):
            out.write(bytes(str(pixel[channel]) + ", ", encoding="ascii"))

        for i in range(4 - len(pixel)):
            out.write(bytes(str(255) + ", ", encoding="ascii"))
out.seek(-2, os.SEEK_CUR)
out.write(bytes("};", encoding="ascii"))
out.close()
