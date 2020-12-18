local args = {...}

local function printHelp()
	print("Usage: wget URL [FILENAME]")
end

local block_size = 1024*1024 --1MB

if #args >= 1 then
	local filename = "dl"
	if args[2] then
		filename = args[2]
	end

	io.write("Downloading ", args[1], "\n")

	local r = io.http.get(args[1])
	local f = io.open(filename, "w")
	local written = 0

	local block, more = "", true
	while more do
		block, more = r:read(block_size)
		f:write(block)
		written = written + #block
		io.write("\rWritten ", written, " bytes")
	end
	f:close()
	r:close()
	print(string.format("\nWritten \"%s\" to disk", filename))
else
	printHelp()
end
