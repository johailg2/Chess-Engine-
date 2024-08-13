from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        'chessBoardWrap',
        sources=[
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/wrapper.cpp',
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/chessboard.cpp',
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/bitboards.cpp',
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/attackTables.cpp',
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Src/eval.cpp'
        ],
        include_dirs=[
            pybind11.get_include(),
            'C:/Users/johai/AppData/Local/Programs/Python/Python312/Include',
            'C:/Users/johai/Desktop/Chess Engine Attempt/Engine/Headers',  # Include directory containing headers
            'include'
        ],
        language='c++',
        extra_compile_args=[],  # Ensure C++11 or later
    ),
]

setup(
    name='chessBoardWrap',
    ext_modules=ext_modules,
    zip_safe=False,
)
