# A C++ source code for the reversi orientation

## Requirements

* gcc (>=5.4) / clang (>=3.8)
* cmake (>=2.8)

## How to compile

    $ git clone git@kiwi.logos.ic.i.u-tokyo.ac.jp:shared/reversi_sample_cpp.git
    $ cd reversi_sample_cpp
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make -j 4

### Debug build

    $ cmake .. -DCMAKE_BUILD_TYPE=Debug

* The assertions are enabled.
* The compiled executable file contains debug symbols.

### Release build

    $ cmake .. -DCMAKE_BUILD_TYPE=Release

* The assertions are disabled.
* The compiled program runs fast.

## How to run

    $ ./reversi BLACK_PLAYER_TYPE WHITE_PLAYER_TYPE

For example

    $ ./reversi human sample

Please type

    $ ./reversi --help

for more information.

## Editing the code

### Creating your own git repository

    $ git remote set-url origin {URL of your repository}
    $ git push -u origin master

### How to commit and push your changes

After you edit the source codes,

    $ git add {editted files}
    $ git commit
    $ git push

### Adding new files

If you create a new `.cpp` file, you should edit the `CMakeLists.txt`.

### Formatting the code

You can format your source code with `clang-format`.

    $ cp .clang-format.sample .clang-format
    $ clang-format -i *.cpp *.hpp player/*.cpp player/*.hpp

You can change the formatting rules by edditing the `.clang-format`.
