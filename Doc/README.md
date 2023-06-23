
# ft_irc

Description des fonctions autorisées


## socket

#### Include

```c
  #include <sys/socket.h>
```
#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `domain` | `int` |  `AF_LOCAL` or `AF_INET` or `AF_INET6` |
| `type` | `macro` | `SOCK_STREAM` (TCP) or `SOCK_DGRAM` (UDP) |
| `protocol` | `int` | Protocol value, `0` for IPv6 |

#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `sockfd` | `int` | socket descriptor |

#### Description

    Generate socket file descriptor

## setsockopt

#### Include

```c
  #include <sys/types.h>
  #include <sys/socket.h>
```
#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `sockfd` | `int` | socket file descriptor |
| `level` | `int` |  |
| `optname` | `int` |  |
| `optval` | `const void *` |  |
| `optlen` | `socklen_t` |  |

#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `error` | `int` | `0` on success, `-1` on error |

#### Description

    Manipulate options for the socket referred to by the file descriptor sockfd


## template

#### Include

```c
  #include <sys/types.h>
```
#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `` | `int` |  |
| `` | `int` |  |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `` | `int` |  |

#### Description

    G
