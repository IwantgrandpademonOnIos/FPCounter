# v1.1.0

* Fixed detection using `m_collisionBlocks`, which is a niche editor trigger feature, not hazard geometry. Now checks against `m_hazardCollisionObjects` and `m_solidCollisionObjects`, the actual objects that can kill or block the player.
* Fixed the tick rate being hardcoded to 240. Detection now reads the real per-tick delta from `GJBaseGameLayer::getModifiedDelta` every tick, so it works correctly with macros/bots and TPS-changing mods that don't run at 240.
* Redesigned the Scan popup: panel backgrounds, a column header, alternating row stripes, and rows colored by difficulty tier (white/orange/red for 60/120/240fps).
* Replaced the plain text menu/pause buttons with a proper circular icon button matching the game's own UI.
* Added `changelog.md`.

# v1.0.0

* Initial release: live frame-perfect detection with sound, screen flash, and an on-screen counter.
* Scan popup for reviewing every frame perfect from your last recorded clear of a level, with required FPS per input.
