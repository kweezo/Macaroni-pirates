from PIL import Image
import numpy as np
import sys
import os

if len(sys.argv) != 3:
    print("Input 2 args, input and output")
    sys.exit(1)

dat = np.array(Image.open(sys.argv[1]).convert("RGBA"))

out = open(sys.argv[2], "wb")
out.write(bytes(f"""
uint8_t texWidth = {dat.shape[1]};
uint8_t texHeight = {dat.shape[0]};
uint8_t texDat[] = {'{'}
""", encoding="ascii"))

for row in dat:
    for pixel in row:
        for channel in range(2, -1, -1):
            out.write(bytes(str(pixel[channel]) + ", ", encoding="ascii"))
        out.write(bytes(str(pixel[3]) + ", ", encoding="ascii"))

out.seek(-2, os.SEEK_CUR)
out.write(bytes("};", encoding="ascii"))
out.close()
