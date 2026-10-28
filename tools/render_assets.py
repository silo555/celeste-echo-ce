"""Render the repo-native pixel-vector icon (no generated or edited photo assets)."""
from pathlib import Path
import re
import xml.etree.ElementTree as ET
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
image = Image.new('RGB', (16, 16))
draw = ImageDraw.Draw(image)
for node in ET.parse(ROOT/'assets/icon.svg').getroot():
    commands = re.findall(r'[MmhvHzVvLl]|-?\d+', node.attrib['d'])
    x = y = 0
    points = []
    cursor = 0
    while cursor < len(commands):
        command = commands[cursor]; cursor += 1
        if command in 'MmLl':
            a,b = map(int,commands[cursor:cursor+2]); cursor += 2
            if command.islower(): x += a; y += b
            else: x,y = a,b
        elif command in 'hHvV':
            value = int(commands[cursor]); cursor += 1
            if command == 'h': x += value
            elif command == 'H': x = value
            elif command == 'v': y += value
            else: y = value
        elif command == 'z':
            continue
        points.append((x,y))
    draw.polygon(points, fill=node.attrib['fill'])
image.save(ROOT/'icon.png')
image.resize((128,128), Image.Resampling.NEAREST).save(ROOT/'docs/icon-preview.png')
print('Rendered 16x16 Echo mountain/loop icon from assets/icon.svg')
