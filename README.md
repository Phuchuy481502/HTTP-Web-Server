# HTTP-Web-Server

A minimal HTTP/1.1 web server written in pure C. Handles multiple clients concurrently using `fork()`.

## Features

- TCP socket server on port ....
- Parses GET requests and routes by path
- Serves HTML responses with correct HTTP headers
- Handles concurrent clients via `fork()`
- Returns 404 for unknown routes

## Routes

| Path | Response |
|---|---|
| `/` | Home page |
| `/about` | About page |
| anything else | 404 |

## Build

```bash
gcc -Wall -o webserver server.c
./webserver
```

Then open `http://localhost:8080` in your browser, or test with curl:

```bash
curl http://localhost:8080
curl http://localhost:8080/about
```

## How it works

```
socket() → bind() → listen() → accept() → fork() → handle_client()
```

Each incoming connection is handled by a child process, so the server stays free to accept new clients immediately.

## Requirements

GCC on Linux/macOS or WSL on Windows.

