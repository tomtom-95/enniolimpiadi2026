# Clay Hover Callback Mechanism

## Phase 1: Layout Declaration (when you write `Clay_OnHover(...)`)

When you write:
```c
CLAY(CLAY_IDI("HeaderButton", page), { ... }) {
    Clay_OnHover(HandleHeaderButtonInteraction, (intptr_t)page);
    ...
}
```

### What happens internally at `layout.c:361`:

1. `Clay_OnHover` is called **during** the element declaration (inside the `CLAY(...)` block)

2. At `clay.h:4296-4308`:
   ```c
   void Clay_OnHover(...) {
       Clay_LayoutElement *openLayoutElement = Clay__GetOpenLayoutElement();
       Clay_LayoutElementHashMapItem *hashMapItem = Clay__GetHashMapItem(openLayoutElement->id);
       hashMapItem->onHoverFunction = onHoverFunction;  // stores your callback
       hashMapItem->hoverFunctionUserData = userData;   // stores your page enum
   }
   ```

3. `Clay__GetOpenLayoutElement()` (`clay.h:1319-1322`) returns the **currently open element** from a stack:
   ```c
   Clay_LayoutElement* Clay__GetOpenLayoutElement(void) {
       return Clay_LayoutElementArray_Get(&context->layoutElements,
           Clay__int32_tArray_GetValue(&context->openLayoutElementStack,
               context->openLayoutElementStack.length - 1));
   }
   ```

The callback and userData are stored in the **hash map item** (`Clay_LayoutElementHashMapItem` at `clay.h:1178-1187`):
```c
typedef struct {
    Clay_BoundingBox boundingBox;
    Clay_ElementId elementId;
    Clay_LayoutElement* layoutElement;
    void (*onHoverFunction)(...);     // your callback
    intptr_t hoverFunctionUserData;   // your userData
    ...
} Clay_LayoutElementHashMapItem;
```

## Phase 2: Pointer State Update (when callbacks are invoked)

The callbacks are **NOT** called during layout. They're called during `Clay_SetPointerState()` (`clay.h:3969-4038`), which you typically call each frame with the mouse position:

```c
void Clay_SetPointerState(Clay_Vector2 position, bool isPointerDown) {
    context->pointerOverIds.length = 0;  // reset hover list

    // DFS traversal of ALL layout elements (lines 3978-4023)
    for (rootIndex...) {
        while (dfsBuffer.length > 0) {
            Clay_LayoutElement *currentElement = ...;
            Clay_LayoutElementHashMapItem *mapItem = Clay__GetHashMapItem(currentElement->id);

            if (mapItem) {
                Clay_BoundingBox elementBox = mapItem->boundingBox;

                // Check if pointer is inside this element's bounding box
                if (Clay__PointIsInsideRect(position, elementBox) && /* clip check */) {
                    // >>> CALLBACK IS INVOKED HERE! <<<
                    if (mapItem->onHoverFunction) {
                        mapItem->onHoverFunction(
                            mapItem->elementId,
                            context->pointerInfo,        // contains position + state
                            mapItem->hoverFunctionUserData  // your (intptr_t)page
                        );
                    }
                    Clay_ElementIdArray_Add(&context->pointerOverIds, mapItem->elementId);
                }
            }
        }
    }

    // Update pointer state machine (lines 4025-4037)
    if (isPointerDown) {
        // PRESSED_THIS_FRAME -> PRESSED -> ...
    } else {
        // RELEASED_THIS_FRAME -> RELEASED -> ...
    }
}
```

## The Pointer State Machine

The `Clay_PointerData.state` (`clay.h:738-747`) tracks:

| State | Meaning |
|-------|---------|
| `CLAY_POINTER_DATA_PRESSED_THIS_FRAME` | Click/touch just started this frame |
| `CLAY_POINTER_DATA_PRESSED` | Button held down (not first frame) |
| `CLAY_POINTER_DATA_RELEASED_THIS_FRAME` | Button just released this frame |
| `CLAY_POINTER_DATA_RELEASED` | Button is up (idle) |

## Complete Flow Summary

```
Frame N:
1. Clay_SetPointerState(mousePos, IsMouseButtonDown())
   └─> Traverses ALL elements via DFS
   └─> For each element: if pointer inside bounding box
       └─> Call onHoverFunction(elementId, pointerData, userData)

2. Clay_BeginLayout()

3. Your CLAY() declarations with Clay_OnHover()
   └─> Stores callback + userData in hash map (for NEXT frame)

4. Clay_EndLayout()
   └─> Calculates bounding boxes

5. Render...
```

## Key Insight

The callbacks are called **before** your layout code runs each frame. The callback registration you do in `CLAY()` blocks prepares the callbacks for the **next** `Clay_SetPointerState()` call. This is why on the first frame an element appears, its hover callback won't fire until the second frame.
