local t = {}

function t:Do()
    local tbl = {1, 2, 3, key1 = 5, key2 = 7, name = "john"}
    print("traverse by lua:")
    for key, val in pairs(tbl) do
        print(tostring(key), ":", tostring(val))
    end

    TraverseTable(tbl)
end

return t