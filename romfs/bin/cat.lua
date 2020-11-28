local args = {...}

local function printHelp()
	print("Usage: cat FILE...")
end

if #args > 0 then
	local read_files = {}

	for _, value in ipairs(args) do
		if value == "-h" or value == "--help" then
			printHelp()
			read_files = {}
			break
		else
			table.insert(read_files, value)
		end
	end

	if #read_files > 0 then
		for _, value in ipairs(read_files) do
			if os.isfile(value) then
				local f = io.open(value, "r")
				if f then
					print(f:read())
					f:close()
				end
			else
				shell.printError("cat: " .. value .. ": is not a file")
			end
		end
	end
else
	printHelp()
end
