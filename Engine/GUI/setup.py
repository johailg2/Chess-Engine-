from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'chessBoardWrap',
        sources=[
            '../Src/wrapper.cpp',
            '../Src/chessboard.cpp',
            '../Src/bitboards.cpp',
            '../Src/attackTables.cpp',
            '../Src/eval.cpp'
        ],
        include_dirs=[
            pybind11.get_include(),
            '../Headers',
        ],
        language='c++',
        extra_compile_args=['/std:c++17'] if __import__('sys').platform == 'win32' else ['-std=c++17'],
    ),
]

setup(
    name='chessBoardWrap',
    ext_modules=ext_modules,
    zip_safe=False,
)