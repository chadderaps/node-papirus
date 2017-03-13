import sys
from PIL import Image
from PIL import ImageOps
from PIL import ImageFont
from PIL import ImageDraw
from timeit import Timer

WHITE = 1
BLACK = 0

def convert(image):
    if image.mode != "1":
        image = ImageOps.grayscale(image).convert("1", dither=Image.FLOYDSTEINBERG)
    return image

def dump(image, char, fontSize, width, height):
    img_bytes = image.tobytes().encode('hex')
    idx = 0
    row = 0
    byte = 0
    count = 0
    sep = ''
    row_sep = '},\n    {'

    bytewidth = width / 8
    if width % 8:
        bytewidth = bytewidth + 1

    sys.stdout.write('unsigned char char_%s_%u_data[%u] = {' % (char, fontSize, len(img_bytes) / 2))

    sep = ''
    while idx < len(img_bytes):
        nib1 = img_bytes[idx]
        nib2 = img_bytes[idx+1]

        idx = idx + 2
        sys.stdout.write("%s 0x%s%s" % (sep, nib1, nib2))
        sep = ','
    sys.stdout.write(' };\n\n')
    sys.stdout.write('char_font char_%s_%u = { %u, %u, %u, char_%s_%u_data };\n\n' % (char, fontSize, width, height, fontSize, char, fontSize))
    return

def generate(char, size):

    font = ImageFont.truetype('Menlo.ttc', size)
    (w, h) = font.getsize(char)

    image = Image.new('1', (w,h), WHITE)
    draw = ImageDraw.Draw(image)
    draw.text((0,0), char, font=font, fill=BLACK)
    dump(image, char, size, w, h)

def createTable(name, chars, sizes):

    sys.stdout.write('const char_font * %s[%u][%u] = {\n' % (name, len(chars), len(sizes)))

    char_sep = ''
    size_sep = ''

    for char in chars:
        sys.stdout.write('%s    {\n' % (char_sep))
        char_sep = ',\n'
        size_sep = ''
        for size in sizes:
            sys.stdout.write('%s        &char_%s_%u' % (size_sep, char, size))
            size_sep = ',\n'
        sys.stdout.write('\n    }')
    sys.stdout.write('\n};\n')

sys.stdout.write('#include "bitimage_fonts.h"\n\n')

numbers = '0123456789'
lower_letters = 'abcdefghijklmnopqrstuvwxyz'
upper_letters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'

sizes = [8, 12, 16]

for num in numbers:
    for size in sizes:
        generate(num, size)

for lower in lower_letters:
    for size in sizes:
        generate(lower, size)

for upper in upper_letters:
    for size in sizes:
        generate(upper, size)

createTable('NUMBER_CHARS', numbers, sizes)
createTable('LOWER_CHARS', lower_letters, sizes)
createTable('UPPER_CHARS', upper_letters, sizes)
