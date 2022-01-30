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
    if load_mesh(v[1], v[2], scale) < 0 then
        print("Unable to load: " .. v[2])
    end
end