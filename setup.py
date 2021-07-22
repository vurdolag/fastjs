from setuptools import setup, Extension


arg_compile = [
    '-O2'
]


module1 = Extension('fastjs',
                    sources=['src/fastjs.cpp', 'src/validator.cpp'],
                    extra_compile_args=arg_compile)

setup(
    name='fastjs',
    version='1.0.2',
    description='Faster json decoder, encoder, validator, written in c++ for Python',
    ext_modules=[module1]
)
