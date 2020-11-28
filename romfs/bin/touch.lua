local args = {...}

local function printHelp()
	print("Usage: touch FILE...")
end

if #args > 0 then
	local create_files = {}

	for _, value in ipairs(args) do
		if value == "-h" or value == "--help" then
			printHelp()
			create_files = {}
			break
		else
			table.insert(create_files, value)
		end
	end

	if #create_files > 0 then
		for _, value in ipairs(create_files) do
			if not os.isfile(value) and not os.isdir(value) then
				local f = io.open(value, "w")
				if f then
					f:close()
				else
					shell.printError(value..": could not create file")
				end
			end
		end
	end
else
	printHelp()
end
