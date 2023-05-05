local frandom = io.open("/dev/urandom", "rb")
local d = frandom:read(4)
math.randomseed(d:byte(1) + (d:byte(2) * 256) + (d:byte(3) * 65536) + (d:byte(4) * 4294967296))

number =  math.random(1,100000)
request = function()
    headers = {}
    headers["Content-Type"] = "application/json"
    headers["Authorization"] = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE2ODMxMzY3NTMuMTM3NzkyLCJpZCI6MiwibG9naW4iOiI0dnlPc29aIiwic3ViIjoibG9naW4ifQ.AymUX47VW0enwrJNA4II_BZD4b3IvKuIPHK4V72TMn4"
    body = ''
    return wrk.format("GET", "/user?id=".. tostring(number), headers, body)
end