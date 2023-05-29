import ctypes

lib = ctypes.CDLL('./PRESENT_16.so')

lib.PRESENT_16_wrapper_for_python.argtypes = [ctypes.c_int]
lib.PRESENT_16_wrapper_for_python.restype = ctypes.c_int

result = lib.PRESENT_16_wrapper_for_python(0x00)

print(hex(result))