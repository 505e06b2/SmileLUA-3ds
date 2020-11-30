local arg = ...
arg = arg or "."

local dir_contents = os.listdir(arg)
table.sort(dir_contents, function(a, b) return string.lower(a) < string.lower(b) end)
for i, file in ipairs(dir_contents) do
	local absolute_path = string.format("%s%s/%s", os.getcwd(), arg, file)
	io.write(os.isdir(absolute_path) and "\x1b[31;1m" or "")
	io.write(file .. "\x1b[0m\t")
	--if i % 3 == 0 then io.write("\n") end
end
io.write("\n")
