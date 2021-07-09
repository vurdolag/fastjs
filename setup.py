from setuptools import setup, Extension


arg_compile = [
    '-O2'
]


module1 = Extension('fastjs',
                    sources=['src/fastjs.cpp'],
                    extra_compile_args=arg_compile)

setup(
    name='fastjs',
    version='1.0.1',
    description='This is a demo package',
    ext_modules=[module1]
)
