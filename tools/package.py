from pathlib import Path
import hashlib
import json
import shutil
import zipfile

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'dist'
OUT.mkdir(exist_ok=True)
tests = json.loads((ROOT/'build/tests/results.json').read_text())
verify = json.loads((ROOT/'build/verification.json').read_text())
assert verify['passed'] and len(tests) == 2 and all(t['passed'] for t in tests)
shutil.copyfile(ROOT/'bin/CECHO.8xp',OUT/'CECHO.8xp')
for name in ['LICENSE','THIRD_PARTY.md']: shutil.copyfile(ROOT/name,OUT/name)
shutil.copyfile(ROOT/'docs/USER_GUIDE.md',OUT/'USER_GUIDE.md')
(OUT/'INSTALL.txt').write_text('CEleste Echo (CECHO)\n\nSend CECHO.8xp and clibs.8xg using TI Connect CE. Launch CECHO with your existing compatible CEleste launcher.\nUp/Down select; Enter/2nd start. In game: arrows move, 2nd jumps, Alpha dashes. Enter retries; Graph/Window returns to the selector and saves PBs. Clear/Del quits and saves PBs.\nThe chosen room loops after its normal exit; touch the summit flag to finish the summit.\nGreen minus = faster than the previous room PB. Red plus = slower or tied. Milliseconds use the original 30 Hz clock.\nReturn to the selector or quit before resetting RAM; improvements are not archived between attempts. Keep CEchoA and CEchoB to retain saved PBs. Main-game saves are separate.\n\nNative compilation, host integration tests and transfer-file checks passed. Calculator/emulator play-testing remains pending.\nSee USER_GUIDE.md for the complete guide.\n')
(OUT/'TESTS.txt').write_text('\n'.join(t['output'] for t in tests)+'\nNative eZ80 build and binary/physics integrity verification: PASS.\nCalculator/emulator play-testing: NOT PERFORMED.\n')
names = ['CECHO.8xp','clibs.8xg','INSTALL.txt','USER_GUIDE.md','LICENSE','THIRD_PARTY.md','TESTS.txt']
assert all((OUT/n).is_file() for n in names)
(OUT/'SHA256SUMS.txt').write_text(''.join(hashlib.sha256((OUT/n).read_bytes()).hexdigest()+'  '+n+'\n' for n in names))
archive = OUT/'CEleste-Echo-install.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
    for name in names+['SHA256SUMS.txt']: z.write(OUT/name,name)
with zipfile.ZipFile(archive) as z:
    assert z.testzip() is None
    for row in z.read('SHA256SUMS.txt').decode().splitlines():
        digest,name=row.split('  ')
        assert hashlib.sha256(z.read(name)).hexdigest() == digest
print('PASS install ZIP CRC and SHA256 manifest:',archive.name)
