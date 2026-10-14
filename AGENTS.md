# AGENTS.md

Canonical shared guidance for coding agents and automated reviewers working on
AsyncPubSub. It applies to the whole repository, including the `MeshNetwork/`
submodule. Agent-specific entry points refer here instead of keeping separate
rules.

Claude Code reads this file directly as long as the repo has no `CLAUDE.md`,
`.claude/CLAUDE.md` or `CLAUDE.local.md`. If one is ever added, it must contain
`@AGENTS.md`, or this file stops loading.

AsyncPubSub is a C++20 publish/subscribe system on top of a self-routing TCP
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
  behavior and reproduce it with a failing test (or a `startNode` topology when
  a test can't express it). Check both peers of the connection: a symptom on
  one node often originates on the other. Do not make speculative fixes.
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

There is a CTest suite (see Tests) and no CI.

## Build

```bash
git submodule update --init           # if MeshNetwork/ is empty
./build.sh                            # configure (Release) + build into build/
cmake --build build/ --target startNode   # or playground / LogicalLayer / MeshNetwork
```

Outputs: `build/StartNode/startNode`, `build/Playground/playground`.

Dependencies: CMake, a C++20 compiler, Boost. Code uses `asio`, `variant`,
`algorithm/string` (header-only), `serialization`, and `system`
(`error_code`). Timers are `asio::steady_timer` with `std::chrono`.

Checked 2026-10-10 with CMake 4.4.3, Boost 1.92.0 and Apple clang 21: the
build is clean apart from CMake's "Compatibility with CMake < 3.10" and policy
`CMP0167` (FindBoost removed) warnings. The same day the `Dockerfile` image
(Ubuntu 26.04.1 LTS, GCC 15.2, Boost 1.90, aarch64) built without compiler
warnings and passed every test (`docker build -t asyncpubsub .`, then
`docker run --rm asyncpubsub sh -c 'cd build && ctest'`). The `Dockerfile`
uses `ubuntu:latest`, so that result can change.

Do not trust an old binary after a Boost upgrade: executables link Boost
dynamically from `/opt/homebrew/opt/boost/lib`, so a binary built against an
older Boost runs against the new dylibs. Rebuild.

## Tests

```bash
cmake --build build && (cd build && ctest -j 8)
```

- `MeshNetwork/tests/` (`mesh.*`): `Node`/`Connection` behavior. Each test
  builds a topology in-process on one `io_context` and drives it with the
  helpers in `testSupport.hpp` (`runUntil`, `runUntilQuiet`, `CoutCapture`).
  MeshNetwork also builds and runs these on its own (`cmake -S MeshNetwork`).
- `LogicalLayer/tests/` (`logical.*`): broker, publisher and subscriber
  behavior, using the same helpers.
- `cli.*` (in `StartNode/` and `Playground/` CMake files): the executables'
  argument and stdin handling.

Tests use fixed ports 47000-47300; `cli.playgroundExitsOnEof` needs the
playground's 7777 and 8888. A test destroys a `Node` only after `close()` and
a short `handlersRunIn` drain, and destroys a role before its `Node`. Add
the failing test first, register its name in the directory's
`CMakeLists.txt`, then fix.

## Run

`startNode` flags (parsed by the hand-rolled `StartNode/src/arguments.cpp`;
keep adding flags there, not via `boost::program_options`):

| Flag | Meaning |
|------|---------|
| `--name N` | Required. Must be unique in the mesh (not checked). |
| `--accept PORT` | Listen (IPv4 only). |
| `--connect HOST PORT` | Connect once. Repeatable. |
| `--connect-reconnect HOST PORT` | Connect; on failure or disconnect retry every 1 s. Repeatable. |
| `--logger` | This node prints `LogMessage`s it receives. |
| `--as broker\|publisher\|subscriber` | Role. Omit for a pure relay node. |
| `--publish TYPE` | Publisher identity; repeatable. |
| `--interval MS` | Publish period. Required with `--as publisher`. |
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

Stdout is block-buffered when redirected and there is no signal handling, so
`kill` loses unflushed output. Use `stdbuf -oL` (GNU coreutils; also at
`/usr/bin/stdbuf` on recent macOS) as above, and stop nodes by PID or
`pkill -f 'startNode --name X '`. Errors in flags or arguments print a message
and exit with status 1.

### Playground

Fixed ports 8888 (logger) and 7777 (broker). Commands on stdin: `start`,
`stop`, `add`, `remove`, `exit`. Commands run immediately, so pace them with
sleeps. Closing stdin acts as `exit`.

```bash
(sleep 3; echo remove; sleep 3; echo add; sleep 5; echo exit) | stdbuf -oL build/Playground/playground
```

### Reconnection

For any change touching connections or routing, kill one side and watch the
other reconverge (`--connect-reconnect` peers retry every second). "Reconnect
works" needs an observed run, not a reading of the code.

## Architecture

- **Framing:** 4-byte big-endian length + body (`MeshNetwork/inc/message.hpp`),
  at most 16 MiB (`Message::eMaxBodyLength`). A frame that is too large or
  doesn't decode closes that connection only. Body is a `boost::archive::binary_oarchive` of
  `NetworkLayer::MessageVariant` = `boost::variant<RoutingMessage, DataMessage,
  DataMessageAck, LogMessage, Handshake, HandshakeReply>`
  (`MeshNetwork/inc/allMessages.hpp`).
- **Connection setup:** connector sends `Handshake`; acceptor replies
  `HandshakeReply`; both then send a `RoutingMessage` with their table.
- **Routing:** path-vector. `Node::routes` maps each destination to its hop
  list (next hop first) and the next-hop `Connection`. `RoutingMessage` carries
  `RouteAdvert`s (sender first) and `failedNodes` withdrawals. A node ignores
  any advert containing itself, which keeps routes loop-free; such an advert
  from its current next hop withdraws that route. A next hop's update replaces
  its earlier route; another neighbour's route wins only by being shorter. Full
  tables go out on connect; changes go to every neighbour, including the one
  that caused them; a node told it is unreachable answers with its own advert.
  There is no periodic refresh (`processAdverts`, `processFailedNodes`). A
  reroute can reach `notifyNewNodeStatus` as "unreachable" then "reachable".
- **Logger:** a flag on each advert. `Node::log()` prints locally on the
  logger node and otherwise sends to the nearest logger in `routes`.
- **Send API:** `Node::sndMessage(dest, data, cb)` calls `cb(dest, error)`
  exactly once: `eSuccess` or `eNodeNotAccepting` from the destination's ack;
  `eNoPath` if there is no route, a hop has none, or the route is withdrawn
  while waiting (the message may still have been delivered);
  `eMessageTooLarge` above the frame limit. Message IDs are a per-node counter.
- **Logical layer:** `LogicalLayer::MessageVariant` = `boost::variant<
  BrokerIdentity, AddRemoveSubscriptionMessage, PublisherIdentityMessage,
  StartPublish, StopPublish, SubscriptionData>`, binary-serialized into
  `DataMessage`'s buffer. Flow: broker sends `BrokerIdentity` to every node it
  sees → subscribers send subscriptions, publishers send identity → broker
  matches and sends `StartPublish`/`StopPublish` to publishers → publishers
  send `SubscriptionData` straight to subscribers on a timer. Only the broker
  stops a publisher: when it loses a subscriber it sends `StopPublish`, and
  subscribers re-send their subscriptions every time a broker announces
  itself, so a reroute pauses delivery instead of ending it.
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
5. **No `std::cout` in library code.** Use `Node::log()`. MeshNetwork still
   prints connection events and errors (`connection.cpp`, `node.cpp`), and the
   logger node prints received logs; don't add more. CLI front-ends may print.
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

Listed so they aren't mistaken for regressions. "Code" = read in source, not
reproduced.

| Issue | Where | Evidence |
|-------|-------|----------|
| A send callback can stay pending forever if its message or ack is lost while the destination stays reachable (no timeout). | `Node::sndMessage` | code |
| A publisher keeps sending to a subscriber that left if no broker saw it leave (for example, no broker was running at the time). | `Broker::stopPublishingTo` | code |
| If the broker's route to a publisher changes between a `StopPublish` and the next `StartPublish`, they can arrive reversed and the publisher stops sending to that subscriber. | `Broker::handleNewNodeStatus` | code |
| No authentication or encryption; any TCP client can join the mesh. | — | code |

## Handoff

Summarize what changed, why, and the exact checks run (build targets,
topologies, what the logs showed). Call out skipped checks, residual risks,
assumptions, and unrelated issues separately; do not present unverified
behavior as complete.
