local args = {...}

local function printHelp()
	print("Usage: wget URL [FILENAME]")
end

if #args >= 1 then
	local filename = "dl"
	if args[2] then
		filename = args[2]
	end

	local text = io.http.get(args[1])
	local f = io.open(filename, "w")
	f:write(text)
	f:close()
	print(string.format("Written \"%s\" to disk", filename))
else
	printHelp()
end
