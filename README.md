# FAST WORK

A c library for FAST netWORK.

Support:
- TCP
- UDP 
- Windows

## Table of Contents

- [Install](#install)
- [Usage](#usage)
- [Example](#example)
- [Maintainers](#maintainers)
- [Contributing](#contributing)
- [License](#license)

## Install

compile and run the test server program:
```cmd
make run_server

```
or

```cmd
gcc -o server.c server.exe -I src/include -lws32ç2
```

compile and run the test client program:
```cmd
make run_client

```
or

```cmd
gcc -o client.c client.exe -I src/include -lws32ç2
```

the library is in the folder [src/include/fast_work](src/include/fast_work)

## Usage

I try to comment stuff in the [example](#example) and in the library to make it easier

include the library has a header:
```c
#include "fast_work/fw.h"
```

include the library has a header and an implementation:
```c
#define FW_IMPL
#include "fast_work/fw.h"
```

initialize and quit the library:
```c
int main() {
    if (!fw_init()) { // all function that can failed return a boolean -> true if didn't failed or false otherwise
        printf("Can't initialize fast work");
        return 1;
    }

    // ...

    if (!fw_quit()) {
        printf("Can't quit fast work");
        return 1;
    }
    return 0;
}
```

for the actual network part see function that start with FwConn_ or FwAddr_ 
(it mean that the function take FwConn * or FwAddr has first arguments and works like methods)

## Example

A better example are in the file [server.c](server.c) and [client.c](client.c)

## Maintainers

[@raiseFlaymeException](https://github.com/raiseFlaymeException).

## Contributing

Feel free to contibute [Open an issue](https://github.com/raiseFlaymeException/fast_work/issues/new) or submit PRs.

## License

[ZLIB](LICENSE) © raiseFlaymeException
