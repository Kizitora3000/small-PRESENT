import os
import numpy as np
import ctypes

PRESENT_16 = ctypes.CDLL('./PRESENT_16.so')

PRESENT_16.EncryptWrapper.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int]
PRESENT_16.EncryptWrapper.restype = ctypes.c_int
PRESENT_16.DecryptWrapper.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int]
PRESENT_16.DecryptWrapper.restype = ctypes.c_int

""" 
wrapper example

ciphertext  = PRESENT_16.EncryptWrapper(secret_key, plaintext)
decrypttext = PRESENT_16.DecryptWrapper(secret_key, ciphertext)
"""

"""
notation

CP: Ciphertext Prediction
PR: Plaintext Recovery
"""


#secret_key = (ctypes.c_int * 5)(0x0000, 0x0000, 0x0000, 0x0000, 0x0000)
secret_key = (ctypes.c_int * 5)(0x1234, 0x4567, 0x89ab, 0xcdef, 0x1234)

x_CP_dataset = []
y_CP_dataset = []

for plaintext in range(2**16):
    ciphertext = PRESENT_16.EncryptWrapper(secret_key, plaintext, 2)

    x_CP_dataset.append(plaintext)
    y_CP_dataset.append(ciphertext)

split_train_and_test = len(x_CP_dataset) // 2

x_CP_train = x_CP_dataset[:split_train_and_test]
y_CP_train = y_CP_dataset[:split_train_and_test]

x_CP_test = x_CP_dataset[split_train_and_test:]
y_CP_test = y_CP_dataset[split_train_and_test:]

if not os.path.exists("output"):
    os.mkdir("output")

np.save("output/x_CP_train", np.array(x_CP_train))
np.save("output/y_CP_train", np.array(y_CP_train))
np.save("output/x_CP_test",  np.array(x_CP_test))
np.save("output/y_CP_test",  np.array(y_CP_test))