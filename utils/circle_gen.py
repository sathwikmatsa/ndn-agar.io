#!/usr/bin/python3

from PIL import Image, ImageDraw

image = Image.new('RGBA', (200, 200))
draw = ImageDraw.Draw(image)
draw.rectangle((0, 0, 200, 200), fill = 'black', outline = 'black')
draw.ellipse((0, 0, 200, 200), fill = 'white', outline ='white')
image.save('circle.png')
