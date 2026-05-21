# Clirc

**IRC as a hypertext — click your way around the network like it's IMDB.**

Written in 2002–2003, before embeddable web servers, before vast SDKs and libraries, before modern computers.

## What it does

Clirc crawls IRC networks and builds a browsable database of who hangs out where. Fire up the web server and you get:

- **Channel page** → lists every nick in that channel, when they were last seen
- **Nick page** → lists every channel that nick inhabits
- **Click through** → channel→nick→channel→nick like a Wikipedia rabbit hole
- **Search** → find a nick or channel and jump straight into the graph
- **Affinity** → see how many users two channels share

It turns IRC — a real-time chat protocol — into something you browse asynchronously, following links between nicks and channels the way you'd click from an actor to their filmography and back.

### How the crawl works

```
populate  →  join random channels
                ↓
           NAMREPLY lists users → hashadd(channel, user)
                ↓
           WHOIS every new user → RWHOISCHANNELS lists their channels
                ↓
           hashadd(user, channel) → join new channels → repeat
```

Each WHOIS reveals more channels. Each channel reveals more users. The graph grows outward like a BFS, mapping the social topology of the network.

### The data structure

Two hash tables hold the entire graph:

- `p_names` — every nick, each with pointers to the channels they're in
- `p_chans` — every channel, each with pointers to the nicks inside it

An edge is just a pointer from one table to the other. `hashadd(nick, channel)` adds both sides. `getaffinity(channelA, channelB)` counts shared nicks. `walk()` DFS-traverses the graph and prints the tree.

### Web UI

The HTTP server (`server start 8080`) generates an inline-CSS HTML interface from `sprintf` calls. No templates, no framework, no dependencies. Browse the entire database with pagination, search, and click-through navigation.

```sh
./clirc
>> connect
>> populate 50     # crawl 50 random channels
>> server start    # open http://localhost:8080/
```

## Also happens to be an IRC client

Full interactive CLI with multi-server support, line editing, command history, tab completion, per-channel message buffers, raw IRC mode, CTCP, the works. But that's really just the engine for the crawler.

### Bonus features

- **IRC bouncer** — `bouncer start 5555`
- **Remote CLI** — telnet in from another terminal
- **Scripting** — `label`/`goto`/`if` for automation
- **Config persistence** — `save`/`load`

The idea was: connect via dial-up, `populate 200`, hang up, then browse the results offline through the web server. IRC as a read-only database you explored at your leisure — no need to stay connected once the crawl was done.

## Build

```sh
make
```

No dependencies. POSIX C and a compiler. Will compile on that 486. Might take a while.

## Commands

### Crawling
| Command | |
|---------|-|
| `populate` | Join N random channels to discover the graph |
| `affinity <#chan>` | Compute channel overlap via WHOIS |
| `walk` | Recursive DFS of the nick↔channel graph |
| `list` | `/LIST` or show cached channels |
| `join` / `part` | Join/leave a channel |
| `chat` | Interactive chat for a channel |

### Servers
| Command | |
|---------|-|
| `server start [port]` | HTTP webserver (default 8080) |
| `server stop` | Stop webserver |
| `bouncer start [port]` | IRC bouncer (default 5555) |
| `remote start [port]` | Remote CLI access |

### Connection & config
`connect` / `disconnect` / `context` / `raw` / `set` / `show` / `save` / `load`

## Architecture

```
p_names (nicks)          p_chans (channels)
   │                          │
   nodes[] ──→ channel node   nodes[] ──→ nick node
   (pointers to p_chans)      (pointers to p_names)
```

The web UI (`urlprint` in `hash.c`) renders this as hyperlinked HTML. Click a nick → see their channels (each clickable). Click a channel → see its members (each clickable). Same traversal as `walk()` but in a browser.

## License

WTFPL v2 — see [WTFPL-LICENSE](./WTFPL-LICENSE).
