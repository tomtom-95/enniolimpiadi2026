# Clay UI: Complete Guide to Clipping and Scrolling

This guide explains how Clay's clipping and scrolling mechanisms work internally, step by step.

## Table of Contents

1. [The Simplest Example](#the-simplest-example)
2. [Overview](#overview)
3. [The Clip Configuration](#the-clip-configuration)
3. [How Clipping Works Internally](#how-clipping-works-internally)
4. [How Scrolling Works](#how-scrolling-works)
5. [Element Culling (Offscreen Detection)](#element-culling-offscreen-detection)
6. [Floating Elements and Clipping](#floating-elements-and-clipping)
7. [Common Pitfalls](#common-pitfalls)
8. [Practical Examples](#practical-examples)

---

## The Simplest Example

Before diving into details, let's understand clipping and childOffset with the simplest possible example.

### Setup: A Container with Content Larger Than It

```c
// A container that is 200x100 pixels
// But contains a child that is 400x100 pixels (twice as wide!)

CLAY(CLAY_ID("Container"), {
    .layout = { .sizing = { CLAY_SIZING_FIXED(200), CLAY_SIZING_FIXED(100) } },
    .backgroundColor = GRAY,
    .clip = {
        .horizontal = true,
        .childOffset = { 0, 0 }  // We'll change this value
    }
}) {
    CLAY(CLAY_ID("Content"), {
        .layout = { .sizing = { CLAY_SIZING_FIXED(400), CLAY_SIZING_FIXED(100) } },
        .backgroundColor = BLUE
    }) {
        // Imagine this contains: [A] [B] [C] [D] (four 100px boxes)
    }
}
```

### What Each Part Does

**1. The Container size (200x100)** = The "window" you look through

**2. The Content size (400x100)** = The actual content (larger than the window)

**3. `.clip.horizontal = true`** = "Cut off anything outside the window"

**4. `.childOffset`** = "Move the content left/right behind the window"

### Visual: childOffset = { 0, 0 }

```
Container position on screen: x=100, y=100
Content is at its natural position (no offset)

Screen:
         100       200       300       400       500
    ┌─────────────────────────────────────────────────
100 │     ┌─────────────────────┐
    │     │ [A]  [B]  │ [C]  [D] │   ← Content (400px wide)
    │     │  VISIBLE  │  HIDDEN  │
200 │     └───────────┴──────────┘
    │           ↑
    │     Container (200px)
    │     with clip=true
    │
    │     You see: [A] [B]
    │     [C] [D] are clipped (cut off)
```

### Visual: childOffset = { -100, 0 }

Now we set `childOffset = { -100, 0 }`. This means: **move all children 100 pixels to the LEFT**.

```
Container position on screen: x=100, y=100
Content is shifted LEFT by 100 pixels

Screen:
         0         100       200       300       400
    ┌─────────────────────────────────────────────────
100 │ [A] │     ┌───────────────────┐
    │     │     │ [B]  [C]  │ [D]   │   ← Content shifted left
    │HIDDEN     │  VISIBLE  │HIDDEN │
200 │     │     └───────────┴───────┘
    │                 ↑
    │           Container (200px)
    │
    │     You see: [B] [C]
    │     [A] is off the left edge (clipped)
    │     [D] is off the right edge (clipped)
```

### Visual: childOffset = { -200, 0 }

Now `childOffset = { -200, 0 }`:

```
Container position on screen: x=100, y=100
Content is shifted LEFT by 200 pixels

Screen:
    -100  0         100       200       300       400
    ┌─────────────────────────────────────────────────
100 │[A] [B]│    ┌───────────────────┐
    │       │    │ [C]  [D]  │       │
    │ HIDDEN│    │  VISIBLE  │       │
200 │       │    └───────────┘       │
    │                 ↑
    │           Container (200px)
    │
    │     You see: [C] [D]
```

### The Key Insight

**childOffset moves the CONTENT, not the container.**

Think of it like a camera looking through a window:
- The **container** = the window frame (fixed position on screen)
- The **content** = a large painting behind the window
- The **childOffset** = sliding the painting left/right/up/down
- The **clip** = the window frame cuts off what's outside

```
childOffset.x = 0      childOffset.x = -100    childOffset.x = -200
                       (scroll right)          (scroll more right)

┌──────┐               ┌──────┐                ┌──────┐
│[A][B]│[C][D]         [A]│[B][C]│[D]          [A][B]│[C][D]│
└──────┘               └──────┘                └──────┘
   ↑                      ↑                       ↑
 Window                 Window                  Window
```

### Why Negative Values?

- `childOffset = { -100, 0 }` means "move content 100px LEFT"
- This shows content that was originally to the RIGHT
- It's like scrolling RIGHT in a web browser

Think: **the offset is applied to the content's position**.
- Content normally at x=100 with childOffset.x=-100 → drawn at x=0
- Negative X = content moves left = you see content from the right side

---

## Overview

Clay provides a unified system for clipping and scrolling through the `.clip` configuration. This system:

1. **Clips** content that overflows the element's bounds (using GPU scissor rectangles)
2. **Scrolls** content by offsetting child positions
3. **Culls** elements that are completely outside the visible screen area (performance optimization)

Understanding how these three mechanisms interact is crucial for building scrollable UIs.

---

## The Clip Configuration

The clip configuration is defined as:

```c
typedef struct Clay_ClipElementConfig {
    bool horizontal;           // Clip overflowing elements on the X axis
    bool vertical;             // Clip overflowing elements on the Y axis
    Clay_Vector2 childOffset;  // Offsets x,y positions of all children (for scrolling)
} Clay_ClipElementConfig;
```

### Basic Usage

```c
CLAY(CLAY_ID("ScrollContainer"), {
    .layout = { .sizing = { CLAY_SIZING_FIXED(400), CLAY_SIZING_FIXED(300) } },
    .clip = {
        .horizontal = true,      // Enable horizontal clipping
        .vertical = true,        // Enable vertical clipping
        .childOffset = { -50, -100 }  // Scroll position (negative = content moved up/left)
    }
}) {
    // Children here will be clipped if they overflow
    // Their positions will be offset by childOffset
}
```

### Using Clay's Built-in Scroll System

```c
CLAY(CLAY_ID("ScrollContainer"), {
    .layout = { .sizing = layoutExpand },
    .clip = {
        .horizontal = true,
        .vertical = true,
        .childOffset = Clay_GetScrollOffset()  // Get scroll offset for THIS element
    }
}) {
    // Content that may be larger than the container
}
```

**Important**: `Clay_GetScrollOffset()` returns the scroll offset for the **currently open element** (the element being declared). It must be called during the element declaration, not before.

---

## How Clipping Works Internally

### Step 1: Layout Phase

During layout, Clay calculates the dimensions of all elements. When a parent has `.clip` enabled:

- Children are **not compressed** to fit the parent's size
- Children can have dimensions **larger** than the parent
- The parent's `contentSize` tracks the total size of all children

```
Parent (400x300, clip=true)
└── Child (800x600)  ← Allowed to be larger than parent
```

### Step 2: Render Command Generation

When Clay generates render commands, it processes the clip configuration:

1. **SCISSOR_START Command**: Generated when entering a clip element
   ```c
   // Clay generates this render command:
   {
       .commandType = CLAY_RENDER_COMMAND_TYPE_SCISSOR_START,
       .boundingBox = { x, y, width, height }  // The clip element's bounds
   }
   ```

2. **Child Render Commands**: All children are rendered with their positions offset by `childOffset`

3. **SCISSOR_END Command**: Generated when exiting the clip element
   ```c
   {
       .commandType = CLAY_RENDER_COMMAND_TYPE_SCISSOR_END
   }
   ```

### Step 3: Renderer Implementation

Your renderer must handle these commands:

```c
case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
    // Enable GPU scissor test - only pixels within this rectangle will be drawn
    BeginScissorMode(
        (int)boundingBox.x,
        (int)boundingBox.y,
        (int)boundingBox.width,
        (int)boundingBox.height
    );
    break;
}

case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
    // Disable scissor test - return to normal rendering
    EndScissorMode();
    break;
}
```

### The Scissor Stack

**Critical**: Scissor modes in most graphics APIs **do not stack**. Each `BeginScissorMode` replaces the previous scissor rectangle. This means:

```
SCISSOR_START (rect A)    ← Scissor = A
  SCISSOR_START (rect B)  ← Scissor = B (NOT intersection of A and B!)
  SCISSOR_END             ← Scissor = NONE (not back to A!)
SCISSOR_END               ← Scissor = NONE
```

If you need nested clipping, your renderer must manually track and intersect scissor rectangles.

---

## How Scrolling Works

### The childOffset Mechanism

`childOffset` is applied to **all direct children** of the clip element during render command generation:

```c
// In Clay__CalculateFinalLayout (simplified):
Clay_Vector2 childPosition = {
    parentPosition.x + nextChildOffset.x + scrollOffset.x,  // scrollOffset from childOffset
    parentPosition.y + nextChildOffset.y + scrollOffset.y,
};
```

### Scroll Direction Convention

- **Negative** `childOffset.y` = Content scrolled **up** (viewing content below)
- **Positive** `childOffset.y` = Content scrolled **down** (viewing content above)
- **Negative** `childOffset.x` = Content scrolled **left** (viewing content to the right)
- **Positive** `childOffset.x` = Content scrolled **right** (viewing content to the left)

```
childOffset = (0, 0)         childOffset = (0, -100)
┌─────────────┐              ┌─────────────┐
│ Line 1      │              │ Line 2      │  ← Line 1 scrolled out of view
│ Line 2      │      →       │ Line 3      │
│ Line 3      │              │ Line 4      │
└─────────────┘              └─────────────┘
```

### Using Clay's Built-in Scroll Handling

Clay provides automatic scroll handling via `Clay_UpdateScrollContainers()`:

```c
// In your main loop:
Clay_UpdateScrollContainers(
    enableDragScrolling,  // true for touch/drag scrolling
    scrollDelta,          // Mouse wheel delta (usually from input)
    deltaTime             // Time since last frame
);
```

This function:
1. Tracks scroll state for each clip element
2. Handles mouse wheel scrolling
3. Handles touch/drag scrolling with momentum
4. Clamps scroll position to valid range

To use it, declare your clip element with `Clay_GetScrollOffset()`:

```c
CLAY(CLAY_ID("MyScrollArea"), {
    .clip = {
        .vertical = true,
        .childOffset = Clay_GetScrollOffset()  // Clay manages this value
    }
}) {
    // Scrollable content
}
```

### Manual Scroll Handling

For custom scroll behavior (like panning with zoom), manage the offset yourself:

```c
// In your app state:
typedef struct {
    float scrollX;
    float scrollY;
    float zoom;
} ChartState;

// In your layout:
CLAY(CLAY_ID("Chart"), {
    .clip = {
        .horizontal = true,
        .vertical = true,
        .childOffset = { state.scrollX, state.scrollY }  // Your custom values
    }
}) {
    // Pannable/zoomable content
}

// In your input handling:
if (mouse_dragging) {
    state.scrollX += mouse_delta.x;
    state.scrollY += mouse_delta.y;
}
```

---

## Element Culling (Offscreen Detection)

### How Culling Works

Clay performs **visibility culling** to optimize rendering. Elements completely outside the screen don't get render commands generated.

```c
bool Clay__ElementIsOffscreen(Clay_BoundingBox *boundingBox) {
    if (context->disableCulling) {
        return false;
    }

    return (boundingBox->x > layoutDimensions.width) ||   // Left edge past right screen edge
           (boundingBox->y > layoutDimensions.height) ||  // Top edge past bottom screen edge
           (boundingBox->x + boundingBox->width < 0) ||   // Right edge before left screen edge
           (boundingBox->y + boundingBox->height < 0);    // Bottom edge before top screen edge
}
```

### Critical Understanding: Screen Coordinates vs Parent Coordinates

**Culling is based on SCREEN coordinates, not parent coordinates!**

This means:
- An element can be inside its parent's bounds
- But if its **screen position** (after applying all scroll offsets) is outside the screen, it's culled

```
Screen (1920x1080)
┌──────────────────────────────────────┐
│                                      │
│   ClipContainer (at 100,100)         │
│   ┌────────────────────┐             │
│   │                    │             │
│   │  Element A         │ ← Visible, gets render command
│   │  (screen: 150,150) │             │
│   │                    │             │
│   └────────────────────┘             │
│                                      │
└──────────────────────────────────────┘

After scrolling childOffset = (-2000, 0):

Screen (1920x1080)
┌──────────────────────────────────────┐
│                                      │
│   ClipContainer (at 100,100)         │
│   ┌────────────────────┐             │
│   │                    │             │
│   │  (empty - content  │             │
│   │   scrolled away)   │             │
│   │                    │             │
│   └────────────────────┘             │
│                                      │
└──────────────────────────────────────┘

Element A now at screen position (-1850, 150)
→ boundingBox.x + boundingBox.width < 0
→ CULLED! No render command generated!
```

### The Problem with Small/Zero-Size Elements

If you have a custom element with size 0x0 or very small:

```c
CLAY(CLAY_ID("CustomDrawer"), {
    .layout = { .sizing = { CLAY_SIZING_FIXED(0), CLAY_SIZING_FIXED(0) } },
    .custom = { .customData = &myData }
}) {}
```

When scroll offset moves this element's screen position outside the viewport, it gets culled and your custom render code **never runs**.

### Solutions for Custom Elements

**Option 1: Give it a size that keeps it on-screen**

```c
CLAY(CLAY_ID("CustomDrawer"), {
    .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },  // Fill parent
    .custom = { .customData = &myData }
}) {}
```

**Option 2: Use a floating element**

Floating elements are processed separately and their position isn't affected by parent's `childOffset`:

```c
CLAY(CLAY_ID("CustomDrawer"), {
    .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
    .floating = {
        .attachTo = CLAY_ATTACH_TO_PARENT,
        .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_PASSTHROUGH
    },
    .custom = { .customData = &myData }
}) {}
```

**Option 3: Disable culling globally** (not recommended for performance)

```c
Clay_SetCullingEnabled(false);
```

---

## Floating Elements and Clipping

### Floating Elements Don't Scroll

Floating elements attached to a parent are positioned relative to the parent's **original position**, not affected by `childOffset`:

```c
CLAY(CLAY_ID("ScrollContainer"), {
    .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
}) {
    // This scrolls with content
    CLAY(CLAY_ID("ScrollingChild"), { ... }) {}

    // This stays fixed relative to ScrollContainer
    CLAY(CLAY_ID("FloatingOverlay"), {
        .floating = { .attachTo = CLAY_ATTACH_TO_PARENT }
    }) {}
}
```

### Floating Element Clipping Options

Control whether floating elements inherit their parent's clipping:

```c
CLAY(CLAY_ID("FloatingMenu"), {
    .floating = {
        .attachTo = CLAY_ATTACH_TO_PARENT,
        .clipTo = CLAY_CLIP_TO_NONE  // DEFAULT: Float freely, ignore parent's clip
        // or
        .clipTo = CLAY_CLIP_TO_ATTACHED_PARENT  // Clip to parent's scissor rectangle
    }
}) {}
```

---

## Common Pitfalls

### 1. Calling BeginScissorMode in Custom Renderers

**Problem**: Your custom render code calls `BeginScissorMode`/`EndScissorMode`, which cancels the parent's scissor.

```c
// BAD: This cancels the parent element's scissor!
case CUSTOM_TYPE:
    BeginScissorMode(...);  // Replaces parent's scissor
    // Draw stuff
    EndScissorMode();       // Now NO scissor is active!
    break;
```

**Solution**: Don't use scissor in custom renderers if the parent already clips, or carefully manage the scissor state.

### 2. Zero-Size Custom Elements Getting Culled

**Problem**: Custom element with 0x0 size disappears when scrolling.

**Solution**: Use `SIZING_GROW` or make it a floating element (see above).

### 3. Content Not Clipping

**Problem**: Children render outside the clip container.

**Possible causes**:
- Scissor was ended prematurely (see pitfall #1)
- Renderer doesn't handle `SCISSOR_START`/`SCISSOR_END` commands
- Children are floating elements with `CLAY_CLIP_TO_NONE`

### 4. Scroll Not Working

**Problem**: `Clay_GetScrollOffset()` returns zero or content doesn't scroll.

**Checklist**:
- Are you calling `Clay_UpdateScrollContainers()` each frame?
- Is the clip element's ID consistent across frames?
- Does the content actually overflow the container?
- Is `enableDragScrolling` or `scrollDelta` being passed correctly?

### 5. Children Compressed Instead of Clipped

**Problem**: Children shrink to fit the parent instead of overflowing and being clipped.

**Solution**: Make sure `.clip.horizontal` and/or `.clip.vertical` are set to `true`. When these are set, Clay allows children to exceed the parent's size.

---

## Practical Examples

### Example 1: Simple Scroll Container

```c
// Basic vertical scrolling list
CLAY(CLAY_ID("ScrollableList"), {
    .layout = {
        .sizing = { CLAY_SIZING_FIXED(300), CLAY_SIZING_FIXED(400) },
        .layoutDirection = CLAY_TOP_TO_BOTTOM,
        .childGap = 8
    },
    .backgroundColor = { 40, 40, 40, 255 },
    .clip = {
        .vertical = true,
        .childOffset = Clay_GetScrollOffset()
    }
}) {
    for (int i = 0; i < 100; i++) {
        CLAY(CLAY_IDI("ListItem", i), {
            .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(50) } },
            .backgroundColor = { 60, 60, 60, 255 }
        }) {
            // Item content
        }
    }
}

// In main loop:
Clay_UpdateScrollContainers(true, GetMouseWheelMoveV(), GetFrameTime());
```

### Example 2: Pannable Canvas with Custom Drawing

```c
// State
static float panX = 0, panY = 0;

// Handle input
if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
    Vector2 delta = GetMouseDelta();
    panX += delta.x;
    panY += delta.y;
}

// Layout
CLAY(CLAY_ID("Canvas"), {
    .layout = { .sizing = layoutExpand },
    .backgroundColor = { 30, 30, 30, 255 },
    .clip = {
        .horizontal = true,
        .vertical = true,
        .childOffset = { panX, panY }
    }
}) {
    // Custom overlay that doesn't scroll (for drawing connections, etc.)
    // Using floating so it's not affected by childOffset
    CLAY(CLAY_ID("ConnectionsOverlay"), {
        .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
        .floating = {
            .attachTo = CLAY_ATTACH_TO_PARENT,
            .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_PASSTHROUGH
        },
        .custom = { .customData = &connectionsData }
    }) {}

    // Content that scrolls
    RenderCanvasContent();
}
```

### Example 3: Nested Scroll Containers

```c
CLAY(CLAY_ID("OuterScroll"), {
    .layout = { .sizing = { CLAY_SIZING_FIXED(600), CLAY_SIZING_FIXED(400) } },
    .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
}) {
    // This content scrolls vertically
    for (int section = 0; section < 10; section++) {
        CLAY(CLAY_IDI("Section", section), {
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(200) },
                .layoutDirection = CLAY_LEFT_TO_RIGHT
            }
        }) {
            // Horizontal scroll within each section
            CLAY(CLAY_IDI("HorizontalScroll", section), {
                .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) } },
                .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
            }) {
                // Horizontally scrolling content
                for (int item = 0; item < 20; item++) {
                    CLAY(CLAY_IDI_I("Item", section, item), {
                        .layout = { .sizing = { CLAY_SIZING_FIXED(150), CLAY_SIZING_GROW(0) } },
                        .backgroundColor = { 60, 60, 80, 255 }
                    }) {}
                }
            }
        }
    }
}
```

---

## Summary

| Concept | Key Points |
|---------|------------|
| **Clipping** | Uses GPU scissor; enabled via `.clip.horizontal`/`.clip.vertical` |
| **Scrolling** | Uses `.clip.childOffset` to offset all children's positions |
| **Culling** | Elements with screen coords fully outside viewport are skipped |
| **Floating** | Not affected by parent's `childOffset`; can opt into parent's clip |
| **Scissor Stack** | Does NOT automatically stack - each scissor replaces the previous |

### The Data Flow

```
1. Layout Phase
   └── Calculate element dimensions (children can exceed parent if clip enabled)

2. Render Command Generation
   └── For each element:
       ├── Apply scrollOffset to child positions
       ├── Check if offscreen (CULL if yes)
       ├── Generate SCISSOR_START if clip element
       ├── Generate element render commands
       └── Generate SCISSOR_END when leaving clip element

3. Renderer
   └── Process render commands:
       ├── SCISSOR_START → BeginScissorMode()
       ├── Draw elements (clipped by GPU)
       └── SCISSOR_END → EndScissorMode()
```
