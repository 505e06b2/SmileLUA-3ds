local args = {...}

local function printHelp()
	print("Usage: less FILE")
end

local term_info = {width=40, height=30, size=40*30} --chars
local lines = {}

if #args >= 1 then
	local source = io.open(args[1], "r")
	if source then
		local text = source:read("*line")
		while text do
			x = text:gsub("\t", "  ") --tab to space
			--x = x:gsub("\\", "\\") --escape backslashes -> NOT NEEDED
			table.insert(lines, x) --turn tabs into X spaces - use os.terminal.tab_size in the future
			text = source:read("*line")
		end

		if #lines == 0 then
			table.insert(lines, "")
		end
		source:close()
	else
		shell.printError("less: " .. args[1] .. ": could not be opened")
		return
	end
else
	printHelp()
	return
end

local current_line = 1

function generateText(start) --can't lose line info, but need to fit window bounds
	local out = {}
	local y = 1
	local height = term_info.height*2

	for i=start,#lines,1 do
		local line_text = lines[i]
		local line_out = {line_no=i, text=""}

		--empty
		if #line_text < 1 then
			line_out.text = string.rep(" ", term_info.width)
			table.insert(out, line_out)
		end

		--not empty
		while #line_text >= 1 do
			line_out.text = string.format("%-40s", line_text:sub(1,term_info.width))
			line_text = line_text:sub(term_info.width+1)
			table.insert(out, line_out)
			line_out = {text=""}
		end

		if #out >= height then
			return {table.unpack(out, 1, height-1)} --make sure it fits
		end
	end

	return out --empty
end

function printText(add_to_index)
	io.write("\x1b[2J")
	io.writeBottom("\x1b[2J")
	if add_to_index then
		current_line = current_line + add_to_index
	end

	--outside the if above, since current_line can be altered outside this function
	if not type(current_line) == "number" or current_line < 1 then current_line = 1
	elseif current_line > #lines then current_line = #lines end

	local text = generateText(current_line)

	for i, value in ipairs(text) do
		if i < term_info.height then
			if value.line_no then

				local line_number = tostring(value.line_no)
				if #line_number > 5 then
					line_number = "100k+"
				end

				line_number = string.format("%-5s", line_number)

				io.write("\x1b[33m", line_number, "\x1b[0m")
			else
				io.write("     ")
			end
			io.write(value.text, "\n")
		else
			io.writeBottom(value.text)
		end
	end
end

printText()
local controls = io.readControls()
local cooldown = false
while os.mainLoop() and not controls["Start"] do
	controls = io.readControls()
	if io.readTouchscreen() then
		local text = io.read("1")
		if text then
			local line_no = tonumber(text)
			if line_no then
				current_line = math.floor(line_no)
				printText()
			end
		end

	elseif cooldown and (
		controls["Up"] or controls["Down"] or
		controls["L"] or controls["R"] or
		controls["ZL"] or controls["ZR"]
	) then --skip
	elseif controls["Up"] then
		cooldown = true
		printText(-1)
	elseif controls["Down"] then
		cooldown = true
		printText(1)
	elseif controls["L"] then
		cooldown = true
		printText(-5)
	elseif controls["ZL"] then
		cooldown = true
		printText(-10)
	elseif controls["R"] then
		cooldown = true
		printText(5)
	elseif controls["ZR"] then
		cooldown = true
		printText(10)
	else
		cooldown = false
	end
end

io.write("\x1b[2J")
io.writeBottom("\x1b[2J")
