

%echoudp('on',8051)
u = udp('127.0.0.1',8051);

fopen(u)

fwrite(u, 'test')

fclose(u)
