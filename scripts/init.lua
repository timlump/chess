-- load all shaders
do
    local shader_entries = {
        {"id_shader", {"shaders/id.vert", "shaders/id.frag"}},
        {"shadow_shader", {"shaders/shadow.vert", "shaders/shadow.frag"}},
        {"board_shader", {"shaders/default.vert", "shaders/checker.frag"}},
        {"piece_shader", {"shaders/default.vert", "shaders/piece.frag"}},
        {"passthrough_shader", {"shaders/passthrough.vert", "shaders/passthrough.frag"}}
    }

    for i,v in ipairs(shader_entries) do
        shader.load(v[1],v[2][1],v[2][2])
    end
end

-- load all meshes
do
    local mesh_entries = {
        {"board", "meshes/plane.bin"},
        {"pawn", "meshes/pawn.bin"},
        {"bishop", "meshes/bishop.bin"},
        {"knight", "meshes/knight.bin"},
        {"rook", "meshes/rook.bin"},
        {"queen", "meshes/queen.bin"},
        {"king", "meshes/king.bin"},
        {"unknown", "meshes/unknown.bin"}
    }
end

-- setup board
mesh.create("board","meshes/plane.bin")
mesh.set_shader("board", 0, "board_shader")
mesh.set_shader("board", 1, "shadow_shader")

-- setup pieces
