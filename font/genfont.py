#!/usr/bin/env python3
#
# convert a given 8 x 16 font to a ASCII based atlas. This ASCII atlas is further used by the
# application to generate a graphical texture for displaying
#
import monobit
import sys

spacing = 0

if len(sys.argv)<3:
    print("usage: "+sys.argv[0]+" in-file out-file")
    sys.exit(1)
else:
    font = monobit.load(sys.argv[1])
    outfile = open(sys.argv[2],"wb")

# sanity checks
if len(font.get().glyphs) != 256:
    print("font need to have 256 defined glyphs")
    sys.exit(1)

if font.get().cell_size != (8,16):
    print("font glyph must have a size of 8x16 pixels");
    sys.exit(1)

# read all the glyphs as their text representation
glyphs = []
for i in range(0,256):
    glyphs.append(font.get().get_glyph(codepoint=i).as_text())

# we want to have 16x16 glyphs in our atlas. Each glyph also contains 16 lines of ASCII data
num_lines = 16
num_columns = 16
for i in range(0,num_lines):
    cp_from = i*num_columns 
    cp_to = cp_from + num_columns
    print("(%d-%d) " % (cp_from, cp_to-1))
    glyph_range = glyphs[cp_from:cp_to]
    for j in range(0,font.get().cell_size.y):                       # number of lines per glyph
        for k in range(0,num_columns):                              # number of glyphs per row in the atlas
            glyph_line = glyph_range[k].splitlines()[j]
            outfile.write((glyph_line + ("%s" % (" " if spacing else ""))).encode("ascii"))
        outfile.write('\n'.encode("ascii"))
    outfile.write(("%s" % ("\n" if spacing else "")).encode("ascii"))

outfile.close()
print("done")

