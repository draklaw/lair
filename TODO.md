# Todo-list

- Convert `assert` to `lairAssert`
- ~File system abstraction ?
- Make a ressource system and add a default built-in texture / font used as a
  placeholder when some ressource is not found.
- ~Make the whole load / asset system more robust. Currently a missing asset or
  a file not found tend to crash (assert). It should be ignored when possible.
  If an asset is not loaded when it should be used (by rendering for instance),
  it should wait for it, I guess, and display an error message to tell the
  dev he should think to preload it.
- ~Find a way to avoid uploading texture during component rendering. If texture
  are accessed through TextureSet or similar, it should be easy ?
- Variant: merge Variant and ConstVarRef (and maybe VarRef) in a single class,
  then update Property to return this instead of a ConstVarRef. That way we
  don't need to store a temporary value in GenericPropertyValue.
- Renderer module
  - Move SpriteRenderer to the renderer module and rename it ?
  - Implement draw call sorting
    * Mostly done
    - Create a VertexArray class that combines VertexFormat and VertexBuffer ? YES
    - Find a wise way to assing indices to object (VertexArray, others ?) to
      avoid using hashes (and collisions that comes with them).
    - Take shader parameters into account to generate Indices ?
    - Add support for custom shader parameters (Move shaderParam list to
      RenderPass, takes unique_ptr).
    - Combine Texture and flags into an indexed class to save bits in Index.
      Use this class instead of Texture in Sprite component and others, remove
      the texture flags from them.
    - So: VertexArray (internal), Shader (vert+frag), TextureSet (Tex+flags*n), Sprite (TexSet+tiles)
  - Culling
  - Shape rendering ?
  - Write some shader building code. Might be useful to do tricky things, like
    dealing with various lighting models.
  - Multitexturing is required. How to deal with it at the component level ?
    Create a TextureSet class ?
- ec
  - Support multiple renderPass (with a mask ? or a list of Renderer ?) for
    sprite / text.
  - Make camera part of the scene graph to allow different cameras for different
    things (HUD, level, etc.)
  - Flag disabled entities to avoid walking the tree in components ? How to deal
    with hierarchical stuff (might be used by rendering) ? Idea: use an other
    graph / tree.
  - Scripting: how to do the binding, which language ?
  - Replace grid tilling with custom tilling. Use it for text rendering. Need
    to update the shader to support this.
  - Write helper methods to deal with Entities tranforms. Make sure they accept
    Eigen types.
  - What to do about Entity::extra ?
  - prevTransform update is now performed by a first step at the beginning of
    a tick, then the world map is updated. It simplify dealing with
    interpolating / non-interpolating transforms. Is this the right way to deal
    with it ?
  - How to deal with interpolation of other parameters (color...) ?
- Audio module (OpenAL ?)
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
- Write a json2ldl utility, then get rid of Json as a dependency.


- Move to Python coding convention ? (Closer to stdc++ & same conventions for python and C)
