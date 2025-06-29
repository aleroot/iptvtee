"""iptvtee.py - a simple wrapper around libiptvtee; importing a calss from C++"""
import ctypes, platform 

if platform.system() == "Darwin":
    #Load libVLC since macOS Python builds ignore DYLD_LIBRARY_PATH due to hardened runtime
    DYLD_LIBRARY_PATH = "/Applications/VLC.app/Contents/MacOS/lib"
    ctypes.cdll.LoadLibrary(f'{DYLD_LIBRARY_PATH}/libvlccore.dylib')
    ctypes.cdll.LoadLibrary(f'{DYLD_LIBRARY_PATH}/libvlc.dylib')
    lib = ctypes.cdll.LoadLibrary('./libiptvtee.dylib')
else:
    lib = ctypes.cdll.LoadLibrary('./libiptvtee.so')

class Analyzer(object):
    def __init__(self, val):
        lib.Analyzer_new.argtypes = [ctypes.c_char_p]
        lib.Analyzer_new.restype = ctypes.c_void_p

        lib.Analyzer_score.argtypes = [ctypes.c_void_p]
        lib.Analyzer_score.restype = ctypes.c_float
        
        lib.Analyzer_delete.argtypes = [ctypes.c_void_p]

        self.obj = lib.Analyzer_new(val)


    def score(self):
        return lib.Analyzer_score(self.obj)
        
    def __del__(self):
        """destruct unmanaged object"""
        return lib.Analyzer_delete(self.obj)
