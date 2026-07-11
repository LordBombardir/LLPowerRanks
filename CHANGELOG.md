# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [26.20.1] - 2026-07-11

### Fixed
- Fixed compilation error for third-party developers including `<power_ranks/Api.h>` (relocated include path `../types/Rank.h` to `"types/Rank.h"`).
- Cleaned up include structure across the entire codebase by removing relative parent directories (`../` and `../../`) where possible.

## [26.20.0] - 2026-07-11

### Changed
- Updated project to support LeviLamina `26.20.0`, PlaceholderAPI `26.20.1`, and PlayerDB `26.20.1`.
- Adapted database and manager code to use PlayerDB's `addUnknownPlayerEntry` API instead of the deprecated `addTemporaryPlayerEntry`.
- Restored `setScoreTag` support.
- Refactored compiler flags, exceptions, and diagnostics to resolve conflicts and improve warnings.
