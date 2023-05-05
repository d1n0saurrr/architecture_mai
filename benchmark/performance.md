# Описание проделанного benchmark
Benchmark был сделан с использованием wrk. Для измерения использовался метод получения пользователя по идентификатору.
Кэш реализован на Redis, пользователь ищется в кэше, если указать флаг. Если же пользователя нет в кэше, то он записывается.

# Описание скриптов:
- get_cache.lua - скрипт для получения пользователя с рандомным id из кэша
- get_no_cache.lua - скрипт для получения пользователя с рандомным id

# С кэшом:
```
Script: ./get_cache.lua, threads: 1, connections: 1
Running 10s test @ http://localhost:8082/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   143.17ms  391.06ms   1.83s    89.03%
    Req/Sec   662.71    102.18   757.00     79.27%
  Latency Distribution
     50%    1.45ms
     75%    1.82ms
     90%  648.93ms
     99%    1.71s 
  5456 requests in 10.01s, 2.39MB read
Requests/sec:    545.21
Transfer/sec:    244.39KB
-------------------------------------------------------
Script: ./get_cache.lua, threads: 2, connections: 2
Running 10s test @ http://localhost:8082/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.77ms  308.82us   4.26ms   74.87%
    Req/Sec   559.98     57.07   666.00     63.00%
  Latency Distribution
     50%    1.72ms
     75%    1.93ms
     90%    2.16ms
     99%    2.76ms
  11155 requests in 10.01s, 4.86MB read
Requests/sec:   1114.35
Transfer/sec:    496.78KB
-------------------------------------------------------
Script: ./get_cache.lua, threads: 5, connections: 5
Running 10s test @ http://localhost:8082/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   132.00ms  272.32ms   1.24s    85.50%
    Req/Sec   536.12    142.56   636.00     90.59%
  Latency Distribution
     50%    1.77ms
     75%   47.93ms
     90%  595.09ms
     99%    1.10s 
  18582 requests in 10.01s, 8.08MB read
Requests/sec:   1855.89
Transfer/sec:    826.48KB
-------------------------------------------------------
Script: ./get_cache.lua, threads: 10, connections: 10
Running 10s test @ http://localhost:8082/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   204.95ms  370.16ms   1.50s    82.71%
    Req/Sec   443.70    199.53   616.00     79.79%
  Latency Distribution
     50%    1.89ms
     75%  258.11ms
     90%  879.60ms
     99%    1.31s 
  13673 requests in 10.01s, 5.95MB read
  Socket errors: connect 3, read 0, write 0, timeout 0
Requests/sec:   1365.32
Transfer/sec:    608.41KB
-------------------------------------------------------
Script: ./get_cache.lua, threads: 20, connections: 20
Running 10s test @ http://localhost:8082/
  20 threads and 20 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   280.48ms  461.22ms   1.85s    81.56%
    Req/Sec   246.19    132.30   404.00     70.21%
  Latency Distribution
     50%    3.52ms
     75%  464.69ms
     90%    1.09s 
     99%    1.65s 
  12702 requests in 10.01s, 5.49MB read
  Socket errors: connect 3, read 0, write 0, timeout 0
Requests/sec:   1268.50
Transfer/sec:    561.82KB
```

# Без кэша:
```
Script: ./get_no_cache.lua, threads: 1, connections: 1
Running 10s test @ http://localhost:8082/
  1 threads and 1 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.31ms  448.82us  15.77ms   92.72%
    Req/Sec   432.25     32.11   494.00     78.00%
  Latency Distribution
     50%    2.22ms
     75%    2.38ms
     90%    2.59ms
     99%    3.64ms
  4306 requests in 10.00s, 1.84MB read
Requests/sec:    430.42
Transfer/sec:    188.73KB
----------------------------------------------------------
Script: ./get_no_cache.lua, threads: 2, connections: 2
Running 10s test @ http://localhost:8082/
  2 threads and 2 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.19ms  360.29us   4.62ms   77.42%
    Req/Sec   454.51     45.50   555.00     69.50%
  Latency Distribution
     50%    2.14ms
     75%    2.40ms
     90%    2.60ms
     99%    3.44ms
  9053 requests in 10.01s, 3.95MB read
Requests/sec:    904.64
Transfer/sec:    404.17KB
----------------------------------------------------------
Script: ./get_no_cache.lua, threads: 5, connections: 5
Running 10s test @ http://localhost:8082/
  5 threads and 5 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     8.83ms   35.35ms 331.65ms   95.83%
    Req/Sec   442.97     94.61   510.00     88.89%
  Latency Distribution
     50%    2.07ms
     75%    2.27ms
     90%    2.68ms
     99%  227.39ms
  16044 requests in 10.01s, 6.97MB read
  Socket errors: connect 0, read 0, write 0, timeout 5
Requests/sec:   1603.16
Transfer/sec:    712.99KB
----------------------------------------------------------
Script: ./get_no_cache.lua, threads: 10, connections: 10
Running 10s test @ http://localhost:8082/
  10 threads and 10 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    99.68ms  312.77ms   1.66s    91.24%
    Req/Sec   364.24    156.53   474.00     79.49%
  Latency Distribution
     50%    2.24ms
     75%    2.57ms
     90%  238.78ms
     99%    1.52s 
  11582 requests in 10.01s, 5.08MB read
  Socket errors: connect 2, read 0, write 0, timeout 16
Requests/sec:   1156.56
Transfer/sec:    519.14KB
----------------------------------------------------------
Script: ./get_no_cache.lua, threads: 20, connections: 20
Running 10s test @ http://localhost:8082/
  20 threads and 20 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   133.31ms  394.82ms   1.99s    90.35%
    Req/Sec   255.68    102.66   373.00     81.90%
  Latency Distribution
     50%    3.26ms
     75%    4.19ms
     90%  475.93ms
     99%    1.84s 
  11144 requests in 10.01s, 4.84MB read
  Socket errors: connect 2, read 0, write 0, timeout 30
Requests/sec:   1112.80
Transfer/sec:    494.77KB
```