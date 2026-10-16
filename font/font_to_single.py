#!/usr/bin/env python3
#
# extract all glyphs in a given font to a ASCII representation
#
import monobit
import sys
import os

spacing = 0

if len(sys.argv)<3:
    print("usage: "+sys.argv[0]+" in-file out-dir")
    sys.exit(1)
else:
    font = monobit.load(sys.argv[1])
    outdir = sys.argv[2]

# sanity checks
if not os.path.isdir (outdir):
    print ("output directory does not exist")
    sys.exit (1)

# print information about the font
number_of_glyphs = len (font.get().glyphs)
size_of_glyph = font.get().cell_size
print ("extracting %d glyphs of size %d x %d" % (number_of_glyphs, size_of_glyph[0], size_of_glyph[1]))

# extract individual glyphs
for i in range (0,number_of_glyphs):
    cp = outdir + "/" + "cp_%03d" % (int.from_bytes (font.get().glyphs[i].codepoint.value, byteorder="little")) + ".txt"   

    with open (cp, "wb") as file:
        print ("writing codepoint %d" % (i))
        file.write (font.get().glyphs[i].as_text().encode("ascii"))
