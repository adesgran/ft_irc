
# ft_irc

Description des fonctions autorisées






# **_socket_**

#### Include

```c
  #include <sys/socket.h>
```

#### Prototype

```c
  int socket(int domain, int type, int protocol);
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






# **_setsockopt_**

#### Include

```c
  #include <sys/types.h>
  #include <sys/socket.h>
```

#### Prototype

```c
  int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `sockfd` | `int` | socket file descriptor |
| `level` | `int` | level at which the option resides |
| `optname` | `int` | specify option to set |
| `optval` | `const void *` |  |
| `optlen` | `socklen_t` |  |

#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `error` | `int` | On success, `0` is returned.  On error, `-1` is returned, and `errno` is set to indicate the error. |

#### Description

    Manipulate options for the socket referred to by the file descriptor sockfd
    `errno` set to indicate the error
    see https://pubs.opengroup.org/onlinepubs/000095399/functions/setsockopt.html for more






# **_getsockname_**

#### Include

```c
  #include <sys/socket.h>
```

#### Prototype

```c
  int getsockname(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `sockfd` | `int` | socket file descriptor |
| `addr` | `struct sockaddr *restrict` |   |
| `addrlen` | `socklen_t *restrict` | amount of space (in bytes) pointed to by addr |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `error` | `int` | On success, `0` is returned.  On error, `-1` is returned, and `errno` is set to indicate the error. |

#### Description

    G






# **_template_**

#### Include

```c
  #include <sys/types.h>
```

#### Prototype

```c
  void f(void);
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
