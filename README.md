<div align="center">
  <h1>🌐 ft_irc</h1>
  <p><i>A fully functional Internet Relay Chat server written in <b>C++98</b>.</i></p>
  <p>
    <img src="https://img.shields.io/badge/C++-98-00599C?style=for-the-badge&logo=c%2B%2B" alt="C++98" />
    <img src="https://img.shields.io/badge/-%20-black?style=for-the-badge&logo=42&logoColor=white" alt="42" />
    <img src="https://img.shields.io/badge/-1337-black?style=for-the-badge" alt="1337" />
  </p>
  <p>
    <b>Collaborators:</b>
    <b>riel-fas</b> •
    <b>zben-oma</b> •
    <b>yabenman</b>
  </p>
</div>

---

## 📖 Description

`ft_irc` is a custom IRC server implemented entirely in C++98, designed as part of the 42 curriculum. The core objective is to replicate a functioning IRC server from scratch, enabling interactions with real-world IRC clients such as **LimeChat**, **HexChat**, and **irssi**.

Operating solely on non-blocking I/O with a single `poll()` call, the server effortlessly handles multiple simultaneous clients—avoiding the overhead of threading or forking. It successfully enforces the core IRC protocols, including robust authentication, channel operations, user modes, and operator privileges. 

As a **Bonus**, the project features a Moroccan-flavored IRC Bot for dynamic interactions, as well as file transfer capabilities.

## ✨ Key Features

- **Robust Concurrency**: Multi-client handling via `poll()` utilizing non-blocking sockets.
- **Client Authentication**: Complete authentication sequence handling (`PASS`, `NICK`, `USER`).
- **Channel Operations**: Supports joining and managing channels (`JOIN`, `PRIVMSG`, `NOTICE`, `TOPIC`, `KICK`, `INVITE`).
- **Access Control Modes**: Advanced channel modes implemented:
  - `i` : Invite-only channels
  - `t` : Topic modifications restricted to operators
  - `k` : Password-protected channels (key)
  - `o` : Channel operator privileges (grant/revoke)
  - `l` : Maximum user limit per channel
- **RFC 1459 Compliant**: Fully adheres to the core Internet Relay Chat specifications (RFC 1459), including proper IRC numeric replies (e.g., 001–004 sequence) and expected client-server interactions.
- **Graceful Error Handling**: Resilient signal handling (`SIGINT`, `SIGQUIT`, `SIGPIPE`).
- **Message Assembly**: Effective partial message buffering and TCP stream reassembly to handle fragmented data precisely.
- **Bonus Bot Integration**: `l7aj_bot` providing culturally inspired Moroccan responses (`!salam`, `!nasi7a`).

---

## 🚀 Getting Started

### Prerequisites

To compile and run this server, ensure you have:
- A modern Unix/Linux or macOS system.
- A **C++98-compatible compiler** (e.g., `c++`, `g++`, `clang++`).
- **Make** installed.
- An IRC client for testing (e.g., [LimeChat](http://limechat.net/mac/), [HexChat](https://dl.hexchat.net/hexchat/osx/), [irssi](https://irssi.org/)).

### Compilation Commands

Use the provided Makefile to compile the server:

```bash
# Compile the IRC server
make

# Compile the bot (bonus)
make bonus
```

### Running the Server

Start the IRC server by specifying a custom port and password:

```bash
./ircserv <port> <password>
```

**Example:**
```bash
./ircserv 6969 mypassword
```
- `<port>`: The TCP port to listen on (typically 1–65535, e.g., 6667).
- `<password>`: The required connection password that all connecting clients must use.

### Connect with an IRC Client

Configure your IRC client (irssi, HexChat, LimeChat, etc.) to use these credentials:

```text
Server:   127.0.0.1
Port:     6969
Password: mypassword
```

---

## 🤖 Running the Bonus Bot (`l7aj`)

```bash
./l7aj_bot <server_ip> <port> <password>
```

**Example:**
```bash
./l7aj_bot 127.0.0.1 6969 mypassword
```

Once connected, the bot (`l7aj`) responds to specific keywords formatted via `PRIVMSG`:

| Trigger | Action |
| --- | --- |
| `!salam` | Responds with a customary Moroccan greeting. |
| `!nasi7a` | Gives random, culturally-rooted Moroccan advice. |

**Usage within the IRC client:**
```text
/msg l7aj !nasi7a
/msg l7aj !salam
```

---

## 📂 Repository Structure

```text
├── srcs/         # Core server logic (Sockets, Poll loop, Command execution)
├── includes/     # Header files and class definitions
├── bot/          # Bonus l7aj bot implementation
└── Makefile      # Build scripts
```

## ⚙️ Under the Hood

<details>
<summary><b>How we handle concurrency without threads</b></summary>
<br>
Unlike traditional multi-threaded servers that spawn a new thread/process for every client, `ft_irc` uses a single-threaded asynchronous approach. 

We utilize a single `poll()` event loop. This system call monitors an array of file descriptors (sockets) to see if any are ready for reading or writing. When a client connects or sends a message, `poll()` alerts us, and we process that specific chunk of data immediately without blocking the entire server. This makes the server exceptionally lightweight and perfectly compliant with the C++98 strict requirements.
</details>

---

### Makefile Reference

| Command | Action |
| --- | --- |
| `make` / `make all` | Compiles the `ircserv` executable. |
| `make bonus` | Compiles the `l7aj_bot` executable. |
| `make clean` | Removes all compiled object (`.o`) files. |
| `make fclean` | Thoroughly cleans by removing `.o` files and executables. |
| `make re` | Performs `fclean` followed by `make all`. |

---

## 📚 Supported IRC Commands

| Command | Functionality |
| --- | --- |
| `PASS` | Submit the connection password. |
| `NICK` | Assign or update a user's nickname. |
| `USER` | Assign user configurations (username and realname). |
| `PING` | Check connectivity; expects a `PONG` response from the server. |
| `JOIN` | Join existing channels or create new ones. |
| `PRIVMSG` | Dispatch a private message to users or broadcast to a channel. |
| `NOTICE` | Similar to PRVMSG, but ignores automated replies (avoids loops). |
| `TOPIC` | Set, display, or clear a channel's topic. |
| `KICK` | Eject a client from a channel *(Requires Operator)*. |
| `INVITE` | Exclusively invite a user into a channel *(Useful for +i mode)*. |
| `MODE` | Alter channel flags *(Requires Operator)*. |

---

## 📖 Essential Resources

### IRC Specifications
- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/)

### Network Programming Concepts
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Tutorialspoint — What is a Socket?](https://www.tutorialspoint.com/unix_sockets/what_is_socket.htm)

### Recommended Tools
- [HexChat](https://dl.hexchat.net/hexchat/osx/) — Great open-source client for real-world interactions.
- `nc` (netcat) — Command-line tool essential for raw TCP stream simulation.

