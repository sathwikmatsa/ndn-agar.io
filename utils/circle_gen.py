#!/usr/bin/python3

from PIL import Image, ImageDraw

image = Image.new('RGBA', (200, 200))
draw = ImageDraw.Draw(image)
draw.ellipse((20, 20, 180, 180), fill = 'green', outline ='green')
image.save('circle.png')
