# Todo-list

- Convert `assert` to `lairAssert`
- ~File system abstraction ?
- Make a ressource system and add a default built-in texture / font used as a
  placeholder when some ressource is not found.
- Renderer module
  - ~Tilemap
  - Move SpriteRenderer to the renderer module and rename it ?
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
  - Make camera part of the scene graph to allow different cameras for different
    things (HUD, level, etc.)
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
