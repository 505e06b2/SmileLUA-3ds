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

local function printHelp()
	io.writeBottom("\x1b[2J")
	io.writeBottom("\n")
	io.writeBottom("\n")
	io.writeBottom("\n")
	io.writeBottom("\n")
	io.writeBottom("\n")
	io.writeBottom("\n")
	io.writeBottom("\n")
	io.writeBottom("\n")
	io.writeBottom("           \x1b[36;1m[\x1b[0m Execute \"qr\" \x1b[36;1m]\x1b[0m\n")
	io.writeBottom("         \x1b[31;1mScan QR To Enter Text\x1b[0m\n")
	io.writeBottom("\n")
	io.writeBottom("         \x1b[36;1m[\x1b[0m A \x1b[36;1m|\x1b[0m Right \x1b[36;1m|\x1b[0m Touch \x1b[36;1m]\x1b[0m\n")
	io.writeBottom("             \x1b[31;1mOpen Keyboard\x1b[0m\n")
	io.writeBottom("\n")
	io.writeBottom("              \x1b[36;1m[\x1b[0m Y \x1b[36;1m|\x1b[0m Left \x1b[36;1m]\x1b[0m\n")
	io.writeBottom("            \x1b[31;1mExecute Command\x1b[0m\n")
	io.writeBottom("\n")
	io.writeBottom("         \x1b[36;1m[\x1b[0m Up \x1b[36;1m|\x1b[0m X \x1b[36;1m/\x1b[0m Down \x1b[36;1m|\x1b[0m B \x1b[36;1m]\x1b[0m\n")
	io.writeBottom("             \x1b[31;1mInput History\x1b[0m\n")
	io.writeBottom("\n")
	io.writeBottom("          \x1b[36;1m[\x1b[0m Select \x1b[36;1m+\x1b[0m Start \x1b[36;1m]\x1b[0m\n")
	io.writeBottom("                 \x1b[31;1mExit\x1b[0m\n")
end


local chars_per_line = 50 --CRASH IF \b AT START OF LINE
local function replaceLine(old, new)
	local old_line = shell.ps1() .. old .. "_"
	local lines_spanned = math.floor(string.len(old_line)/chars_per_line)
	io.write(string.format("\x1b[%dA\r", lines_spanned))
	io.write(string.rep(" ", string.len(old_line)))
	io.write(string.format("\x1b[%dA\r", lines_spanned))
	io.write(shell.ps1() .. new)
end

local input = ""
local history = {"qr"} --add qr immediately, so technically no touch has to be performed
local history_index = 0
local history_cooldown = false --this is for repeating keys
local function moveHistory(direction)
	history_cooldown = true
	history_index = history_index + direction
	if history_index < 1 then
		history_index = 1
	elseif history_index > #history then
		history_index = #history+1
		replaceLine(input, "")
		io.write("_")
		input = ""
	else --already at last point
		replaceLine(input, history[history_index])
		io.write("_")
		input = history[history_index]
	end
end

_G["shell"] = {}
_G["shell"] = {
	path = "sdmc:/smilelua/bin;romfs:/bin",

	ps1 = function() return "\x1b[36;1m" .. os.getcwd():sub(1,-2) .. "$\x1b[0m " end,

	printError = function(...)
		io.write("\x1b[31;1m")
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
printHelp()
io.write(shell.ps1() .. "_")

while os.mainLoop() do
	local controls = io.readControls()
	if controls["Select"] and controls["Start"] then
		break

	elseif controls["Y"] or controls["Left"] then
		if input == "qr" then
			local new_input = io.readQR()
			if new_input then
				if string.len(new_input) > 0 then table.insert(history, new_input) end
				history_index = #history
			end
			input = new_input or ""
			io.write("\x1b[2J") --clear framebuffer
			printHelp()
			io.write(shell.ps1() .. input .. "_")

		elseif input and string.len(input) > 0 then
			replaceLine(input, input) --removes "_"
			io.write("\n")
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
			io.write(shell.ps1() .. "_")
			input = ""
			history_index = #history+1
		end

	elseif controls["A"] or controls["Right"] or io.readTouchscreen() then
		local new_input = io.read(input) --prefilled with current input
		if new_input then
			replaceLine(input, new_input)
			io.write("_")
			input = new_input
			if string.len(input) > 0 then table.insert(history, input) end
			history_index = #history
		end

	elseif history_cooldown and (controls["Up"] or controls["Down"] or controls["X"] or controls["B"]) then
	elseif controls["Up"] or controls["X"] then
		moveHistory(-1)
	elseif controls["Down"] or controls["B"] then
		moveHistory(1)
	else
		history_cooldown = false
	end
end
