# MeowTTP

a minimalistic http server written in C. there is no args, only enviroment variables. it is small, because it don't need an inetd service like microhttpd (they have a pretty good codebase tho).

## Features
- Minimal
- Low level streaming

## Will be a feature
- [ ] Directory traversial (use chroot as temporary fix, the executable is static tho)
- [ ] Partial
- [ ] Index

## Issues
- [ ] Not yet the partial (well this cause big files prune)

## Manual

### Enviroment Variables

- `MTTP_PORT`:`int` - Yea the port, it is parsed as unsigned long so please don't break it.
- `MTTP_ROOT`:`str` - Yea the init directory. default to "current work dir"

