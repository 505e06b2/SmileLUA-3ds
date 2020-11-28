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

	printError = function(...)
		io.write("\x1b[31m")
		io.write(...)
		io.write("\x1b[0m\n")
	end,

	run = function(path, ...)
		local first_char = path:sub(1,1)
		local check_paths = {}

		if first_char == "." or first_char == "/" or path:sub(1,5) == "sdmc:" or path:sub(1,6) == "romfs:" then
			check_paths = {path, path .. ".lua"}
		else
			for x in shell.path:gmatch("([^;]+)") do
				table.insert(check_paths, x .. "/" .. path)
				table.insert(check_paths, x .. "/" .. path .. ".lua")
			end
		end

		for _, value in ipairs(check_paths) do
			local ret, error = loadfile(value)
			if error and error ~= ("cannot open " .. value .. ": No such file or directory") then --skip this error, but return any "real" ones
				return nil, error
			elseif ret then
				return pcall(ret, ...)
			end
		end

		return nil, path .. ": command not found"
	end
}

print("SmileLUA - " .. _VERSION)
os.chdir("sdmc:/")
while os.mainLoop() do
	local controls = io.readControls()
	if controls["Select"] and controls["Start"] then break end

	input = io.read(os.getcwd())
	if input and string.len(input) > 0 then
		print("\x1b[36m$\x1b[0m " .. input)
		local tokens = tokenise(input)
		if tokens[1] == "exit" then
			break
		elseif tokens[1] == "echo" then
			print(table.unpack(tokens, 2), " ")
		elseif tokens[1] == "cd" then
			if not os.chdir(tokens[2]) then
				shell.printError(tokens[2] .. ": no such directory")
			end
		else
			local ret, error = shell.run(tokens[1], table.unpack(tokens, 2))
			if error then
				shell.printError(error)
			end
		end
	end
end
