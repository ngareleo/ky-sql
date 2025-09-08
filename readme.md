# ky-sql

This is an attempt at building a minimal database server. Fully custom and heavily inspired by sqlite.

## Mahiko

Mahiko is the custom build system to improve dev loop. You need to give executable permissions:

```bash
chmod +x mahiko
```

Next, you need to install [`cmake`](https://cmake.org/download/)

## Examples

There are code examples in [Examples](./src/Examples), we're using to test specific functionality of the database engine. They are numbered from `0 - n` depending on the order. [Check this section of the docs](./docs/engine#storage-and-retreival) to know the order.

To build examples

```bash
./mahiko build examples {index}
```

To run example

```bash
./mahiko run examples {index}
```
