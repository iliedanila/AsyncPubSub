# AGENTS.md

Canonical shared guidance for coding agents and automated reviewers working on
AsyncPubSub. It applies to the whole repository, including the `MeshNetwork/`
submodule. Agent-specific entry points refer here instead of keeping separate
rules.

Claude Code reads this file directly as long as the repo has no `CLAUDE.md`,
`.claude/CLAUDE.md` or `CLAUDE.local.md`. If one is ever added, it must contain
`@AGENTS.md`, or this file stops loading.

AsyncPubSub is a C++17 publish/subscribe system on top of a self-routing TCP
mesh. Every node is one process (or one `Node` object); nodes discover each
other by exchanging routing tables, and pub/sub roles ride on top as opaque
payloads.

## Working Agreement

- Verify facts from the current worktree before asserting them: paths, flags,
  ports, message types, versions, behavior. Ground each claim in the file read
  or command output it came from. If something remains unknown or a check
  errors, say so rather than reporting success.
- Before editing, inspect the relevant code, documentation, and `git status`
  (plus `git -C MeshNetwork status`). Preserve user-owned changes and do not
  clean up unrelated code.
- State assumptions when they affect scope, behavior, architecture, or the wire
  format. If several interpretations would produce materially different
  results, surface them instead of choosing silently. Proceed with reasonable
  assumptions for reversible steps within the authorized scope; ask when a
  missing decision is necessary to proceed correctly.
- For bugs, check the known-issues table first, then diagnose observed
  behavior and reproduce it with a focused topology (`startNode` or
  `playground`; a test once a suite exists). Check both peers of the
  connection: a symptom on one node often originates on the other. Do not make
  speculative fixes.
- Make the smallest change that satisfies the request. Avoid unrelated
  refactors, premature abstractions, broad formatting, and speculative failure
  handling without a plausible contract or failure mode.
- Update the relevant documentation in the same change, including this file:
  fixing a known issue removes its row, changing a flag updates the Run table.
  Documentation is part of completing the work, not a follow-up task.
- Define success in verifiable terms, run the narrowest sufficient checks, and
  report exactly what passed and what was not run. Building is not running;
  running the happy path is not running the failure case. Documentation-only
  changes need path, link and diff checks, not a build.
- Do not install or upgrade the toolchain or Boost unless the task requires it.
- Use the least privilege needed. Do not perform destructive, irreversible,
  publication, or other external-state actions without clear authorization.
  Authorization already given in the task remains valid; do not ask the user to
  reconfirm the same work.
- Verification may start and stop node processes it owns; stop them by PID or
  an exact `pkill -f 'startNode --name X '`. Report port or process conflicts
  instead of killing unrelated processes. Remove diagnostic logging in the same
  change that finds the root cause.

## Working With Ilie

- Reply in English, whatever language the request is written in.
- Do not commit directly to `master` (in this repo or the submodule). Work on a
  feature branch; create one before committing from a detached worktree.
  Preserve unrelated work in every checkout.
- Multi-phase features belong in one PR with separately self-reviewed commits,
  merged with a merge commit rather than squashed or split into stacked PRs,
  unless the user requests otherwise.
- "Address it in the current PR" includes committing and pushing the fix. A
  requested ready PR is non-draft; report pending CI separately from readiness
  for review.
- Self-review before presenting a PR, with rigor scaled to risk. For routing,
  connection lifecycle, or anything crossing the two layers, check interactions
  in both directions (both peers, both layers) and the promised scope against
  the delivered diff. Repeat checks when new changes or evidence justify it,
  not as an open-ended search for zero findings.
- Address review comments as one batch, never one push per comment. Collect
  every unanswered comment on the current head from every reviewer. For each,
  name the class of defect, find every sibling that does the same job (other
  handlers, other message types, the peer side), and fix the class once, the
  way its neighbours already do it. Then self-review the whole PR diff, not
  only the fix delta, run the checks once, and push once. A comment is not a
  license to add features: behavior a comment calls for that the PR did not
  already have is a new change, so propose it instead of bundling it.
- Progress updates during long runs should carry relevant evidence, such as a
  completed check or commit SHA, rather than a bare "still running".

## Layout

| Path | What | Namespace |
|------|------|-----------|
| `MeshNetwork/` | **git submodule** (`github.com/iliedanila/MeshNetwork`). TCP mesh, framing, routing. | `NetworkLayer` |
| `LogicalLayer/` | `Broker`, `Publisher`, `Subscriber` and their messages. | `LogicalLayer` |
| `StartNode/` | CLI: one node, one role per process. Canonical entry point. | — |
| `Playground/` | Logger + broker + 1 publisher + 1 subscriber in one process, driven from stdin. | — |

There is no test suite and no CI. A clean build plus an observed run is the
only verification available.

## Build

```bash
git submodule update --init           # if MeshNetwork/ is empty
./build.sh                            # configure (Release) + build into build/
cmake --build build/ --target startNode   # or playground / LogicalLayer / MeshNetwork
```

Outputs: `build/StartNode/startNode`, `build/Playground/playground`.

Dependencies: CMake, a C++17 compiler, Boost. Code uses `asio`, `variant`,
`algorithm/string` (header-only), `serialization` and `date_time`
(`posix_time` for timers), and `system` (`error_code`). The executables'
CMake files also link `regex`, which no source uses.

**Current toolchain status (checked 2026-10-10, CMake 4.4.3, Boost 1.92.0,
Apple clang 21):** the build **fails** with
`no member named 'deadline_timer' in namespace 'boost::asio'`
(`MeshNetwork/inc/node.hpp:69`, `MeshNetwork/src/node.cpp:72,202`). Boost 1.92's
`<boost/asio.hpp>` no longer pulls in `deadline_timer`; `publisher.hpp` includes
it explicitly and only warns. Until the source is fixed, configure a separate
build dir with the header forced in (the `-I` is needed because CMake's
compiler probe runs before Boost's include path is added; the path shown is
Homebrew's):

```bash
cmake -S . -B /tmp/aps-build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS='-I/opt/homebrew/include -include boost/asio/deadline_timer.hpp'
cmake --build /tmp/aps-build -j
```

Expected noise after that: `deadline_timer is deprecated: Use system_timer`,
CMake "Compatibility with CMake < 3.10" and policy `CMP0167` (FindBoost
removed) warnings.

Do not trust an old binary after a Boost upgrade: executables link Boost
dynamically from `/opt/homebrew/opt/boost/lib`, so a binary built against an
older Boost runs against the new dylibs. Rebuild.

## Run

`startNode` flags (parsed by the hand-rolled `StartNode/src/arguments.cpp`;
keep adding flags there, not via `boost::program_options`):

| Flag | Meaning |
|------|---------|
| `--name N` | Required. Must be unique in the mesh (not checked). |
| `--accept PORT` | Listen (IPv4 only). |
| `--connect HOST PORT` | Connect once. |
| `--connect-reconnect HOST PORT` | Connect; on failure or disconnect retry every 1 s. |
| `--logger` | This node prints `LogMessage`s it receives. |
| `--as broker\|publisher\|subscriber` | Role. Omit for a pure relay node. |
| `--publish TYPE` | Publisher identity; repeatable. |
| `--interval MS` | Publish period. **Required** with `--as publisher` (missing → out-of-range access). |
| `--subscription TYPE` | Repeatable; all values form **one** subscription set. |

A publisher matches a subscription when its identity set is a superset of the
subscription set.

### Smoke test (verified 2026-10-10)

```bash
N=build/StartNode/startNode
stdbuf -oL $N --name broker --accept 7777 --as broker --logger > broker.log 2>&1 &
stdbuf -oL $N --name publisher --connect-reconnect localhost 7777 --as publisher --publish dummyData --interval 1000 > pub.log 2>&1 &
stdbuf -oL $N --name subscriber --connect-reconnect localhost 7777 --as subscriber --subscription dummyData > sub.log 2>&1 &
```

Pass: `broker.log` gains roughly one line per interval like
`LOG -- 2026-10-10 16:14:06 -- subscriber -- Publisher data: dummyData: Dummy value here.`

Observation pitfalls (all confirmed):

- **Buffering.** stdout is block-buffered when redirected and there is no
  signal handling, so `kill` loses unflushed output. Use `stdbuf -oL` (GNU
  coreutils; also at `/usr/bin/stdbuf` on recent macOS) as above.
  Stop nodes by PID or `pkill -f 'startNode --name X '`.
- **One outgoing connection per process.** A second `--connect`/
  `--connect-reconnect` cancels the first (`error connecting: Operation
  canceled`) because `Node` has a single `connect_socket`. Build multi-link
  topologies by pairing one outgoing connect per node with `--accept` on peers.
- **The logger never prints its own logs.** Put `--logger` on a dedicated node
  (no `--as`) when you need to see a role's `node.log()` output.
- **Logger discovery doesn't reach late joiners.** Only the logger itself
  advertises the logger route to new connections (`node.cpp:177`). Connect the
  logger *directly* to the nodes you are observing, or start them in an order
  where they are connected when the logger announces itself.

### Playground

Fixed ports 8888 (logger) and 7777 (broker). Commands on stdin: `start`,
`stop`, `add`, `remove`, `exit`. Commands run immediately, so pace them with
sleeps, and always finish with `exit` — on EOF the input loop spins at 100%
CPU.

```bash
(sleep 3; echo remove; sleep 3; echo add; sleep 5; echo exit) | stdbuf -oL build/Playground/playground
```

### Reconnection

For any change touching connections or routing, kill one side and watch the
other reconverge (`--connect-reconnect` peers retry every second). "Reconnect
works" needs an observed run, not a reading of the code.

## Architecture

- **Framing:** 4-byte big-endian length + body (`MeshNetwork/inc/message.hpp`).
  Body is a `boost::archive::binary_oarchive` of
  `NetworkLayer::MessageVariant` = `boost::variant<RoutingMessage, DataMessage,
  DataMessageAck, LogMessage, Handshake, HandshakeReply>`
  (`MeshNetwork/inc/allMessages.hpp`).
- **Connection setup:** connector sends `Handshake`; acceptor replies
  `HandshakeReply`; both then send a `RoutingMessage` with their table.
- **Routing:** distance-vector. `Node::nodeDistances` (hop count per node) and
  `Node::nodePaths` (next-hop `Connection`) are updated in
  `processAddNodePaths` / `processFailedNodes` and changes are flooded to other
  neighbours. No sequence numbers, no max distance.
- **Send API:** `Node::sndMessage(dest, data, cb)`. If there is no route, `cb`
  fires immediately with `eNoPath`; otherwise it fires when a
  `DataMessageAck` comes back. See known issues for cases where it never fires.
- **Logical layer:** `LogicalLayer::MessageVariant` = `boost::variant<
  BrokerIdentity, AddRemoveSubscriptionMessage, PublisherIdentityMessage,
  StartPublish, StopPublish, SubscriptionData>`, binary-serialized into
  `DataMessage`'s buffer. Flow: broker sends `BrokerIdentity` to every node it
  sees → subscribers send subscriptions, publishers send identity → broker
  matches and sends `StartPublish`/`StopPublish` to publishers → publishers
  send `SubscriptionData` straight to subscribers on a timer.
- **One role per `Node`:** `Node` holds a single `acceptMessages` callback and
  a single `notifyNewNodeStatus` callback; a second registration replaces the
  first.

## Rules

The CMake files govern how things build; these rules govern design invariants;
code and observed runs establish current behavior.

1. **Single thread, one `io_context`.** Anything touching a `Node`/role from
   outside the loop goes through `boost::asio::post(io_context, ...)` (pattern:
   `Playground/src/main.cpp`). No locks.
2. **Layer separation.** Logical concepts never appear in `NetworkLayer`
   types and vice versa; logical traffic only travels inside `DataMessage`
   bodies.
3. **Adding a message** (either layer): define the type with a `serialize()`
   template; append it to the **end** of the relevant `MessageVariant`; add a
   `handleMessage<T>` specialization for every visitor target (`Node`, or
   each of `Broker`, `Publisher`, `Subscriber`). A missing specialization
   shows up as an undefined-symbol **link** error. Never dispatch with
   `dynamic_cast`/RTTI.
4. **Wire compatibility.** Boost serializes a variant as its index then the
   value, so reordering or inserting alternatives breaks running nodes. Binary
   archives are not portable across architectures; treat a mesh as needing
   identical builds.
5. **No `std::cout` in library code.** Use `Node::log()`. MeshNetwork already
   has violations (`connection.cpp:83,111`, `node.cpp:43,66,69,319,334`);
   don't add more. CLI front-ends may print.
6. **Submodule edits.** Changes under `MeshNetwork/` are commits in that repo
   plus a pointer bump in this one. Say so explicitly when proposing them.

## Style and Commits

- `.clang-format`: Google base, 4-space indent, 80 columns,
  `PointerAlignment: Left`, `AllowShortIfStatementsOnASingleLine: Never`. Run
  `clang-format -i` on touched files.
- Braces on every `if` is a project convention; clang-format does not enforce
  it (`InsertBraces` is unset) and a few existing `if`s lack them.
- Naming: `PascalCase` types, `camelCase` functions/members/files
  (`publisherIdentity.hpp`), no member prefix.
- Use Conventional Commit subjects such as `fix(mesh): ...`,
  `feat(broker): ...`, `build: ...` or `docs: ...` (older history predates
  this). PRs explain the change, link relevant issues, and list the
  verification commands with what was observed.

## Known issues (as of 2026-10-10)

Listed so they aren't mistaken for regressions. "Observed" = reproduced with
the commands above; "code" = read in source, not reproduced.

| Issue | Where | Evidence |
|-------|-------|----------|
| Build fails on Boost 1.92 (`deadline_timer`). | `node.hpp:69` | observed |
| One malformed frame kills a node: `printf '\x00\x00\x00\x03abc' \| nc localhost 7777` → `input stream error`, process exits. Deserialization exception escapes `io_context.run()`. | `connection.cpp:102-106` | observed |
| Second outgoing connect cancels the first. | `node.hpp:102`, `StartNode/src/main.cpp:48-59` | observed |
| Logger drops its own logs; nodes joining later never learn the logger. | `node.cpp:177`, `node.cpp:433` | observed |
| In a cyclic mesh a node adds itself as a remote node and routes messages to itself (`A -- New node in network: A`). | `processAddNodePaths`, `node.cpp:343-350` | observed |
| Routing livelock: two neighbours at the same logger distance keep replying to each other (`<` should be strict, cf. `node.cpp:366`). In a triangle containing the logger, the two non-logger nodes idle at ~42% CPU each. | `node.cpp:408` | observed (CPU) |
| Count-to-infinity: when a node leaves a ring, failure notices are answered with stale routes (`node.cpp:390-392`) and the dead route circulates. Ring A-B-C-D + X: killing X takes every ring node from 0% to ~28% CPU, sustained. `DataMessage::distance` is incremented but never capped. | `processFailedNodes` | observed (CPU) |
| `closeConnection` never resets `loggerDistance`; after the logger link drops and returns, nodes further out stop logging. | `node.cpp:133-167` | code |
| Subscriber never re-subscribes after a broker restart; with a publisher restart too, delivery stops for good. `Subscriber` never registers `handleNewNodeStatus`. | `subscriber.cpp:17-22` | observed |
| Playground spins at 100% CPU on stdin EOF. | `Playground/src/main.cpp:93-158` | observed |
| `eNoPath` (mid-route) and `eNodeNotAccepting` acks are built with source/destination swapped, so the sender never gets them. | `node.cpp:251,263` | code |
| Send callback never fires if the route dies after sending (no timeout); entry stays in `ackCallbacks`. | `node.cpp:111-127` | code |
| `messageID = std::hash(payload)`: identical payloads in flight collide and the later callback is dropped. | `node.cpp:117` | code |
| A failed write leaves the queue head in place, so the connection never writes again. | `connection.cpp:35-45` | code |
| Length header is trusted: a peer can make a node allocate up to 4 GiB. | `message.hpp:38-44` | code |
| Publishers keep dead subscribers forever (deliberate, commit `651bc65`); broker sends no `StopPublish` on node loss. | `publisher.cpp:69-73`, `broker.cpp:73-83` | code |
| `Node::connect` resolves DNS synchronously on the loop thread. | `node.cpp:53-54` | code |
| `Arguments::GetParameters` doesn't bounds-check (`--name` as last arg is UB). | `arguments.cpp:24` | code |
| No authentication or encryption; any TCP client can join the mesh. | — | code |

## Handoff

Summarize what changed, why, and the exact checks run (build targets,
topologies, what the logs showed). Call out skipped checks, residual risks,
assumptions, and unrelated issues separately; do not present unverified
behavior as complete.
