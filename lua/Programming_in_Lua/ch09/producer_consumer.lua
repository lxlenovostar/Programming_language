--[[
function producer ()
	while true do
		local x = io.read() -- produce new value
		send(x) -- send to consumer
	end
end

function consumer ()
	while true do
		local x = receive() -- receive from producer
		io.write(x, "\n") -- consume new value
	end
end
]]

function send(x)
	coroutine.yield(x)
end

producer = coroutine.create(function ()
	while true do
		local x = io.read() -- produce new value
		print(x)
		send(x)
	end
end)

function receive()
	local status, value = coroutine.resume(producer)
	print(status)
	return value
end

value=receive()
print(value)
