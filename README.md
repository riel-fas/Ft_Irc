*This project has been created as part of the 42 curriculum by riel-fas, zben-oma, yabenman

---

# ft_irc — Internet Relay Chat Server

## Description

ft_irc is a fully functional IRC server written in **C++98**, built as part of the 42 school curriculum. The goal is to implement a real IRC server from scratch that real IRC clients (such as LimeChat, HexChat, or irssi) can connect to and use normally.

The server handles multiple simultaneous clients using a single `poll()` call with non-blocking I/O — no forking, no threading. It implements the core IRC protocol including authentication, channel management, operator privileges, and a bonus part that includes a bot and file transfer.

Key features implemented:

- Multi-client handling via `poll()` with non-blocking sockets
- Full client authentication flow: `PASS`, `NICK`, `USER`
- Channel operations: `JOIN`, `PRIVMSG`, `NOTICE`, `TOPIC`, `KICK`, `INVITE`
- Channel modes: `i` (invite-only), `t` (topic restriction), `k` (key/password), `o` (operator), `l` (user limit)
- Proper IRC numeric replies (001–004 welcome sequence, error codes)
- Graceful signal handling (`SIGINT`, `SIGQUIT`, `SIGPIPE`)
- Partial message buffering (TCP stream reassembly)
- **Bonus:** A Moroccan-flavored IRC bot (`l7aj_bot`) responding to `!salam` and `!nasi7a`

---

## Instructions

### Requirements

- A C++98-compatible compiler (`c++` / `g++`)
- A Unix/Linux or macOS system
- An IRC client (e.g., [LimeChat](http://limechat.net/mac/), [HexChat](https://dl.hexchat.net/hexchat/osx/), [irssi](https://irssi.org/))

### Compilation

```bash
# Compile the IRC server
make
# Compile the bot (bonus)
make bonus
### Running the Server
./ircserv <port> <password>
```

**Example:**
```bash
./ircserv 6969 mypassword
```
- `port`: The TCP port to listen on (1–65535)
- `password`: The connection password required by all clients

### Connecting with an IRC Client

In your IRC client, connect to:

```
Server:   127.0.0.1
Port:     6969
Password: mypassword
```

### Running the Bot (Bonus)

```bash
./l7aj_bot <server_ip> <port> <password>
```

**Example:**
```bash
./l7aj_bot 127.0.0.1 6969 mypassword
```

Once connected, the bot (`l7aj`) responds to two commands sent via `PRIVMSG`:

| Command   | Response                              |
|-----------|---------------------------------------|
| `!salam`  | Returns a Moroccan-style greeting     |
| `!nasi7a` | Returns a random Moroccan proverb/advice |

**Example in IRC client:**
```
/msg l7aj !nasi7a
/msg l7aj !salam
```

### Testing with netcat

To verify partial message handling (as required by the subject):

```bash
nc -C 127.0.0.1 6969
```

Then type parts of a command and press `Ctrl+D` between fragments to simulate partial TCP delivery.

### Makefile Rules

| Rule     | Description                          |
|----------|--------------------------------------|
| `all`    | Compile the server                   |
| `clean`  | Remove object files                  |
| `fclean` | Remove object files and binary       |
| `re`     | Full recompile                       |

---

## Supported Commands

| Command   | Description                                      |
|-----------|--------------------------------------------------|
| `PASS`    | Authenticate with the server password            |
| `NICK`    | Set or change nickname                           |
| `USER`    | Set username and realname                        |
| `PING`    | Keepalive — server replies with `PONG`           |
| `JOIN`    | Join or create a channel                         |
| `PRIVMSG` | Send a message to a user or channel              |
| `NOTICE`  | Send a notice (no auto-reply)                    |
| `TOPIC`   | View or set a channel topic                      |
| `KICK`    | Remove a user from a channel (operators only)    |
| `INVITE`  | Invite a user to a channel (operators only)      |
| `MODE`    | Set channel modes (operators only)               |

### Channel Modes

| Mode | Description                          |
|------|--------------------------------------|
| `i`  | Invite-only                          |
| `t`  | Topic settable by operators only     |
| `k`  | Channel key (password)               |
| `o`  | Grant/revoke operator privilege      |
| `l`  | Set user limit                       |

---

## Resources

### IRC Protocol References

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [What is a socker - tutorialspoint](https://www.tutorialspoint.com/unix_sockets/what_is_socket.htm)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — Great reference for socket programming in C/C++

### Tools

- [HexChat](https://dl.hexchat.net/hexchat/osx/) — extensible IRC client used as reference client
- `nc` (netcat) — used to test raw TCP/partial message delivery

### AI Usage

- **RFC 1459 Summary and Understanding:** Breaking down and clarifying the RFC 1459 specifications to help design the core architecture and command structures for the IRC protocol.
- **Core Concepts:** Assisting with the comprehension of fundamental network programming concepts such as socket multiplexing, `poll()`...etc.
