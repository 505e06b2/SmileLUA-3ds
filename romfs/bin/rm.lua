local args = {...}

local function printHelp()
	print("Usage: rm FILE...")
end

if #args > 0 then
	local remove_files = {}

	for _, value in ipairs(args) do
		if value == "-h" or value == "--help" then
			printHelp()
			remove_files = {}
			break
		else
			table.insert(remove_files, value)
		end
	end

	if #remove_files > 0 then
		for _, value in ipairs(remove_files) do
			local ret, error = os.remove(value)
			if error then
				shell.printError(error)
			end
		end
	end
else
	printHelp()
end
