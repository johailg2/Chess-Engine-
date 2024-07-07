from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'chessBoardWrap',
        sources=[
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/wrapper.cpp',
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/chessboard.cpp',
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/bitboards.cpp',
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/attackTables.cpp'
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/bitboards.cpp'
        ],
        include_dirs=[
            pybind11.get_include(),
            'C:/Users/johai/AppData/Local/Programs/Python/Python312/Include',
            'include',
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Headers/attackTables.h'
        ],
        language='c++'
    ),
]

setup(
    name='chessBoardWrap',
    ext_modules=ext_modules,
    zip_safe=False,
)
