-- load all shaders
shader_entries = {
    {"id", {"shaders/id.vert", "shaders/id.frag"}},
    {"shadow", {"shaders/shadow.vert", "shaders/shadow.frag"}},
    {"board", {"shaders/default.vert", "shaders/checker.frag"}},
    {"piece", {"shaders/default.vert", "shaders/piece.frag"}},
    {"passthrough", {"shaders/passthrough.vert", "shaders/passthrough.frag"}}
}

for i,v in ipairs(shader_entries) do
    load_shader(v[1],v[2][1],v[2][2])
end

-- load all meshes
mesh_entries = {
    {"board", "meshes/plane.bin"},
    {"pawn", "meshes/pawn.bin"},
    {"bishop", "meshes/bishop.bin"},
    {"knight", "meshes/knight.bin"},
    {"rook", "meshes/rook.bin"},
    {"queen", "meshes/queen.bin"},
    {"king", "meshes/king.bin"},
    {"unknown", "meshes/unknown.bin"}
}

scale = 0.05

for i,v in ipairs(mesh_entries) do
    load_mesh(v[1], v[2], scale)
end

-- setup models
