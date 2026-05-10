#!/usr/bin/env python3
"""Generate {◆} breathing GIF for alchemy README."""
from PIL import Image, ImageDraw, ImageFont
import math, os

# ── Config ──
W, H = 1600, 100
BG = (10, 10, 20)
FONT_REG  = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
FONT_BOLD = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
FONT_SIZE = 40
BASE_R, BASE_G, BASE_B = 255, 204, 61
BRACKET_BRIGHT = 0.75
BRIGHT_MIN = 0.15
BRIGHT_MAX = 1.00
CYCLE_MS = 1500
FPS = 20
STEP_MS = 1000 // FPS  # 50ms
CYCLE_STEPS = (FPS * CYCLE_MS) // 1000  # 30 frames
LOOPS = 3  # 3 full cycles = ~4.5s
TOTAL_FRAMES = CYCLE_STEPS * LOOPS

font       = ImageFont.truetype(FONT_REG,  FONT_SIZE)
font_bold  = ImageFont.truetype(FONT_BOLD, FONT_SIZE)

# Layout
TEMPLATE = "Finding the optimal skill"
TEXT_BASE = TEMPLATE  # no leading space
DOT_PATTERNS = [".", "..", "..."]
DOTS_INTERVAL = 8  # frames per dot state (8 * 50ms = 400ms)

# Measure diamond (regular font)
diamond_bbox = font.getbbox("◆")
diamond_w = diamond_bbox[2] - diamond_bbox[0]

# Bracket widths (BOLD font)
bracket_open_bbox  = font_bold.getbbox("{")
bracket_open_w  = bracket_open_bbox[2] - bracket_open_bbox[0]
bracket_close_bbox = font_bold.getbbox("}")
bracket_close_w = bracket_close_bbox[2] - bracket_close_bbox[0]

# Logo total width: {◆}
logo_total_w = bracket_open_w + diamond_w + bracket_close_w

# Measure text height
base_full = font.getbbox(TEXT_BASE + "...")
base_h = base_full[3] - base_full[1]

# Vertical positions
y_text = (H - base_h) // 2 + 2     # text baseline
y_logo = y_text - 2                 # logo 2px above text

# Fixed horizontal center
GAP = 8  # px between logo and text
full_bbox = font.getbbox(TEXT_BASE + "...")
full_w = full_bbox[2] - full_bbox[0]
x0_fixed = (W - (logo_total_w + GAP + full_w)) // 2

text_base_bbox = font.getbbox(TEXT_BASE)
text_base_w = text_base_bbox[2] - text_base_bbox[0]

frames = []

for frame_idx in range(TOTAL_FRAMES):
    img = Image.new("RGB", (W, H), BG)
    draw = ImageDraw.Draw(img)

    # Breathing calculation
    t = (frame_idx % CYCLE_STEPS) / CYCLE_STEPS * 2 * math.pi
    s = math.sin(t)
    bright = BRIGHT_MIN + (BRIGHT_MAX - BRIGHT_MIN) * (s + 1) / 2

    # Colors
    br_r = int(BASE_R * BRACKET_BRIGHT)
    br_g = int(BASE_G * BRACKET_BRIGHT)
    br_b = int(BASE_B * BRACKET_BRIGHT)
    d_r = int(BASE_R * bright)
    d_g = int(BASE_G * bright)
    d_b = int(BASE_B * bright)

    # Dots cycling
    dot_idx = (frame_idx // DOTS_INTERVAL) % len(DOT_PATTERNS)
    dots = DOT_PATTERNS[dot_idx]

    # Fixed position — only dots change
    x = x0_fixed

    # Draw "{" (BOLD)
    draw.text((x, y_logo), "{", font=font_bold, fill=(br_r, br_g, br_b))
    x += bracket_open_w

    # Draw "◆" (regular)
    draw.text((x, y_logo), "◆", font=font, fill=(d_r, d_g, d_b))
    x += diamond_w

    # Draw "}" (BOLD)
    draw.text((x, y_logo), "}", font=font_bold, fill=(br_r, br_g, br_b))
    x += bracket_close_w + GAP  # gap between logo and text

    # Draw fixed text (same every frame, at y_text)
    draw.text((x, y_text), TEXT_BASE, font=font, fill=(220, 220, 220))
    # Draw cycling dots at the end
    dot_x = x + text_base_w
    draw.text((dot_x, y_text), dots, font=font, fill=(220, 220, 220))

    frames.append(img)

# Save as GIF
OUT = os.path.expanduser("~/alchemy/docs/screenshots/alchemy-breath.gif")
os.makedirs(os.path.dirname(OUT), exist_ok=True)
frames[0].save(
    OUT,
    save_all=True,
    append_images=frames[1:],
    duration=STEP_MS,
    loop=0,
    optimize=True,
)
print(f"GIF saved: {OUT}  ({len(frames)} frames, {STEP_MS}ms/frame)")
print(f"Duration: {len(frames) * STEP_MS / 1000:.1f}s")
print(f"Size: {os.path.getsize(OUT) / 1024:.1f} KB")
