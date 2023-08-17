# tihsOS-64

64 bit OS.

Make the docker image
```sh
$ docker build buildenv -t tihsos-64-buildenv
```

To compile on Linux/MacOS:
```sh
$ docker run --rm -it -v $pwd:/root/env tihsos-64-buildenv
# if that errors:
$ docker run --rm -it -v $(pwd):/root/env tihsos-64-buildenv
```

To compile on Windows:
```sh
$ docker run --rm -it -v %cd%:/root/env tihsos-64-buildenv
```

