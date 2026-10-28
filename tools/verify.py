from pathlib import Path
import hashlib
import json
import re
import struct
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
baseline = json.loads((ROOT/'docs/upstream-hashes.json').read_text())
source = (ROOT/'src/classic.cpp').read_text()
for span in baseline['sections']:
    text = source[source.index(span['start']):source.index(span['end'])]
    assert hashlib.sha256(text.encode()).hexdigest() == span['sha256'], span['name']
print('PASS unchanged upstream player, entity, movement, collision and spike code')
map_source = (ROOT/'src/emu.cpp').read_text().split('const uint8_t tilemap[',1)[1]
tilemap = bytes(int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]+)',map_source))
assert len(tilemap) == 8192 and hashlib.sha256(tilemap).hexdigest() == baseline['map_sha256']
binary = (ROOT/'bin/CECHO.bin').read_bytes()
font = bytes(int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]+)',(ROOT/'src/font.inc').read_text()))
assert tilemap in binary and font in binary
palette = [(0,0,0),(29,43,83),(126,37,83),(0,135,81),(171,82,54),(95,87,79),(194,195,199),(255,241,232),
           (255,0,77),(255,163,0),(255,236,39),(0,228,54),(41,173,255),(131,118,156),(255,119,168),(255,204,170)]
lookup = {rgb:index*17 for index,rgb in enumerate(palette)}
atlas = Image.open(ROOT/'src/gfx/atlas.png').convert('RGB')
data = b''.join(bytes([8,8]+[lookup[atlas.getpixel((tx*8+x,ty*8+y))] for y in range(8) for x in range(8)])
                for ty in range(8) for tx in range(16))
assert data in binary, 'Linked atlas differs from the source pixels'
program = (ROOT/'bin/CECHO.8xp').read_bytes()
assert program[:8] == b'**TI83F*'
assert len(program) == 57 + struct.unpack_from('<H',program,53)[0]
assert sum(program[55:-2])%65536 == struct.unpack_from('<H',program,len(program)-2)[0]
assert program[60:68].rstrip(b'\0') == b'CECHO'
records = (ROOT/'src/echo_records.cpp').read_text()
assert '"CEchoA"' in records and '"CEchoB"' in records
assert all(x not in records for x in ['CelesteS','CelPbA','CelPbB'])
print('PASS room map, font, all sprite bytes, TI file structure/name/checksum, and separate records')
(ROOT/'build/verification.json').write_text(json.dumps({'passed':True,'program_bytes':len(program),
    'sha256':hashlib.sha256(program).hexdigest(),'base_commit':baseline['upstream_commit']},indent=2))
