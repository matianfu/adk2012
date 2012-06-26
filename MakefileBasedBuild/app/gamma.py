# Generates the precomputed gamma correction table

import math

gamma = 2.2
scale = 4095

print('#ifndef LEDGamma_h')
print('#define LEDGamma_h')
print('const uint16_t led_gamma[256] =')
print('{')
for i in range(256):
	print( '\t' + str(int(round(scale * math.pow((float(i) / 255), gamma)))) + ',')
print('};')
print('#endif')
