#!/usr/bin/python3

from PIL import Image
from PIL import ImageDraw
import math

def makeRectangle(l, w, theta, offset=(0,0)):
    c, s = math.cos(theta), math.sin(theta)
    rectCoords = [(l/2.0, w/2.0), (l/2.0, -w/2.0), (-l/2.0, -w/2.0), (-l/2.0, w/2.0)]
    return [(c*x-s*y+offset[0], s*x+c*y+offset[1]) for (x,y) in rectCoords]

L=300; W=300
image = Image.new('RGBA', (L, W))
draw = ImageDraw.Draw(image)

for i in range(0, 361, 10):
    vertices = makeRectangle(200, 200, i*math.pi/180, offset=(L/2, W/2))
    draw.polygon(vertices, fill='green')

draw.ellipse((22, 22, 278, 278), fill = '#33ff33', outline ='#33ff33')
image.save("virus.png")
