# Todo-list

- Convert `assert` to `lairAssert`
- ~File system abstraction ?
- Renderer module
  - ~Tilemap
  - Use big buffers per batch, fill them after components sort ?
  - Write a class that combines Shader + uniforms values ?
  - Culling
  - Shape rendering ?
- ec
  - Insert entity
  - Only use DenseComponentManager and let EntityManager deal with the mapping ?
    - Access component by index. Indices < MAX_DENSE_COMPONENT are accessed directly,
      others are accessed through a hash map.
    - The current implementation is somewhat dangerous: adding sparse component
      _might_ trigger a realloc and change pointer values.
- Audio module (OpenAL ?)
- ec meta-code (Property system / method call) (python ?)
- Python
  - lair binding
  - Re-implement test-window
  - ~Investigate packaging
- Assets / Loader
  - Auto-remove cached objects when max size is exceded
  - Asset group for easy asset management / progress monitoring
  - Track files for auto-reload / Reload all function
- Support mouse / gamepads
- Collision module (Based on Box2D ? Bullet ?)
- Geometric shapes ?
- UI elements
- Include third-party package and try to link statically ?


- Move to Python coding convention ? (Closer to stdc++ & same conventions for python and C)
