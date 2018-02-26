- build:rm-json: Get rid of Json
- build:rm-boost: Get rid of Boost
- build:rm-glew: Get rid of GLEW
- build:standalone: Inculde third-party packages and build statically
- build:wasm: Test with wasm [req: build:standalone]

- arch:split-core: Split core in several modules (assets, ldl, etc)
- arch:ldl-lib: Make LDL a standalone library ?
- arch:meta-lib: Make meta-type system a standalone library ?

- sys:mouse: Better mouse support
- sys:gamepad: Support gamepads

- assets:simple-loader: Create a simple, single-threaded loader
- assets:file-sys: File system abstraction
- assets:resource: Resource system [req: assets:file-sys]
- assets:default-resources: Built-in default texture, font, etc. [req: assets:resource]
- assets:cleanup: Remove cached objects when possible
- assets:group: Create assets groups to automatically follow load progress / release assets
- assets:autoreload: Track file changes and auto-reload assets

- render:shader-include: support #include in shaders [req: assets:resource]
- render:es3: Upgrade to GL ES3
- render:fbo: Support FBO
- render:tex-filter: Texture filtering function [req: render:shader-includen]
- render:tex-pack: Automatic texture packing (offline ?)
- render:group-states: Grouped states (tex-set+filters / VBO+VAO, etc.)
- render:shapes: Shape drawing (stroke + filled)
- render:text: Text shaders (sdf / vector)

- entities:render-pass: Support render passes + render parameters
- entities:camera-sg: Include camera in the scene-graph
- entities:prop-array: Allow components property to live in a separate array for performances and ping-pong for interpolation
- entities:bake-disable: Propagate disabled state to avoid walking the tree to know if an entity is enabled
- entities:tile-map: Use generic tile maps instead of grid-based tiling
- entities:tranforms-helper: Write transforms methods for entities for convenience

- coll:enhance: Finish & test basic shape collisions

- script:proto: Prototype a simple scripting language ?

- audio:rewrite: Better audio support (OpenAL ?)

- ui:proto: Prototype a UI framework
