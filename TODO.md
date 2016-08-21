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
  - Support multiple renderPass (with a mask ? or a list of Renderer ?) for
    sprite / text.
  - Take a look at entities/component flags. Alive should be to tell an object
    has not be destroyed - it can not be resurected if false. Enable should
    allow to temporarily enable / disable an entity/component.
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
