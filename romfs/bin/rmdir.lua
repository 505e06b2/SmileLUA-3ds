local args = {...}

local function printHelp()
	print("Usage: rmdir FOLDER...")
end

if #args > 0 then
	local remove_folders = {}

	for _, value in ipairs(args) do
		if value == "-h" or value == "--help" then
			printHelp()
			remove_folders = {}
			break
		else
			table.insert(remove_folders, value)
		end
	end

	if #remove_folders > 0 then
		for _, value in ipairs(remove_folders) do
			if not os.rmdir(value) then
				shell.printError("rmdir: " .. value .. ": could not remove directory")
			end
		end
	end
else
	printHelp()
end
