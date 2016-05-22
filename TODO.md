# Todo-list

- Convert `assert` to `lairAssert`
- ~File system abstraction ?
- Renderer module
  - ~Tilemap
  - Move SpriteRenderer to the renderer module
  - Implement draw call sorting
    * Mostly done
    - Create a VertexArray class that combines VertexFormat and VertexBuffer ?
    - Find a wise way to assing indices to object (VertexArray, others ?) to
      avoid using hashes (and collisions that comes with them).
    - Take shader parameters into account to generate Indices ?
    - Add support for custom shader parameters (Move shaderParam list to
      RenderPass, takes unique_ptr).
    - Combine Texture and flags into an indexed class to save bits in Index.
  - Culling
  - Shape rendering ?
- ec
  - Insert entity
  - Only use DenseComponentManager and let EntityManager deal with the mapping ?
    - Access component by index. Indices < MAX_DENSE_COMPONENT are accessed directly,
      others are accessed through a hash map.
    - The current implementation is somewhat dangerous: adding sparse component
      _might_ trigger a realloc and change pointer values.
  - Support multiple renderPass (with a mask ? or a list of Renderer ?) for
    sprite / text.
  - Parameter to set the name of an entity on load.
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
- Geometric shapes ? (Collisions + rendering)
- UI elements
- Include third-party package and try to link statically ?


- Move to Python coding convention ? (Closer to stdc++ & same conventions for python and C)
