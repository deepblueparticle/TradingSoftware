# IB_api

Our mirror of http://interactivebrokers.github.io/# stable version

We created it because IB API provded without any build system

## How to build?

1. clone the project
2. run `make all install`, it will build IB API and create a folder `install` with `lib` and `include` inside.

## How to update to newer version of IB API?

First, don't update to any non-stable version.

If there is new stable IB API version appeared then do:
1. download new IB API
2. unpack it in this repo according to folder structure
3. check that build works
4. commit and push
5. assign new git tag using `git tag 9.72.18` (<- change to proper version number) and push tag to remote by `git push --tags`
