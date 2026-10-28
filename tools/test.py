"""Native host tests of the actual selector, HUD, records and room-loop controller."""
from pathlib import Path
import json
import os
import re
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'build/tests'
OUT.mkdir(parents=True, exist_ok=True)
compiler = os.environ.get('CXX') or shutil.which('c++') or shutil.which('clang++') or shutil.which('g++')
if not compiler: raise SystemExit('A native C++ compiler is required for host tests.')
for p in (ROOT/'tests/fakes').iterdir(): shutil.copyfile(p, OUT/p.name)
for name in ['echo_state.h','echo_records.h','echo_records.cpp','practice.h','practice_loop.cpp','font.inc']:
    shutil.copyfile(ROOT/'src'/name, OUT/name)
for name in ['atlas.c','atlas.h']: shutil.copyfile(ROOT/'src/gfx'/name, OUT/name)
source = (ROOT/'src/emu.cpp').read_text().split('const uint8_t tilemap[',1)[1]
tiles = [int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)',source)]
assert len(tiles) == 8192
(OUT/'room_data.h').write_text('const uint8_t tilemap[8192] = {'+','.join(map(str,tiles))+'};\n')
results = []
for name in ['echo_state_test','practice_integration_test']:
    shutil.copyfile(ROOT/'tests'/f'{name}.cpp', OUT/f'{name}.cpp')
    executable = OUT/(name+('.exe' if os.name == 'nt' else ''))
    flags = ['-std=c++14','-O2','-Wall','-Wextra','-Werror']
    subprocess.run([compiler,*flags,'-I'+str(OUT),str(OUT/f'{name}.cpp'),'-o',str(executable)], check=True)
    result = subprocess.run([str(executable)],cwd=ROOT,check=True,text=True,capture_output=True)
    print(result.stdout,end='')
    results.append({'test':name,'passed':True,'output':result.stdout})
(OUT/'results.json').write_text(json.dumps(results,indent=2))
