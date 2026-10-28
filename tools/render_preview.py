from pathlib import Path
from PIL import Image
ROOT = Path(__file__).resolve().parents[1]
image = Image.open(ROOT/'build/selector-layout.ppm')
image.save(ROOT/'docs/selector-preview.png')
image.resize((640,480),Image.Resampling.NEAREST).save(ROOT/'docs/selector-preview@2x.png')
print('Rendered host selector layout preview; not an emulator capture')
