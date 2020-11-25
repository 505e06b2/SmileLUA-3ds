local function tokenise(...)
    local sLine = table.concat({ ... }, " ")
    local tWords = {}
    local bQuoted = false
    for match in string.gmatch(sLine .. "\"", "(.-)\"") do
        if bQuoted then
            table.insert(tWords, match)
        else
            for m in string.gmatch(match, "[^ \t]+") do
                table.insert(tWords, m)
            end
        end
        bQuoted = not bQuoted
    end
    return tWords
end

_G["shell"] = {}
_G["shell"] = {
	path = "sdmc:/smilelua/bin;romfs:/bin",
	run = function(path, ...)
		local first_char = path:sub(1,1)

		if first_char == "." or first_char == "/" or path:sub(1,5) == "sdmc:" or path:sub(1,6) == "romfs:" then
			local ret, error = loadfile(path)
			if ret == nil then return nil, error end
			return pcall(ret, ...)

		else
			for x in shell.path:gmatch("([^;]+)") do
				local ret, error = loadfile(x .. "/" .. path)
				if ret then
					return pcall(ret, ...)
				end
			end
		end
		return nil, path .. " not found"
	end
}

print("SmileLUA - " .. _VERSION)
os.chdir("sdmc:/")
while os.mainLoop() do
	input = io.read()
	if input and string.len(input) > 0 then
		print("$ " .. input)
		local tokens = tokenise(input)
		if tokens[1] == "exit" then
			break
		else
			local ret, error = shell.run(tokens[1], table.unpack(tokens, 2))
			if error then
				print(error)
			end
		end
	end
end
