v1.0.0-rc6 key lock patch

Purpose:
- Fix remaining key repeated-trigger issue caused by mechanical key bounce.
- One physical KEY ADD press changes brightness by exactly one level.
- One physical KEY SUB press changes brightness by exactly one level.
- Holding a key does not continuously adjust brightness.
- A new key event is allowed only after the key is released stably for 80ms.

Replace files:
firmware/bsp/bsp_key.c
firmware/app/app_version.h

Other rc5 files remain unchanged.
