# InteractCore
A minimal Unreal Engine plugin for implement interaction system in your game with FPS,TPS,Isometric presset game  (network-ready).
| MaxHits    | Best approach                                 |
| ---------- | --------------------------------------------- |
| **≤ 16**   | Nested loops (what you have now)              |
| **16–64**  | Linear + small lookup (sorted / cached array) |
| **64–256** | `TSet` / `TMap`                               |
| **256+**   | Persistent sets + delta tracking              |
If MaxHits is, say, 32:

32 × 32 = 1024 comparisons per tick

At 60 FPS → 61,440 comparisons per second

Plus pointer chasing (GetActor()), branches, cache misses

That’s where your perf goes.