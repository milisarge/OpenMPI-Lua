--constants
num_intervals=20
PI=3.1415926535


function main(rank,size,sum)
	_rank=rank
	_size=size
	_sum=sum
	--return {oper="_",msg="hello"}
	if _rank == 0 then
		print("i am master")
		return recv()
	else
		return send()
	end
end

function main2(rank,size,sum)
	_rank=rank
	_size=size
	_sum=sum

	bcast(num_intervals)
	
	rect_width = PI / num_intervals
	partial_sum = 0
	for i = _rank + 1 , num_intervals + 1 , _size do
		x_middle = (i - 0.5) * rect_width
		area =  math.sin(x_middle) * rect_width 
		partial_sum = partial_sum + area
	end

    print( _rank,":",partial_sum,tonumber(_sum))
    
    reduce(partial_sum,_sum)

	--finish("-----")
end


function finish(_msg)
	return {oper="_",msg=_msg}
end

function send()
	return {oper="send",msg=_rank.."_greetings"}
end

function recv()
	return {oper="recv"}
end

function bcast(_msg)	
	print("bcasting")
	return {oper="bcast",msg=_msg}
end

function reduce(_part,_sum)
	print("reducing")
	return {oper="reduce",part=_part,sum=_sum}
end
