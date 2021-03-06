local running = true

function exit()
	running = false
end

print("Type exit() to close")

while os.mainLoop() and running do
	local input = io.read()
	if input and string.len(input) > 0 then
		print(">", input)
		local ret, error = load(input)
		if error then
			shell.printError(error)
		else
			local ret = {pcall(ret)}
			if ret[1] == false then
				shell.printError(ret[2])
			else
				--print("<", ret[2])
			end
		end
	end
end

print("Lua Shell Closed")
