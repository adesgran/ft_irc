
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
| `optval` | `const void *` | content to set for the option |
| `optlen` | `socklen_t` | amount of space (in bytes) pointed to by optval |

#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `error` | `int` | On success, `0` is returned.  On error, `-1` is returned, and `errno` is set to indicate the error. |

#### Description

    Manipulate options for the socket referred to by the file descriptor sockfd
    `errno` set to indicate the error
    see <https://pubs.opengroup.org/onlinepubs/000095399/functions/setsockopt.html> for more






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
| `addr` | `struct sockaddr *restrict` | buffer for the socket name |
| `addrlen` | `socklen_t *restrict` | amount of space (in bytes) pointed to by addr |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `error` | `int` | On success, `0` is returned.  On error, `-1` is returned, and `errno` is set to indicate the error. |

#### Description

    Fill the `addr` buffer with the name of the socket, and rewrite addrlen with the size of addr





# **_getprotobyname_**

#### Include

```c
  #include <netdb.h>
```

#### Prototype

```c
  struct protoent *getprotobyname(const char *name);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `name` | `const char *` | The name of the protocol |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `res` | `struct protoent *` | Protocol that match `name` |

#### Description

    Return the prototype required by the `name`, and `NULL` for an error






# **_gethostbyname_**

#### Include

```c
  #include <netdb.h>
```

#### Prototype

```c
  struct hostent *gethostbyname(const char *name);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `name` | `const char *` |  The name of the host |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `res` | `struct hostent *` | Struct containing host informations |

#### Description

    Return the host struct required by the `name`, and `NULL` for an error.
    Error is described by `h_errno`





# **_getaddrinfo_**

#### Include

```c
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netdb.h>
```

#### Prototype

```c
  int getaddrinfo(const char *restrict node,
                  const char *restrict service,
                  const struct addrinfo *restrict hints, 
                  struct addrinfo **restrict res);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `node` | `const char *restrict` | Internet host identifier |
| `service` | `const char *restrict` | Internet host service |
| `hints` | `const struct addrinfo *restrict` | points to an addrinfo structure that specifies criteria for selecting the socket address structures returned |
| `res` | `struct addrinfo **restrict` | Pointer to the start of a linked list |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `error` | `int` | Return 0 if it succeds or error's macro |

#### Description

    Given node and service, which identify an Internet host and a 
    service, getaddrinfo() returns one or more addrinfo structures in res.

#### Struct

```c
struct addrinfo {
               int              ai_flags;
               int              ai_family;
               int              ai_socktype;
               int              ai_protocol;
               socklen_t        ai_addrlen;
               struct sockaddr *ai_addr;
               char            *ai_canonname;
               struct addrinfo *ai_next;
           };
```






# **_freeaddrinfo_**

#### Include

```c
  #include <sys/socket.h>
  #include <netdb.h>
```

#### Prototype

```c
  void freeaddrinfo(struct addrinfo *ai);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `ai` | `(struct addrinfo *` | ai struct to free |

#### Description

    Free the entire list of structure.






# **_bind_**

#### Include

```c
  #include <sys/socket.h>
```

#### Prototype

```c
  int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `sockfd` | `int` | Socket file descriptor |
| `addr` | `const struct sockaddr *` | Addresse of the structure to fill |
| `addrlen` | `socklen_t` | amount of space (in bytes) pointed to by addr |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `erro` | `int` | On success, `0` is returned.  On error, `-1` is returned, and `errno` is set to indicate the error. |

#### Description

    bind() assigns the address specified by addr to the socket referred to by the file descriptor sockfd.

#### Struct

```c
struct sockaddr {
    sa_family_t sa_family;
    char        sa_data[14];
}
```





# **_connect_**

#### Include

```c
  #include <sys/socket.h>
```

#### Prototype

```c
  int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `sockfd` | `int` | Socket file descriptor |
| `addr` | `const struct sockaddr *` | address to connect with |
| `addrlen` | `socklen_t` | amount of space (in bytes) pointed to by addr |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `error` | `int` | On success, `0` is returned.  On error, `-1` is returned, and `errno` is set to indicate the error. |

#### Description

    The connect() system call connects the socket referred to by the file descriptor sockfd to the address specified by addr.






# **_listen_**

#### Include

```c
  #include <sys/socket.h>
```

#### Prototype

```c
  int listen(int sockfd, int backlog);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `sockfd` | `int` | Socket file descriptor |
| `backlog` | `int` | maximum length to which the queue of pending connections for sockfd may grow |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `error` | `int` | On success, `0` is returned.  On error, `-1` is returned, and `errno` is set to indicate the error. |

#### Description

    listen() marks the socket referred to by sockfd as a passive socket, that is, as a socket that will be used to accept incoming connection requests using accept().





# **_accept_**

#### Include

```c
  #include <sys/socket.h>
```

#### Prototype

```c
  int accept(int sockfd, struct sockaddr *restrict addr, socklen_t *restrict addrlen);
```

#### Parameter

| Parameter | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `sockfd` | `int` | Socket file descriptor |
| `addr` | `struct sockaddr *` | Address of the socket struct |
| `addrlen` | `socklen_t *restrict` | amount of space (in bytes) pointed to by addr |


#### Return

| Return | Type     | Description                |
| :-------- | :------- | :------------------------- |
| `new sockfd` | `int` | New socket file descriptor |

#### Description

    accept() extracts the first connection request on the queue of pending connections for the listening socket, sockfd, creates a new connected socket, and  returns a new file descriptor referring to that socket.






# **_htonl/htons/ntohl/ntohs_**

#### Include

```c
  #include <arpa/inet.h>
```

#### Prototype

```c
  uint32_t htonl(uint32_t hostlong);
  uint16_t htons(uint16_t hostshort);
  uint32_t ntohl(uint32_t netlong);
  uint16_t ntohs(uint16_t netshort);
```

#### Description

    The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.
    
    The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
    
    The ntohl() function converts the unsigned integer netlong from network byte order to host byte order.
    
    The ntohs() function converts the unsigned short integer netshort from network byte order to host byte order.






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
