local args = {...}

local function printHelp()
	print("Usage: mv SOURCE DESTINATION")
end

if #args == 2 then
	local source = io.open(args[1], "rb")
	if source then
		local destination = io.open(args[2], "wb")
		if destination then
			destination:write(source:read() or "")
			destination:close()
			source:close()

			local ret, error = os.remove(args[1])
			if error then
				shell.printError(error)
			end
		else
			shell.printError("cp: " .. destination .. ": could not be opened")
		end
	else
		shell.printError("cp: " .. source .. ": could not be opened")
	end
else
	printHelp()
end
