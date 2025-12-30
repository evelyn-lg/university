import numpy as np
import cv2

p01 = cv2.imread('imagen_oscura.jpg')
cv2.imshow('original', p01)
cv2.waitKey(0)
cv2.resize(p01, (110, 110))

# calcular los máximos de cada capa
mb = p01[:, :, 0].max() # la capa 0 es la azul
mg = p01[:, :, 1].max()
mr = p01[:, :, 2].max()

print('Maximo azul: ', mb)

ab = (-np.log(1 - mb / 255)) / mb
ag = (-np.log(1 - mg / 255)) / mg
ar = (-np.log(1 - mr / 255)) / mr
print('ab: ', ab)

# .copy para que no se modifique la imagen original
p02 = p01.copy().astype(float)
p02[:, :, 0] = p02[:, :, 0] / (1 - np.exp(- p02[:, :, 0] * ab) + 0.1) # ecuación del pixel = pix / (1-e^(-a*pix)), la suma de 0.1 es para que no se pase del 255
p02[:, :, 1] = p02[:, :, 1] / (1 - np.exp(- p02[:, :, 1] * ab) + 0.1)
p02[:, :, 2] = p02[:, :, 2] / (1 - np.exp(- p02[:, :, 2] * ab) + 0.1)

cv2.imshow('corregida', p02.astype(np.uint8))
cv2.waitKey(0)