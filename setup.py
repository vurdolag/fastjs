from setuptools import setup, Extension


arg_compile = [
    '-O2'
]


module1 = Extension('fastjs',
                    sources=['src/fastjs.cpp', 'src/validator.cpp'],
                    extra_compile_args=arg_compile)

with open("README.md", encoding="utf-8") as f:
    long_description = f.read()


setup(
    name='fastjs',
    version='1.0.2',
    description='Faster json decoder, encoder, validator, written in c++ for Python',
    long_description=long_description,
    ext_modules=[module1],
    python_requires=">=3.6",
    project_urls={"Source": "https://github.com/vurdolag/fastjs"},
    download_url="https://github.com/vurdolag/fastjs",
    platforms=["any"],
    url="https://github.com/vurdolag/fastjs",
    author="https://github.com/vurdolag",

    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Operating System :: OS Independent',
        'License :: OSI Approved :: BSD License',
        'Programming Language :: C',
        'Programming Language :: C++',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3 :: Only'
    ],
)
