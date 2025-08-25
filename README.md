# web_server_c
Minimal single-threaded web server written in C

### compile
```make main```

### run
```PORT=8080 ./main```

### docker build
```docker build -t web_server_c .```

### docker run
```docker run --rm --init -e PORT=8080 -p 8080:8080 web_server_c```

### demo
https://marekm4.com/
