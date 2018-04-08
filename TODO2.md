- build:rm-json: Get rid of Json
- build:standalone: Inculde third-party packages and build statically
- build:wasm: Test with wasm [req: build:standalone]

- arch:split-core: Split core in several modules (assets, ldl, etc)
- arch:ldl-lib: Make LDL a standalone library ?
- arch:meta-lib: Make meta-type system a standalone library ?

- log:newlines: Indent new lines and replace tabs by spaces.

- sys:mouse: Better mouse support
- sys:gamepad: Support gamepads

- assets:rework: Rethink asset system _or_ document it
- assets:simple-loader: Create a simple, single-threaded loader
- assets:default-resources: Built-in default texture, font, etc.
- assets:cleanup: Remove cached objects when possible
- assets:group: Create assets groups to automatically follow load progress / release assets
- assets:autoreload: Track file changes and auto-reload assets
- assets:race-condition: Fix race-condition when accessing loader callbacks.

- render:shader-include: support #include in shaders [req: assets:resource]
- render:fbo: Support FBO
- render:tex-filter: Texture filtering function [req: render:shader-include]
- render:tex-atlas: Allow one GL texture to contain several "logical" texture (batching)
- render:tex-pack: Automatic texture packing (offline ?) [req: render:tex-atlas]
- render:uniform-buffer: Improve uniform args system + use uniform buffers.
- render:shapes: Shape drawing (stroke + filled)
- render:text: Text shaders (sdf / vector)

- entities:render-pass: Support render passes + render parameters
- entities:camera-sg: Include camera in the scene-graph
- entities:prop-array: Allow components property to live in a separate array for performances and ping-pong for interpolation
- entities:bake-disable: Propagate disabled state to avoid walking the tree to know if an entity is enabled
- entities:tile-map-culling: Update render so it uses culling instead of static buffers
- entities:tranforms-helper: Write transforms methods for entities for convenience

- coll:enhance: Finish & test basic shape collisions

- script:proto: Prototype a simple scripting language ?

- audio:rewrite: Better audio support (OpenAL ?)

- ui:proto: Prototype a UI framework
