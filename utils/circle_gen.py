#!/usr/bin/python3

from PIL import Image, ImageDraw

image = Image.new('RGBA', (720, 720))
draw = ImageDraw.Draw(image)
draw.rectangle((0, 0, 720, 720), fill = 'black', outline = 'black')
draw.ellipse((0, 0, 720, 720), fill = 'white', outline ='white')
image.save('circle.png')
