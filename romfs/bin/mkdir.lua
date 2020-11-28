local args = {...}

local function printHelp()
	print("Usage: mkdir FOLDER...")
end

if #args > 0 then
	local create_folders = {}

	for _, value in ipairs(args) do
		if value == "-h" or value == "--help" then
			printHelp()
			create_folders = {}
			break
		else
			table.insert(create_folders, value)
		end
	end

	if #create_folders > 0 then
		for _, value in ipairs(create_folders) do
			if not os.mkdir(value) then
				shell.printError("mkdir: " .. value .. ": could not create directory")
			end
		end
	end
else
	printHelp()
end
