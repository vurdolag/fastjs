from setuptools import setup, Extension


arg_compile = [
    '-std:c++17', '-O2',
    #'/arch:AVX'
]


module1 = Extension('zjson',
                    sources=['src/zjson.cpp'],
                    extra_compile_args=arg_compile)

setup(
    name='zjson',
    version='1.0.0',
    description='This is a demo package',
    ext_modules=[module1]
)
